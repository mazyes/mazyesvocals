#pragma once
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>
#include <algorithm>

namespace MazyesDSP
{
    enum class ExciterHarmonics
    {
        Even = 0,
        Odd,
        Mixed
    };

    /**
     * Modern Vocal Exciter creating high-frequency shimmer and presence.
     * High-passes vocal above cutoff, generates 2nd/3rd harmonics, and adds sheen.
     */
    class VocalExciter
    {
    public:
        VocalExciter() = default;

        void prepare(double sampleRate, int maxBlockSize)
        {
            sr = sampleRate;
            hpfL.reset();
            hpfR.reset();
            postHpfL.reset();
            postHpfR.reset();
            updateFilters();
        }

        void reset()
        {
            hpfL.reset();
            hpfR.reset();
            postHpfL.reset();
            postHpfR.reset();
        }

        void setParameters(bool enabled, float freqHz, float drivePct, ExciterHarmonics harmMode, float mixPct)
        {
            isEnabled = enabled;
            cutoffFreqHz = std::clamp(freqHz, 3000.0f, 14000.0f);
            drive = 1.0f + (drivePct * 0.01f) * 4.0f; // 1.0 to 5.0
            harmonics = harmMode;
            mix = mixPct * 0.01f;
            updateFilters();
        }

        void process(juce::AudioBuffer<float>& buffer)
        {
            if (!isEnabled || mix <= 0.001f) return;

            const int numSamples = buffer.getNumSamples();
            const int numChannels = buffer.getNumChannels();
            float* outL = buffer.getWritePointer(0);
            float* outR = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;

            for (int i = 0; i < numSamples; ++i)
            {
                float inL = outL[i];
                float inR = outR ? outR[i] : inL;

                // Highpass sidechain
                float highL = hpfL.process(inL) * drive;
                float highR = outR ? (hpfR.process(inR) * drive) : highL;

                // Generate harmonics
                float harmL = generateHarmonics(highL);
                float harmR = outR ? generateHarmonics(highR) : harmL;

                // Clean post-harmonic DC & low frequency artifacts
                harmL = postHpfL.process(harmL);
                if (outR) harmR = postHpfR.process(harmR);

                // Blend with dry
                outL[i] = inL + harmL * mix * 0.5f;
                if (outR) outR[i] = inR + harmR * mix * 0.5f;
            }
        }

    private:
        inline float generateHarmonics(float x)
        {
            switch (harmonics)
            {
                case ExciterHarmonics::Even:
                {
                    // 2nd harmonic (warmth & presence)
                    return (x * x) * (x >= 0.0f ? 1.0f : -1.0f);
                }
                case ExciterHarmonics::Odd:
                {
                    // 3rd harmonic (edge & cut)
                    return (x * x * x) * 0.5f;
                }
                case ExciterHarmonics::Mixed:
                default:
                {
                    // Combination of 2nd and 3rd harmonics
                    float x2 = (x * x) * (x >= 0.0f ? 0.7f : -0.7f);
                    float x3 = (x * x * x) * 0.3f;
                    return std::tanh(x2 + x3);
                }
            }
        }

        struct SimpleHPF
        {
            float s1 { 0.0f };
            float alpha { 0.5f };
            void reset() { s1 = 0.0f; }

            void setCutoff(float freq, double sampleRate)
            {
                float dt = 1.0f / static_cast<float>(sampleRate);
                float rc = 1.0f / (2.0f * juce::MathConstants<float>::pi * freq);
                alpha = rc / (rc + dt);
            }

            inline float process(float in)
            {
                float out = alpha * (s1 + in - s1);
                s1 = in;
                return in - (s1 * (1.0f - alpha));
            }
        };

        void updateFilters()
        {
            hpfL.setCutoff(cutoffFreqHz, sr);
            hpfR.setCutoff(cutoffFreqHz, sr);
            postHpfL.setCutoff(std::min(cutoffFreqHz, 4000.0f), sr);
            postHpfR.setCutoff(std::min(cutoffFreqHz, 4000.0f), sr);
        }

        double sr { 44100.0 };
        bool isEnabled { true };
        float cutoffFreqHz { 8000.0f };
        float drive { 2.0f };
        ExciterHarmonics harmonics { ExciterHarmonics::Mixed };
        float mix { 0.35f };

        SimpleHPF hpfL;
        SimpleHPF hpfR;
        SimpleHPF postHpfL;
        SimpleHPF postHpfR;
    };
}
