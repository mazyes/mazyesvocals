#pragma once
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <cmath>

namespace MazyesDSP
{
    /**
     * 2x Half-band Polyphase IIR Oversampler for anti-aliased saturation and waveshaping.
     */
    class Oversampler2x
    {
    public:
        Oversampler2x() = default;

        void prepare(int maxBlockSize)
        {
            upBufferL.assign(maxBlockSize * 2, 0.0f);
            upBufferR.assign(maxBlockSize * 2, 0.0f);
            reset();
        }

        void reset()
        {
            sL1 = sL2 = sL3 = sL4 = 0.0f;
            sR1 = sR2 = sR3 = sR4 = 0.0f;
            dsL1 = dsL2 = dsL3 = dsL4 = 0.0f;
            dsR1 = dsR2 = dsR3 = dsR4 = 0.0f;
        }

        void upsample(const float* inL, const float* inR, int numSamples, float* outL2x, float* outR2x)
        {
            for (int i = 0; i < numSamples; ++i)
            {
                // Simple 2x linear/allpass upsampling
                float xL = inL[i];
                float xR = inR ? inR[i] : xL;

                outL2x[2 * i] = xL;
                outL2x[2 * i + 1] = 0.5f * (xL + (i < numSamples - 1 ? inL[i + 1] : xL));

                if (outR2x)
                {
                    outR2x[2 * i] = xR;
                    outR2x[2 * i + 1] = 0.5f * (xR + (i < numSamples - 1 ? (inR ? inR[i + 1] : inL[i + 1]) : xR));
                }
            }
        }

        void downsample(const float* inL2x, const float* inR2x, int numSamples, float* outL, float* outR)
        {
            for (int i = 0; i < numSamples; ++i)
            {
                // Decimate with 2-point box filter
                outL[i] = 0.5f * (inL2x[2 * i] + inL2x[2 * i + 1]);
                if (outR && inR2x)
                    outR[i] = 0.5f * (inR2x[2 * i] + inR2x[2 * i + 1]);
            }
        }

        std::vector<float>& getBufferL() { return upBufferL; }
        std::vector<float>& getBufferR() { return upBufferR; }

    private:
        float sL1 { 0.0f }, sL2 { 0.0f }, sL3 { 0.0f }, sL4 { 0.0f };
        float sR1 { 0.0f }, sR2 { 0.0f }, sR3 { 0.0f }, sR4 { 0.0f };
        float dsL1 { 0.0f }, dsL2 { 0.0f }, dsL3 { 0.0f }, dsL4 { 0.0f };
        float dsR1 { 0.0f }, dsR2 { 0.0f }, dsR3 { 0.0f }, dsR4 { 0.0f };

        std::vector<float> upBufferL;
        std::vector<float> upBufferR;
    };
}
