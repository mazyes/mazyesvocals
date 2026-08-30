#pragma once
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <cmath>
#include <algorithm>

namespace MazyesDSP
{
    /**
     * Transparent brickwall peak limiter with 1ms lookahead,
     * fast peak follower, and smooth exponential release to prevent inter-sample clipping.
     */
    class VocalLimiter
    {
    public:
        VocalLimiter() = default;

        void prepare(double sampleRate, int maxBlockSize)
        {
            sr = sampleRate;
            lookaheadSamples = std::max(8, static_cast<int>(sampleRate * 0.001)); // 1ms lookahead
            delayLineL.assign(lookaheadSamples, 0.0f);
            delayLineR.assign(lookaheadSamples, 0.0f);
            writePos = 0;
            currentGain = 1.0f;
            updateReleaseCoeff();
        }

        void reset()
        {
            std::fill(delayLineL.begin(), delayLineL.end(), 0.0f);
            std::fill(delayLineR.begin(), delayLineR.end(), 0.0f);
            writePos = 0;
            currentGain = 1.0f;
        }

        void setParameters(bool enabled, float ceilingDb, float releaseMs)
        {
            isEnabled = enabled;
            ceilingLinear = std::pow(10.0f, ceilingDb / 20.0f);
            releaseTimeMs = releaseMs;
            updateReleaseCoeff();
        }

        void process(juce::AudioBuffer<float>& buffer, float& outGainReductionDb)
        {
            if (!isEnabled)
            {
                outGainReductionDb = 0.0f;
                return;
            }

            const int numSamples = buffer.getNumSamples();
            const int numChannels = buffer.getNumChannels();
            float* outL = buffer.getWritePointer(0);
            float* outR = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;

            float minGainInBlock = 1.0f;

            for (int i = 0; i < numSamples; ++i)
            {
                float inL = outL[i];
                float inR = outR ? outR[i] : inL;

                // Lookahead buffer read & write
                float delayedL = delayLineL[writePos];
                float delayedR = delayLineR[writePos];

                delayLineL[writePos] = inL;
                delayLineR[writePos] = inR;
                writePos = (writePos + 1) % lookaheadSamples;

                // Peak detection on current input
                float peak = std::max(std::abs(inL), std::abs(inR));

                // Target gain to keep peak below ceiling
                float targetGain = (peak > ceilingLinear) ? (ceilingLinear / peak) : 1.0f;

                // Fast attack, smooth release
                if (targetGain < currentGain)
                    currentGain = targetGain; // Instant lookahead attack
                else
                    currentGain += releaseCoeff * (targetGain - currentGain);

                if (currentGain < minGainInBlock) minGainInBlock = currentGain;

                outL[i] = delayedL * currentGain;
                if (outR)
                    outR[i] = delayedR * currentGain;
            }

            outGainReductionDb = (minGainInBlock < 0.999f) ? (20.0f * std::log10(std::max(minGainInBlock, 1e-4f))) : 0.0f;
        }

    private:
        void updateReleaseCoeff()
        {
            releaseCoeff = static_cast<float>(1.0 - std::exp(-1.0 / (std::max(0.001, releaseTimeMs * 0.001) * sr)));
        }

        double sr { 44100.0 };
        bool isEnabled { true };
        float ceilingLinear { 0.965f }; // -0.3 dB
        float releaseTimeMs { 20.0f };
        float releaseCoeff { 0.01f };
        float currentGain { 1.0f };

        int lookaheadSamples { 48 };
        int writePos { 0 };
        std::vector<float> delayLineL;
        std::vector<float> delayLineR;
    };
}
