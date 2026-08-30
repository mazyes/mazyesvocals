#pragma once
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>
#include <algorithm>

namespace MazyesDSP
{
    /**
     * High-precision dynamic vocal de-esser with sidechain bandpass detector
     * and frequency-specific sibilance reduction.
     */
    class VocalDeEsser
    {
    public:
        VocalDeEsser() = default;

        void prepare(double sampleRate, int maxBlockSize)
        {
            sr = sampleRate;
            envelope = 0.0f;
            filterL.reset();
            filterR.reset();
            updateFilter(centerFreqHz, 2.0f);
        }

        void reset()
        {
            envelope = 0.0f;
            filterL.reset();
            filterR.reset();
        }

        void setParameters(bool enabled, float freqHz, float thresholdDb, float amountPct, bool listenMode)
        {
            isEnabled = enabled;
            centerFreqHz = std::clamp(freqHz, 3000.0f, 10000.0f);
            threshDb = thresholdDb;
            amountLinear = amountPct * 0.01f;
            isListen = listenMode;
            updateFilter(centerFreqHz, 2.0f);
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

            float maxReductionDb = 0.0f;
            const float attackCoeff = static_cast<float>(1.0 - std::exp(-1.0 / (0.001 * sr))); // 1ms attack
            const float releaseCoeff = static_cast<float>(1.0 - std::exp(-1.0 / (0.050 * sr))); // 50ms release

            for (int i = 0; i < numSamples; ++i)
            {
                float inSampleL = outL[i];
                float inSampleR = outR ? outR[i] : inSampleL;

                // Sidechain bandpass filter to isolate sibilant frequencies
                float sibilanceL = filterL.process(inSampleL);
                float sibilanceR = outR ? filterR.process(inSampleR) : sibilanceL;
                float sidechainPeak = std::max(std::abs(sibilanceL), std::abs(sibilanceR));

                // Envelope follower
                if (sidechainPeak > envelope)
                    envelope += attackCoeff * (sidechainPeak - envelope);
                else
                    envelope += releaseCoeff * (sidechainPeak - envelope);

                float envDb = 20.0f * std::log10(std::max(envelope, 1e-5f));

                float grDb = 0.0f;
                if (envDb > threshDb)
                {
                    grDb = (envDb - threshDb) * amountLinear;
                }

                if (grDb > maxReductionDb) maxReductionDb = grDb;

                float attenuation = std::pow(10.0f, -grDb / 20.0f);

                if (isListen)
                {
                    // Listen mode: output isolated sibilance
                    outL[i] = sibilanceL;
                    if (outR) outR[i] = sibilanceR;
                }
                else
                {
                    // Subtract sibilance proportion dynamically
                    outL[i] = (inSampleL - sibilanceL) + (sibilanceL * attenuation);
                    if (outR) outR[i] = (inSampleR - sibilanceR) + (sibilanceR * attenuation);
                }
            }

            outGainReductionDb = -maxReductionDb;
        }

    private:
        struct BandpassFilter
        {
            float b0 { 1.0f }, b1 { 0.0f }, b2 { 0.0f }, a1 { 0.0f }, a2 { 0.0f };
            float s1 { 0.0f }, s2 { 0.0f };

            void reset() { s1 = s2 = 0.0f; }

            inline float process(float in)
            {
                float out = b0 * in + s1;
                s1 = b1 * in - a1 * out + s2;
                s2 = b2 * in - a2 * out;
                return out;
            }

            void setBandpass(float freq, float q, double sampleRate)
            {
                float w0 = 2.0f * juce::MathConstants<float>::pi * freq / static_cast<float>(sampleRate);
                float alpha = std::sin(w0) / (2.0f * q);
                float cosw0 = std::cos(w0);

                float a0 = 1.0f + alpha;
                float invA0 = 1.0f / a0;
                b0 = (alpha) * invA0;
                b1 = 0.0f;
                b2 = (-alpha) * invA0;
                a1 = (-2.0f * cosw0) * invA0;
                a2 = (1.0f - alpha) * invA0;
            }
        };

        void updateFilter(float freq, float q)
        {
            filterL.setBandpass(freq, q, sr);
            filterR.setBandpass(freq, q, sr);
        }

        double sr { 44100.0 };
        bool isEnabled { true };
        bool isListen { false };
        float centerFreqHz { 6500.0f };
        float threshDb { -20.0f };
        float amountLinear { 0.6f };
        float envelope { 0.0f };

        BandpassFilter filterL;
        BandpassFilter filterR;
    };
}
