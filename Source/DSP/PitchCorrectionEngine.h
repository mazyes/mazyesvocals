#pragma once
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include "PitchDetector.h"
#include "PitchShifter.h"
#include "AudioBufferFIFO.h"
#include <array>
#include <cmath>

namespace MazyesDSP
{
    enum class ScaleType
    {
        Major = 0,
        Minor,
        HarmonicMinor,
        MelodicMinor,
        Chromatic
    };

    class PitchCorrectionEngine
    {
    public:
        PitchCorrectionEngine() = default;

        void prepare(double sampleRate, int maxBlockSize)
        {
            sr = sampleRate;
            detector.prepare(sampleRate, maxBlockSize);
            shifterL.prepare(sampleRate, maxBlockSize);
            shifterR.prepare(sampleRate, maxBlockSize);

            // Formant peak filter
            formantFilterL.reset();
            formantFilterR.reset();
            updateFormantFilters(1.0f);

            currentCorrectionRatio = 1.0f;
            targetCorrectionRatio = 1.0f;
            currentSmoothMidi = 60.0f;
            lastTargetMidi = 60;
        }

        void reset()
        {
            detector.reset();
            shifterL.reset();
            shifterR.reset();
            currentCorrectionRatio = 1.0f;
            targetCorrectionRatio = 1.0f;
        }

        void setParameters(bool enabled, int rootKey, ScaleType scaleType,
                           float speedMs, float amountPct, float transitionMs,
                           float humanizePct, float formantShiftSemitones)
        {
            isEnabled = enabled;
            selectedRootKey = std::clamp(rootKey, 0, 11);
            selectedScaleType = scaleType;
            retuneSpeedMs = speedMs;
            correctionAmount = amountPct * 0.01f;
            transitionTimeMs = transitionMs;
            humanizeAmount = humanizePct * 0.01f;
            formantShift = formantShiftSemitones;

            float formantRatio = std::pow(2.0f, formantShift / 12.0f);
            updateFormantFilters(formantRatio);
        }

        void process(juce::AudioBuffer<float>& buffer, LockFreeFIFO<PitchTelemetry>& telemetryQueue)
        {
            if (!isEnabled)
            {
                // Send bypass telemetry
                PitchTelemetry t;
                t.isVoiced = false;
                t.detectedFreqHz = 0.0f;
                t.targetFreqHz = 0.0f;
                t.detectedMidiNote = -1;
                t.targetMidiNote = -1;
                telemetryQueue.push(t);
                return;
            }

            const int numSamples = buffer.getNumSamples();
            const float* inL = buffer.getReadPointer(0);
            const float* inR = buffer.getNumChannels() > 1 ? buffer.getReadPointer(1) : inL;
            float* outL = buffer.getWritePointer(0);
            float* outR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : outL;

            PitchTelemetry lastTelemetry;
            bool hasTelemetry = false;

            for (int i = 0; i < numSamples; ++i)
            {
                // Mono sum for pitch detection
                float monoIn = 0.5f * (inL[i] + inR[i]);
                float detectedFreq = 0.0f;
                float confidence = 0.0f;
                bool isVoiced = false;

                detector.processSample(monoIn, detectedFreq, confidence, isVoiced);

                if (isVoiced && detectedFreq > 40.0f && detectedFreq < 1200.0f)
                {
                    // Convert frequency to fractional MIDI Note
                    float detectedMidi = 69.0f + 12.0f * std::log2(detectedFreq / 440.0f);
                    int targetMidi = quantizeToScale(detectedMidi, selectedRootKey, selectedScaleType);
                    float targetFreq = 440.0f * std::pow(2.0f, (static_cast<float>(targetMidi) - 69.0f) / 12.0f);

                    // Humanize: preserve small micro-tonal vibrato around target note
                    float midiDiff = detectedMidi - static_cast<float>(targetMidi);
                    float humanizedTargetMidi = static_cast<float>(targetMidi) + midiDiff * humanizeAmount * 0.4f;

                    // Retune speed smoothing (0 ms = hard snap, 100 ms = natural)
                    float timeConst = std::max(0.0001f, retuneSpeedMs * 0.001f);
                    float alpha = static_cast<float>(1.0 - std::exp(-1.0 / (timeConst * sr)));
                    if (retuneSpeedMs < 0.5f) alpha = 1.0f; // Instant hard snap

                    currentSmoothMidi += alpha * (humanizedTargetMidi - currentSmoothMidi);

                    float correctedFreq = 440.0f * std::pow(2.0f, (currentSmoothMidi - 69.0f) / 12.0f);
                    float rawPitchRatio = correctedFreq / detectedFreq;

                    // Blend with original pitch based on Amount
                    float finalPitchRatio = 1.0f + (rawPitchRatio - 1.0f) * correctionAmount;
                    targetCorrectionRatio = finalPitchRatio;

                    // Telemetry
                    lastTelemetry.detectedFreqHz = detectedFreq;
                    lastTelemetry.targetFreqHz = targetFreq;
                    lastTelemetry.detectedMidiNote = static_cast<int>(std::round(detectedMidi));
                    lastTelemetry.targetMidiNote = targetMidi;
                    lastTelemetry.centDeviation = (detectedMidi - static_cast<float>(targetMidi)) * 100.0f;
                    lastTelemetry.confidence = confidence;
                    lastTelemetry.isVoiced = true;
                    hasTelemetry = true;
                }
                else
                {
                    targetCorrectionRatio = 1.0f;
                    if (i == 0)
                    {
                        lastTelemetry.isVoiced = false;
                        lastTelemetry.detectedFreqHz = 0.0f;
                        lastTelemetry.targetFreqHz = 0.0f;
                        lastTelemetry.detectedMidiNote = -1;
                        lastTelemetry.targetMidiNote = -1;
                        lastTelemetry.centDeviation = 0.0f;
                        lastTelemetry.confidence = 0.0f;
                        hasTelemetry = true;
                    }
                }

                shifterL.setPitchRatio(targetCorrectionRatio);
                shifterR.setPitchRatio(targetCorrectionRatio);

                float shiftedL = shifterL.processSample(inL[i]);
                float shiftedR = shifterR.processSample(inR[i]);

                // Apply formant shaping
                if (std::abs(formantShift) > 0.1f)
                {
                    shiftedL = formantFilterL.processSample(shiftedL);
                    shiftedR = formantFilterR.processSample(shiftedR);
                }

                outL[i] = inL[i] * (1.0f - correctionAmount) + shiftedL * correctionAmount;
                outR[i] = inR[i] * (1.0f - correctionAmount) + shiftedR * correctionAmount;
            }

            if (hasTelemetry)
            {
                telemetryQueue.push(lastTelemetry);
            }
        }

        static int quantizeToScale(float midiNote, int rootKey, ScaleType scaleType)
        {
            int roundNote = static_cast<int>(std::round(midiNote));
            if (scaleType == ScaleType::Chromatic)
                return roundNote;

            const auto& scaleIntervals = getScaleIntervals(scaleType);

            int bestNote = roundNote;
            float minDistance = 999.0f;

            // Search nearest scale note within +/- 12 semitones
            for (int offset = -12; offset <= 12; ++offset)
            {
                int candidateNote = roundNote + offset;
                int pitchClass = (candidateNote - rootKey) % 12;
                if (pitchClass < 0) pitchClass += 12;

                bool inScale = false;
                for (int interval : scaleIntervals)
                {
                    if (pitchClass == interval)
                    {
                        inScale = true;
                        break;
                    }
                }

                if (inScale)
                {
                    float dist = std::abs(midiNote - static_cast<float>(candidateNote));
                    if (dist < minDistance)
                    {
                        minDistance = dist;
                        bestNote = candidateNote;
                    }
                }
            }

            return bestNote;
        }

        static std::vector<int> getScaleIntervals(ScaleType scale)
        {
            switch (scale)
            {
                case ScaleType::Major:
                    return {0, 2, 4, 5, 7, 9, 11};
                case ScaleType::Minor:
                    return {0, 2, 3, 5, 7, 8, 10};
                case ScaleType::HarmonicMinor:
                    return {0, 2, 3, 5, 7, 8, 11};
                case ScaleType::MelodicMinor:
                    return {0, 2, 3, 5, 7, 9, 11};
                case ScaleType::Chromatic:
                default:
                    return {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
            }
        }

    private:
        void updateFormantFilters(float formantRatio)
        {
            float centerFreq = std::clamp(2800.0f * formantRatio, 200.0f, 16000.0f);
            float q = 1.2f;
            float gainDb = (formantRatio - 1.0f) * 6.0f;
            float gain = std::pow(10.0f, gainDb / 20.0f);

            formantFilterL.setCoefficients(centerFreq, q, gain, sr);
            formantFilterR.setCoefficients(centerFreq, q, gain, sr);
        }

        struct SimpleBiquad
        {
            float b0 { 1.0f }, b1 { 0.0f }, b2 { 0.0f }, a1 { 0.0f }, a2 { 0.0f };
            float x1 { 0.0f }, x2 { 0.0f }, y1 { 0.0f }, y2 { 0.0f };

            void reset() { x1 = x2 = y1 = y2 = 0.0f; }

            void setCoefficients(float freq, float q, float A, double sampleRate)
            {
                float w0 = 2.0f * juce::MathConstants<float>::pi * freq / static_cast<float>(sampleRate);
                float alpha = std::sin(w0) / (2.0f * q);
                float cosw0 = std::cos(w0);

                float a0 = 1.0f + alpha / A;
                b0 = (1.0f + alpha * A) / a0;
                b1 = (-2.0f * cosw0) / a0;
                b2 = (1.0f - alpha * A) / a0;
                a1 = (-2.0f * cosw0) / a0;
                a2 = (1.0f - alpha / A) / a0;
            }

            inline float processSample(float in)
            {
                float out = b0 * in + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
                x2 = x1; x1 = in;
                y2 = y1; y1 = out;
                return out;
            }
        };

        double sr { 44100.0 };
        bool isEnabled { true };
        int selectedRootKey { 0 }; // 0 = C
        ScaleType selectedScaleType { ScaleType::Chromatic };
        float retuneSpeedMs { 0.0f };
        float correctionAmount { 1.0f };
        float transitionTimeMs { 5.0f };
        float humanizeAmount { 0.0f };
        float formantShift { 0.0f };

        float currentCorrectionRatio { 1.0f };
        float targetCorrectionRatio { 1.0f };
        float currentSmoothMidi { 60.0f };
        int lastTargetMidi { 60 };

        PitchDetector detector;
        PitchShifter shifterL;
        PitchShifter shifterR;
        SimpleBiquad formantFilterL;
        SimpleBiquad formantFilterR;
    };
}
