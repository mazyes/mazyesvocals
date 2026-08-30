#pragma once
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <cmath>

namespace MazyesDSP
{
    /**
     * 5-Band Studio Vocal Equalizer with High-Pass, Low-Mid Bell, Mid-Presence Bell,
     * High-Shelf / Air Band, and Low-Pass filter.
     * Uses Direct Form II Transposed Biquads for zero noise and smooth parameter automation.
     */
    class VocalEQ
    {
    public:
        VocalEQ() = default;

        void prepare(double sampleRate, int maxBlockSize)
        {
            sr = sampleRate;
            for (auto& band : bandsL) band.reset();
            for (auto& band : bandsR) band.reset();
        }

        void reset()
        {
            for (auto& band : bandsL) band.reset();
            for (auto& band : bandsR) band.reset();
        }

        void setParameters(bool enabled,
                           float hpfFreq,
                           float lowFreq, float lowGainDb,
                           float midFreq, float midGainDb, float midQ,
                           float highFreq, float highGainDb,
                           float airGainDb,
                           float lpfFreq)
        {
            isEnabled = enabled;

            // Band 0: High-Pass Filter (12 dB/oct Butterworth)
            setHighPass(bandsL[0], bandsR[0], hpfFreq, 0.707f);

            // Band 1: Low-Mid Bell
            setPeak(bandsL[1], bandsR[1], lowFreq, 1.0f, lowGainDb);

            // Band 2: Mid-Presence Bell
            setPeak(bandsL[2], bandsR[2], midFreq, midQ, midGainDb);

            // Band 3: High-Presence Bell
            setPeak(bandsL[3], bandsR[3], highFreq, 1.2f, highGainDb);

            // Band 4: Air High Shelf (10 kHz)
            setHighShelf(bandsL[4], bandsR[4], 10000.0f, 0.707f, airGainDb);

            // Band 5: Low-Pass Filter
            setLowPass(bandsL[5], bandsR[5], lpfFreq, 0.707f);
        }

        void process(juce::AudioBuffer<float>& buffer)
        {
            if (!isEnabled) return;

            const int numSamples = buffer.getNumSamples();
            const int numChannels = buffer.getNumChannels();
            float* outL = buffer.getWritePointer(0);
            float* outR = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;

            for (int i = 0; i < numSamples; ++i)
            {
                float sL = outL[i];
                float sR = outR ? outR[i] : sL;

                for (int b = 0; b < 6; ++b)
                {
                    sL = bandsL[b].process(sL);
                    if (outR) sR = bandsR[b].process(sR);
                }

                outL[i] = sL;
                if (outR) outR[i] = sR;
            }
        }

    private:
        struct Biquad
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

            void setCoeffs(float _b0, float _b1, float _b2, float _a0, float _a1, float _a2)
            {
                float invA0 = 1.0f / _a0;
                b0 = _b0 * invA0;
                b1 = _b1 * invA0;
                b2 = _b2 * invA0;
                a1 = _a1 * invA0;
                a2 = _a2 * invA0;
            }
        };

        void setHighPass(Biquad& bL, Biquad& bR, float freq, float q)
        {
            freq = std::clamp(freq, 20.0f, static_cast<float>(sr * 0.45));
            float w0 = 2.0f * juce::MathConstants<float>::pi * freq / static_cast<float>(sr);
            float alpha = std::sin(w0) / (2.0f * q);
            float cosw0 = std::cos(w0);

            float b0 = (1.0f + cosw0) * 0.5f;
            float b1 = -(1.0f + cosw0);
            float b2 = (1.0f + cosw0) * 0.5f;
            float a0 = 1.0f + alpha;
            float a1 = -2.0f * cosw0;
            float a2 = 1.0f - alpha;

            bL.setCoeffs(b0, b1, b2, a0, a1, a2);
            bR.setCoeffs(b0, b1, b2, a0, a1, a2);
        }

        void setLowPass(Biquad& bL, Biquad& bR, float freq, float q)
        {
            freq = std::clamp(freq, 1000.0f, static_cast<float>(sr * 0.49));
            if (freq >= static_cast<float>(sr * 0.45))
            {
                // Flat through
                bL.setCoeffs(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
                bR.setCoeffs(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
                return;
            }

            float w0 = 2.0f * juce::MathConstants<float>::pi * freq / static_cast<float>(sr);
            float alpha = std::sin(w0) / (2.0f * q);
            float cosw0 = std::cos(w0);

            float b0 = (1.0f - cosw0) * 0.5f;
            float b1 = 1.0f - cosw0;
            float b2 = (1.0f - cosw0) * 0.5f;
            float a0 = 1.0f + alpha;
            float a1 = -2.0f * cosw0;
            float a2 = 1.0f - alpha;

            bL.setCoeffs(b0, b1, b2, a0, a1, a2);
            bR.setCoeffs(b0, b1, b2, a0, a1, a2);
        }

        void setPeak(Biquad& bL, Biquad& bR, float freq, float q, float gainDb)
        {
            if (std::abs(gainDb) < 0.05f)
            {
                bL.setCoeffs(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
                bR.setCoeffs(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
                return;
            }

            freq = std::clamp(freq, 20.0f, static_cast<float>(sr * 0.45));
            float A = std::pow(10.0f, gainDb / 40.0f);
            float w0 = 2.0f * juce::MathConstants<float>::pi * freq / static_cast<float>(sr);
            float alpha = std::sin(w0) / (2.0f * q);
            float cosw0 = std::cos(w0);

            float b0 = 1.0f + alpha * A;
            float b1 = -2.0f * cosw0;
            float b2 = 1.0f - alpha * A;
            float a0 = 1.0f + alpha / A;
            float a1 = -2.0f * cosw0;
            float a2 = 1.0f - alpha / A;

            bL.setCoeffs(b0, b1, b2, a0, a1, a2);
            bR.setCoeffs(b0, b1, b2, a0, a1, a2);
        }

        void setHighShelf(Biquad& bL, Biquad& bR, float freq, float q, float gainDb)
        {
            if (std::abs(gainDb) < 0.05f)
            {
                bL.setCoeffs(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
                bR.setCoeffs(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
                return;
            }

            freq = std::clamp(freq, 1000.0f, static_cast<float>(sr * 0.45));
            float A = std::pow(10.0f, gainDb / 40.0f);
            float w0 = 2.0f * juce::MathConstants<float>::pi * freq / static_cast<float>(sr);
            float cosw0 = std::cos(w0);
            float sinw0 = std::sin(w0);
            float alpha = sinw0 / (2.0f * q);
            float twoSqrtAAlpha = 2.0f * std::sqrt(A) * alpha;

            float b0 = A * ((A + 1.0f) + (A - 1.0f) * cosw0 + twoSqrtAAlpha);
            float b1 = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cosw0);
            float b2 = A * ((A + 1.0f) + (A - 1.0f) * cosw0 - twoSqrtAAlpha);
            float a0 = (A + 1.0f) - (A - 1.0f) * cosw0 + twoSqrtAAlpha;
            float a1 = 2.0f * ((A - 1.0f) - (A + 1.0f) * cosw0);
            float a2 = (A + 1.0f) - (A - 1.0f) * cosw0 - twoSqrtAAlpha;

            bL.setCoeffs(b0, b1, b2, a0, a1, a2);
            bR.setCoeffs(b0, b1, b2, a0, a1, a2);
        }

        double sr { 44100.0 };
        bool isEnabled { true };
        std::array<Biquad, 6> bandsL;
        std::array<Biquad, 6> bandsR;
    };
}
