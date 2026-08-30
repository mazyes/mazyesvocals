#pragma once
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <cmath>
#include <algorithm>

namespace MazyesDSP
{
    class VocalDelay
    {
    public:
        VocalDelay() = default;

        void prepare(double sampleRate, int maxBlockSize)
        {
            sr = sampleRate;
            maxDelaySamples = static_cast<int>(sampleRate * 2.5); // 2.5s max
            bufferL.assign(maxDelaySamples, 0.0f);
            bufferR.assign(maxDelaySamples, 0.0f);
            writePos = 0;
            duckEnvelope = 0.0f;
            filterL.reset();
            filterR.reset();
            updateFilters();
        }

        void reset()
        {
            std::fill(bufferL.begin(), bufferL.end(), 0.0f);
            std::fill(bufferR.begin(), bufferR.end(), 0.0f);
            writePos = 0;
            duckEnvelope = 0.0f;
            filterL.reset();
            filterR.reset();
        }

        void setParameters(bool enabled, float timeMs, int syncDivision, double bpm,
                           float feedbackPct, bool pingPong, float duckingPct,
                           float lowCutHz, float highCutHz, float mixPct)
        {
            isEnabled = enabled;
            isPingPong = pingPong;
            feedback = std::clamp(feedbackPct * 0.01f, 0.0f, 0.95f);
            duckingAmount = duckingPct * 0.01f;
            mix = mixPct * 0.01f;
            lowCut = lowCutHz;
            highCut = highCutHz;

            // Compute delay time (free ms or BPM sync)
            float effectiveTimeMs = timeMs;
            if (syncDivision > 0 && bpm > 20.0 && bpm < 400.0)
            {
                double beatMs = (60.0 / bpm) * 1000.0;
                switch (syncDivision)
                {
                    case 1: effectiveTimeMs = static_cast<float>(beatMs * 0.25); break;  // 1/16
                    case 2: effectiveTimeMs = static_cast<float>(beatMs * 0.5); break;   // 1/8
                    case 3: effectiveTimeMs = static_cast<float>(beatMs * 1.0); break;   // 1/4
                    case 4: effectiveTimeMs = static_cast<float>(beatMs * 2.0); break;   // 1/2
                    case 5: effectiveTimeMs = static_cast<float>(beatMs * 0.75); break;  // 1/8D
                    case 6: effectiveTimeMs = static_cast<float>(beatMs * 0.3333); break;// 1/8T
                    case 7: effectiveTimeMs = static_cast<float>(beatMs * 1.5); break;   // 1/4D
                    case 8: effectiveTimeMs = static_cast<float>(beatMs * 0.6666); break;// 1/4T
                    default: effectiveTimeMs = timeMs; break;
                }
            }

            targetDelaySamplesL = std::clamp(static_cast<float>(effectiveTimeMs * 0.001 * sr), 10.0f, static_cast<float>(maxDelaySamples - 10));
            targetDelaySamplesR = isPingPong ? (targetDelaySamplesL * 1.5f) : targetDelaySamplesL;
            if (targetDelaySamplesR >= maxDelaySamples - 10) targetDelaySamplesR = targetDelaySamplesL;

            updateFilters();
        }

        void process(juce::AudioBuffer<float>& buffer)
        {
            if (!isEnabled || mix <= 0.001f) return;

            const int numSamples = buffer.getNumSamples();
            const int numChannels = buffer.getNumChannels();
            const float* inL = buffer.getReadPointer(0);
            const float* inR = numChannels > 1 ? buffer.getReadPointer(1) : inL;
            float* outL = buffer.getWritePointer(0);
            float* outR = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;

            const float duckAttack = static_cast<float>(1.0 - std::exp(-1.0 / (0.005 * sr))); // 5ms
            const float duckRelease = static_cast<float>(1.0 - std::exp(-1.0 / (0.200 * sr))); // 200ms

            for (int i = 0; i < numSamples; ++i)
            {
                float dryL = inL[i];
                float dryR = inR ? inR[i] : dryL;

                // Vocal ducking envelope follower
                float dryPeak = std::max(std::abs(dryL), std::abs(dryR));
                if (dryPeak > duckEnvelope)
                    duckEnvelope += duckAttack * (dryPeak - duckEnvelope);
                else
                    duckEnvelope += duckRelease * (dryPeak - duckEnvelope);

                float duckGain = 1.0f - std::clamp(duckEnvelope * duckingAmount * 2.0f, 0.0f, 0.85f);

                // Read from delay lines
                float readPosL = static_cast<float>(writePos) - targetDelaySamplesL;
                float readPosR = static_cast<float>(writePos) - targetDelaySamplesR;

                float delayedL = readLinear(bufferL, readPosL);
                float delayedR = readLinear(bufferR, readPosR);

                // Filter delayed feedback signal
                delayedL = filterL.process(delayedL);
                delayedR = filterR.process(delayedR);

                // Write feedback + input into buffers
                if (isPingPong)
                {
                    bufferL[writePos] = dryL + delayedR * feedback;
                    bufferR[writePos] = dryR + delayedL * feedback;
                }
                else
                {
                    bufferL[writePos] = dryL + delayedL * feedback;
                    bufferR[writePos] = dryR + delayedR * feedback;
                }

                writePos = (writePos + 1) % maxDelaySamples;

                // Apply ducked wet mix to output
                outL[i] = dryL + delayedL * mix * duckGain;
                if (outR)
                    outR[i] = dryR + delayedR * mix * duckGain;
            }
        }

    private:
        inline float readLinear(const std::vector<float>& buf, float readPos)
        {
            while (readPos < 0.0f) readPos += static_cast<float>(maxDelaySamples);
            while (readPos >= static_cast<float>(maxDelaySamples)) readPos -= static_cast<float>(maxDelaySamples);

            int idx0 = static_cast<int>(readPos);
            int idx1 = (idx0 + 1) % maxDelaySamples;
            float frac = readPos - static_cast<float>(idx0);

            return buf[idx0] + frac * (buf[idx1] - buf[idx0]);
        }

        struct DampingFilter
        {
            float sLP { 0.0f };
            float sHP { 0.0f };
            float alphaLP { 0.5f };
            float alphaHP { 0.5f };

            void reset() { sLP = sHP = 0.0f; }

            void setCoeffs(float lowCut, float highCut, double sampleRate)
            {
                float dt = 1.0f / static_cast<float>(sampleRate);
                float rcLP = 1.0f / (2.0f * juce::MathConstants<float>::pi * std::clamp(highCut, 1000.0f, 20000.0f));
                alphaLP = dt / (rcLP + dt);

                float rcHP = 1.0f / (2.0f * juce::MathConstants<float>::pi * std::clamp(lowCut, 20.0f, 1000.0f));
                alphaHP = rcHP / (rcHP + dt);
            }

            inline float process(float in)
            {
                // Lowpass
                sLP += alphaLP * (in - sLP);
                // Highpass
                float hpOut = alphaHP * (sHP + sLP - sHP);
                sHP = sLP;
                return sLP - (sHP * (1.0f - alphaHP));
            }
        };

        void updateFilters()
        {
            filterL.setCoeffs(lowCut, highCut, sr);
            filterR.setCoeffs(lowCut, highCut, sr);
        }

        double sr { 44100.0 };
        bool isEnabled { false };
        bool isPingPong { true };
        float feedback { 0.35f };
        float duckingAmount { 0.4f };
        float mix { 0.25f };
        float lowCut { 200.0f };
        float highCut { 6000.0f };

        int maxDelaySamples { 110250 };
        int writePos { 0 };
        float targetDelaySamplesL { 11025.0f };
        float targetDelaySamplesR { 16537.0f };
        float duckEnvelope { 0.0f };

        std::vector<float> bufferL;
        std::vector<float> bufferR;
        DampingFilter filterL;
        DampingFilter filterR;
    };
}
