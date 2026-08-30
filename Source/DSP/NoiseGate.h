#pragma once
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>
#include <algorithm>

namespace MazyesDSP
{
    class NoiseGate
    {
    public:
        NoiseGate() = default;

        void prepare(double sampleRate, int maxBlockSize)
        {
            sr = sampleRate;
            envelope = 0.0f;
            gain = 1.0f;
            updateCoefficients();
        }

        void reset()
        {
            envelope = 0.0f;
            gain = 1.0f;
        }

        void setParameters(bool enabled, float thresholdDb, float attackMs, float releaseMs, float rangeDb)
        {
            isEnabled = enabled;
            threshLinear = std::pow(10.0f, thresholdDb / 20.0f);
            attackTimeMs = attackMs;
            releaseTimeMs = releaseMs;
            rangeLinear = std::pow(10.0f, rangeDb / 20.0f);
            updateCoefficients();
        }

        void process(juce::AudioBuffer<float>& buffer, float& currentGainReductionDb)
        {
            if (!isEnabled)
            {
                currentGainReductionDb = 0.0f;
                return;
            }

            const int numSamples = buffer.getNumSamples();
            const int numChannels = buffer.getNumChannels();
            float* const* channelData = buffer.getArrayOfWritePointers();

            float minGainLinear = 1.0f;

            for (int i = 0; i < numSamples; ++i)
            {
                // Detect peak across channels
                float maxSample = 0.0f;
                for (int ch = 0; ch < numChannels; ++ch)
                {
                    float absVal = std::abs(channelData[ch][i]);
                    if (absVal > maxSample) maxSample = absVal;
                }

                // Envelope follower
                if (maxSample > envelope)
                    envelope += attackCoeff * (maxSample - envelope);
                else
                    envelope += releaseCoeff * (maxSample - envelope);

                // Compute target gain
                float targetGain = (envelope >= threshLinear) ? 1.0f : rangeLinear;

                // Smooth gain transition
                if (targetGain > gain)
                    gain += attackCoeff * (targetGain - gain);
                else
                    gain += releaseCoeff * (targetGain - gain);

                if (gain < minGainLinear) minGainLinear = gain;

                // Apply gate gain
                for (int ch = 0; ch < numChannels; ++ch)
                    channelData[ch][i] *= gain;
            }

            currentGainReductionDb = (minGainLinear < 0.999f) ? (20.0f * std::log10(std::max(minGainLinear, 1e-4f))) : 0.0f;
        }

    private:
        void updateCoefficients()
        {
            attackCoeff = static_cast<float>(1.0 - std::exp(-1.0 / (std::max(0.0001, attackTimeMs * 0.001) * sr)));
            releaseCoeff = static_cast<float>(1.0 - std::exp(-1.0 / (std::max(0.001, releaseTimeMs * 0.001) * sr)));
        }

        double sr { 44100.0 };
        bool isEnabled { false };
        float threshLinear { 0.003f }; // -50 dB
        float attackTimeMs { 2.0f };
        float releaseTimeMs { 100.0f };
        float rangeLinear { 0.001f }; // -60 dB
        float attackCoeff { 0.1f };
        float releaseCoeff { 0.001f };
        float envelope { 0.0f };
        float gain { 1.0f };
    };
}
