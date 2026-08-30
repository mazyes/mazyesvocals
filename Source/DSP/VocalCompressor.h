#pragma once
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>
#include <algorithm>

namespace MazyesDSP
{
    class VocalCompressor
    {
    public:
        VocalCompressor() = default;

        void prepare(double sampleRate, int maxBlockSize)
        {
            sr = sampleRate;
            envelope = 0.0f;
            currentGrDb = 0.0f;
            updateCoefficients();
        }

        void reset()
        {
            envelope = 0.0f;
            currentGrDb = 0.0f;
        }

        void setParameters(bool enabled, float threshDb, float ratioVal,
                           float attackMs, float releaseMs, float makeupGainDb, float kneeDb)
        {
            isEnabled = enabled;
            thresholdDb = threshDb;
            ratio = std::max(1.0f, ratioVal);
            attackTimeMs = attackMs;
            releaseTimeMs = releaseMs;
            makeupLinear = std::pow(10.0f, makeupGainDb / 20.0f);
            kneeWidthDb = std::max(0.0f, kneeDb);
            updateCoefficients();
        }

        void process(juce::AudioBuffer<float>& buffer, float& outMaxGainReductionDb)
        {
            if (!isEnabled)
            {
                outMaxGainReductionDb = 0.0f;
                return;
            }

            const int numSamples = buffer.getNumSamples();
            const int numChannels = buffer.getNumChannels();
            float* const* channelData = buffer.getArrayOfWritePointers();

            float maxGrInBlock = 0.0f;

            for (int i = 0; i < numSamples; ++i)
            {
                // Detect peak across channels
                float maxSample = 0.0f;
                for (int ch = 0; ch < numChannels; ++ch)
                {
                    float absVal = std::abs(channelData[ch][i]);
                    if (absVal > maxSample) maxSample = absVal;
                }

                // Envelope follower (peak/RMS blend)
                if (maxSample > envelope)
                    envelope += attackCoeff * (maxSample - envelope);
                else
                    envelope += releaseCoeff * (maxSample - envelope);

                float envDb = 20.0f * std::log10(std::max(envelope, 1e-5f));

                // Soft-knee gain computation
                float grDb = 0.0f;
                if (kneeWidthDb > 0.0f && envDb > (thresholdDb - kneeWidthDb * 0.5f) && envDb < (thresholdDb + kneeWidthDb * 0.5f))
                {
                    float delta = envDb - thresholdDb + kneeWidthDb * 0.5f;
                    float excess = (delta * delta) / (2.0f * kneeWidthDb);
                    grDb = excess * (1.0f - 1.0f / ratio);
                }
                else if (envDb >= (thresholdDb + kneeWidthDb * 0.5f))
                {
                    grDb = (envDb - thresholdDb) * (1.0f - 1.0f / ratio);
                }

                if (grDb > maxGrInBlock) maxGrInBlock = grDb;

                float gainLinear = std::pow(10.0f, -grDb / 20.0f) * makeupLinear;

                for (int ch = 0; ch < numChannels; ++ch)
                    channelData[ch][i] *= gainLinear;
            }

            outMaxGainReductionDb = -maxGrInBlock; // Negative dB
        }

    private:
        void updateCoefficients()
        {
            attackCoeff = static_cast<float>(1.0 - std::exp(-1.0 / (std::max(0.0001, attackTimeMs * 0.001) * sr)));
            releaseCoeff = static_cast<float>(1.0 - std::exp(-1.0 / (std::max(0.001, releaseTimeMs * 0.001) * sr)));
        }

        double sr { 44100.0 };
        bool isEnabled { true };
        float thresholdDb { -18.0f };
        float ratio { 4.0f };
        float attackTimeMs { 10.0f };
        float releaseTimeMs { 100.0f };
        float makeupLinear { 1.58f }; // +4 dB
        float kneeWidthDb { 4.0f };
        float attackCoeff { 0.1f };
        float releaseCoeff { 0.001f };
        float envelope { 0.0f };
        float currentGrDb { 0.0f };
    };
}
