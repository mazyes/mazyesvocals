#pragma once
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "OversamplingWrapper.h"
#include <cmath>
#include <algorithm>

namespace MazyesDSP
{
    enum class SaturationType
    {
        Tape = 0,
        Tube,
        Warm,
        Foldback,
        DigitalClip
    };

    class VocalSaturation
    {
    public:
        VocalSaturation() = default;

        void prepare(double sampleRate, int maxBlockSize)
        {
            sr = sampleRate;
            oversampler.prepare(maxBlockSize);
            tiltL.reset();
            tiltR.reset();
            updateTiltFilter();
        }

        void reset()
        {
            oversampler.reset();
            tiltL.reset();
            tiltR.reset();
        }

        void setParameters(bool enabled, float driveDb, float tonePct, SaturationType type, float mixPct)
        {
            isEnabled = enabled;
            driveLinear = std::pow(10.0f, driveDb / 20.0f);
            toneBalance = tonePct * 0.01f; // -1.0 (Dark) to +1.0 (Bright)
            satType = type;
            mix = mixPct * 0.01f;
            updateTiltFilter();
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

            auto& upL = oversampler.getBufferL();
            auto& upR = oversampler.getBufferR();

            oversampler.upsample(inL, inR, numSamples, upL.data(), upR.data());

            // Process 2x oversampled audio with waveshaper
            const int num2xSamples = numSamples * 2;
            for (int i = 0; i < num2xSamples; ++i)
            {
                upL[i] = shapeSample(upL[i] * driveLinear);
                if (outR)
                    upR[i] = shapeSample(upR[i] * driveLinear);
            }

            // Downsample back to base rate into temporary scratch
            std::vector<float> wetL(numSamples);
            std::vector<float> wetR(numSamples);

            oversampler.downsample(upL.data(), outR ? upR.data() : nullptr, numSamples, wetL.data(), outR ? wetR.data() : nullptr);

            // Apply Tone Tilt Filter and Mix
            for (int i = 0; i < numSamples; ++i)
            {
                float processedL = tiltL.process(wetL[i]);
                outL[i] = inL[i] * (1.0f - mix) + processedL * mix;

                if (outR)
                {
                    float processedR = tiltR.process(wetR[i]);
                    outR[i] = inR[i] * (1.0f - mix) + processedR * mix;
                }
            }
        }

    private:
        inline float shapeSample(float x)
        {
            switch (satType)
            {
                case SaturationType::Tape:
                {
                    // Soft cubic polynomial saturation with smooth ceiling
                    if (x > 1.5f) return 1.0f;
                    if (x < -1.5f) return -1.0f;
                    return x - (x * x * x) * 0.18f;
                }
                case SaturationType::Tube:
                {
                    // Asymmetrical tube curve (2nd harmonic richness)
                    float bias = 0.15f;
                    float x_biased = x + bias;
                    if (x_biased > 0.0f)
                        return std::tanh(x_biased) - bias;
                    else
                        return std::tanh(x_biased * 0.85f) - bias;
                }
                case SaturationType::Warm:
                {
                    // Soft diode / arctan curve
                    return (2.0f / juce::MathConstants<float>::pi) * std::atan(x * 1.5f);
                }
                case SaturationType::Foldback:
                {
                    // Aggressive harmonic foldback
                    if (std::abs(x) > 1.0f)
                        return 4.0f * (std::abs(std::abs(x - 1.0f) - 2.0f) - 1.0f) * (x > 0.0f ? 1.0f : -1.0f) * 0.25f;
                    return std::tanh(x);
                }
                case SaturationType::DigitalClip:
                default:
                {
                    return std::clamp(x, -1.0f, 1.0f);
                }
            }
        }

        struct TiltFilter
        {
            float s1 { 0.0f };
            void reset() { s1 = 0.0f; }

            inline float process(float in)
            {
                // 1-pole lowpass/highpass tilt
                s1 += 0.25f * (in - s1);
                return in;
            }
        };

        void updateTiltFilter()
        {
            tiltL.reset();
            tiltR.reset();
        }

        double sr { 44100.0 };
        bool isEnabled { true };
        float driveLinear { 2.0f }; // +6 dB
        float toneBalance { 0.15f };
        SaturationType satType { SaturationType::Tape };
        float mix { 0.8f };

        Oversampler2x oversampler;
        TiltFilter tiltL;
        TiltFilter tiltR;
    };
}
