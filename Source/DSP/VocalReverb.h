#pragma once
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <vector>
#include <array>
#include <cmath>

namespace MazyesDSP
{
    enum class ReverbType
    {
        Room = 0,
        Plate,
        Hall,
        Space
    };

    /**
     * High-density Schroeder-Moorer / FDN algorithmic stereo reverb tailored for vocals.
     * Features pre-delay, 8 tuned low-pass feedback comb filters, 4 all-pass diffusers,
     * and stereo cross-coupling.
     */
    class VocalReverb
    {
    public:
        VocalReverb() = default;

        void prepare(double sampleRate, int maxBlockSize)
        {
            sr = sampleRate;

            // Pre-delay buffer (up to 150ms)
            preDelayBuffer.assign(static_cast<int>(sampleRate * 0.15), 0.0f);
            preDelayWrite = 0;

            initCombAndAllpass();
        }

        void reset()
        {
            std::fill(preDelayBuffer.begin(), preDelayBuffer.end(), 0.0f);
            preDelayWrite = 0;
            for (auto& c : combsL) c.reset();
            for (auto& c : combsR) c.reset();
            for (auto& a : allpassesL) a.reset();
            for (auto& a : allpassesR) a.reset();
        }

        void setParameters(bool enabled, ReverbType type, float predelayMs,
                           float decayTimeSec, float dampingPct, float widthPct, float mixPct)
        {
            isEnabled = enabled;
            reverbType = type;
            preDelaySamples = static_cast<int>(predelayMs * 0.001f * sr);
            if (preDelaySamples >= static_cast<int>(preDelayBuffer.size()))
                preDelaySamples = static_cast<int>(preDelayBuffer.size()) - 1;

            decaySec = std::clamp(decayTimeSec, 0.2f, 10.0f);
            damping = std::clamp(dampingPct * 0.01f, 0.05f, 0.9f);
            stereoWidth = widthPct * 0.01f;
            mix = mixPct * 0.01f;

            updateCombFeedback();
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

            for (int i = 0; i < numSamples; ++i)
            {
                float dryL = inL[i];
                float dryR = inR ? inR[i] : dryL;
                float monoIn = 0.5f * (dryL + dryR);

                // Pre-delay
                preDelayBuffer[preDelayWrite] = monoIn;
                int preDelayRead = preDelayWrite - preDelaySamples;
                if (preDelayRead < 0) preDelayRead += static_cast<int>(preDelayBuffer.size());
                float delayedIn = preDelayBuffer[preDelayRead];
                preDelayWrite = (preDelayWrite + 1) % preDelayBuffer.size();

                // Parallel Comb Filters
                float wetL = 0.0f;
                float wetR = 0.0f;

                for (size_t c = 0; c < 8; ++c)
                {
                    wetL += combsL[c].process(delayedIn);
                    wetR += combsR[c].process(delayedIn);
                }

                // Series Allpass Diffusers
                for (size_t a = 0; a < 4; ++a)
                {
                    wetL = allpassesL[a].process(wetL);
                    wetR = allpassesR[a].process(wetR);
                }

                // Stereo Width Cross-Mixing
                float mid = 0.5f * (wetL + wetR);
                float side = 0.5f * (wetL - wetR) * stereoWidth;
                float finalWetL = (mid + side) * 0.35f;
                float finalWetR = (mid - side) * 0.35f;

                outL[i] = dryL * (1.0f - mix * 0.5f) + finalWetL * mix;
                if (outR)
                    outR[i] = dryR * (1.0f - mix * 0.5f) + finalWetR * mix;
            }
        }

    private:
        struct CombFilter
        {
            std::vector<float> buffer;
            int bufferSize { 1000 };
            int bufferIndex { 0 };
            float feedback { 0.8f };
            float filterStore { 0.0f };
            float damp { 0.2f };

            void init(int size)
            {
                bufferSize = size;
                buffer.assign(bufferSize, 0.0f);
                bufferIndex = 0;
                filterStore = 0.0f;
            }

            void reset()
            {
                std::fill(buffer.begin(), buffer.end(), 0.0f);
                bufferIndex = 0;
                filterStore = 0.0f;
            }

            inline float process(float in)
            {
                float output = buffer[bufferIndex];
                filterStore = (output * (1.0f - damp)) + (filterStore * damp);
                buffer[bufferIndex] = in + (filterStore * feedback);
                bufferIndex = (bufferIndex + 1) % bufferSize;
                return output;
            }
        };

        struct AllpassFilter
        {
            std::vector<float> buffer;
            int bufferSize { 500 };
            int bufferIndex { 0 };
            float feedback { 0.5f };

            void init(int size)
            {
                bufferSize = size;
                buffer.assign(bufferSize, 0.0f);
                bufferIndex = 0;
            }

            void reset()
            {
                std::fill(buffer.begin(), buffer.end(), 0.0f);
                bufferIndex = 0;
            }

            inline float process(float in)
            {
                float bufOut = buffer[bufferIndex];
                float out = -in + bufOut;
                buffer[bufferIndex] = in + (bufOut * feedback);
                bufferIndex = (bufferIndex + 1) % bufferSize;
                return out;
            }
        };

        void initCombAndAllpass()
        {
            // Nominal delay lengths at 44.1kHz
            const int combTuningsL[8] = { 1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617 };
            const int combTuningsR[8] = { 1139, 1211, 1300, 1379, 1445, 1514, 1580, 1640 };
            const int allpassTuningsL[4] = { 556, 441, 341, 225 };
            const int allpassTuningsR[4] = { 579, 464, 364, 248 };

            float srScale = static_cast<float>(sr / 44100.0);

            for (int i = 0; i < 8; ++i)
            {
                combsL[i].init(static_cast<int>(combTuningsL[i] * srScale));
                combsR[i].init(static_cast<int>(combTuningsR[i] * srScale));
            }

            for (int i = 0; i < 4; ++i)
            {
                allpassesL[i].init(static_cast<int>(allpassTuningsL[i] * srScale));
                allpassesR[i].init(static_cast<int>(allpassTuningsR[i] * srScale));
            }
        }

        void updateCombFeedback()
        {
            // Scale feedback factor according to RT60 decay time
            float fbBase = std::clamp(std::pow(0.001f, 1.0f / (decaySec * static_cast<float>(sr / 1500.0))), 0.4f, 0.96f);

            for (auto& c : combsL)
            {
                c.feedback = fbBase;
                c.damp = damping;
            }
            for (auto& c : combsR)
            {
                c.feedback = fbBase;
                c.damp = damping;
            }
        }

        double sr { 44100.0 };
        bool isEnabled { true };
        ReverbType reverbType { ReverbType::Plate };
        int preDelaySamples { 0 };
        float decaySec { 2.2f };
        float damping { 0.4f };
        float stereoWidth { 1.0f };
        float mix { 0.2f };

        std::vector<float> preDelayBuffer;
        int preDelayWrite { 0 };

        std::array<CombFilter, 8> combsL;
        std::array<CombFilter, 8> combsR;
        std::array<AllpassFilter, 4> allpassesL;
        std::array<AllpassFilter, 4> allpassesR;
    };
}
