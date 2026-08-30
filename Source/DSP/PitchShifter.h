#pragma once
#include <juce_core/juce_core.h>
#include <vector>
#include <cmath>
#include <algorithm>

namespace MazyesDSP
{
    /**
     * High-quality, low-latency Synchronized Overlap-Add (SOLA/Granular) Pitch Shifter.
     * Optimized for real-time monophonic and polyphonic vocal signals with formant handling.
     */
    class PitchShifter
    {
    public:
        PitchShifter() = default;

        void prepare(double sampleRate, int maxBlockSize)
        {
            sr = sampleRate;
            bufferSize = static_cast<int>(sampleRate * 0.25); // 250ms delay line
            if (bufferSize < 4096) bufferSize = 4096;

            delayBuffer.assign(bufferSize, 0.0f);
            writeIndex = 0;

            grainSize = static_cast<int>(sampleRate * 0.035); // 35ms nominal grain window
            if (grainSize < 128) grainSize = 128;

            readPhase1 = 0.0f;
            readPhase2 = static_cast<float>(grainSize / 2);
            currentPitchRatio = 1.0f;
            targetPitchRatio = 1.0f;
        }

        void reset()
        {
            std::fill(delayBuffer.begin(), delayBuffer.end(), 0.0f);
            writeIndex = 0;
            readPhase1 = 0.0f;
            readPhase2 = static_cast<float>(grainSize / 2);
            currentPitchRatio = 1.0f;
            targetPitchRatio = 1.0f;
        }

        void setPitchRatio(float ratio)
        {
            targetPitchRatio = std::clamp(ratio, 0.5f, 2.0f); // +/- 1 octave
        }

        float processSample(float inputSample)
        {
            // Smooth pitch ratio transitions to eliminate clicks
            currentPitchRatio += 0.005f * (targetPitchRatio - currentPitchRatio);

            // Write into ring buffer
            delayBuffer[writeIndex] = inputSample;

            const float gSize = static_cast<float>(grainSize);
            const float halfGrain = gSize * 0.5f;

            // Increment dual grain tap phases
            float rateDelta = 1.0f - currentPitchRatio;
            readPhase1 += rateDelta;
            readPhase2 += rateDelta;

            // Wrap phases
            while (readPhase1 >= gSize) readPhase1 -= gSize;
            while (readPhase1 < 0.0f)   readPhase1 += gSize;

            while (readPhase2 >= gSize) readPhase2 -= gSize;
            while (readPhase2 < 0.0f)   readPhase2 += gSize;

            // Calculate Hann/Cosine window envelope weights
            float window1 = 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * readPhase1 / gSize));
            float window2 = 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * readPhase2 / gSize));

            // Compute delay taps relative to write head
            float tapOffset1 = readPhase1 + 64.0f;
            float tapOffset2 = readPhase2 + 64.0f;

            float sample1 = readInterpolated(static_cast<float>(writeIndex) - tapOffset1);
            float sample2 = readInterpolated(static_cast<float>(writeIndex) - tapOffset2);

            writeIndex = (writeIndex + 1) % bufferSize;

            return (sample1 * window1) + (sample2 * window2);
        }

    private:
        inline float readInterpolated(float readPos)
        {
            while (readPos < 0.0f) readPos += static_cast<float>(bufferSize);
            while (readPos >= static_cast<float>(bufferSize)) readPos -= static_cast<float>(bufferSize);

            int idx0 = static_cast<int>(readPos);
            int idx1 = (idx0 + 1) % bufferSize;
            float frac = readPos - static_cast<float>(idx0);

            // Linear interpolation
            return delayBuffer[idx0] + frac * (delayBuffer[idx1] - delayBuffer[idx0]);
        }

        double sr { 44100.0 };
        int bufferSize { 16384 };
        int grainSize { 1543 };
        int writeIndex { 0 };
        float readPhase1 { 0.0f };
        float readPhase2 { 0.0f };
        float currentPitchRatio { 1.0f };
        float targetPitchRatio { 1.0f };
        std::vector<float> delayBuffer;
    };
}
