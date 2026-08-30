#pragma once
#include <juce_core/juce_core.h>
#include <vector>
#include <cmath>
#include <algorithm>

namespace MazyesDSP
{
    /**
     * Real-time YIN-based fundamental pitch estimator.
     * Robust to vocal harmonics and ambient noise, with parabolic sub-sample interpolation.
     */
    class PitchDetector
    {
    public:
        PitchDetector() = default;

        void prepare(double sampleRate, int maxBlockSize)
        {
            sr = sampleRate;
            bufferSize = static_cast<int>(sampleRate * 0.04); // 40ms analysis window (~25Hz lowest detectable)
            if (bufferSize < 512) bufferSize = 512;
            if (bufferSize > 4096) bufferSize = 4096;

            audioBuffer.assign(bufferSize, 0.0f);
            diffBuffer.assign(bufferSize / 2, 0.0f);
            cmndBuffer.assign(bufferSize / 2, 0.0f);
            writePos = 0;
            lastDetectedFreq = 0.0f;
            lastConfidence = 0.0f;

            minPeriod = static_cast<int>(sampleRate / 1100.0); // ~1100 Hz (C6) max pitch
            maxPeriod = static_cast<int>(sampleRate / 60.0);   // ~60 Hz (B1) min pitch
            if (maxPeriod >= bufferSize / 2) maxPeriod = (bufferSize / 2) - 1;
            if (minPeriod < 2) minPeriod = 2;
        }

        void reset()
        {
            std::fill(audioBuffer.begin(), audioBuffer.end(), 0.0f);
            writePos = 0;
            lastDetectedFreq = 0.0f;
            lastConfidence = 0.0f;
        }

        /** Feeds audio samples and periodically updates detected pitch */
        void processSample(float inputSample, float& detectedFreq, float& confidence, bool& isVoiced)
        {
            audioBuffer[writePos] = inputSample;
            writePos = (writePos + 1) % bufferSize;

            // Run estimation every hop size (approx every 5ms)
            hopCounter++;
            const int hopSize = static_cast<int>(sr * 0.005);
            if (hopCounter >= hopSize)
            {
                hopCounter = 0;
                estimatePitch();
            }

            detectedFreq = lastDetectedFreq;
            confidence = lastConfidence;
            isVoiced = (lastConfidence >= voicingThreshold && lastDetectedFreq > 0.0f);
        }

    private:
        void estimatePitch()
        {
            // 1. Check RMS Energy
            float sumSquares = 0.0f;
            for (float s : audioBuffer)
                sumSquares += s * s;
            float rms = std::sqrt(sumSquares / static_cast<float>(bufferSize));
            if (rms < 0.005f) // Silence threshold
            {
                lastDetectedFreq = 0.0f;
                lastConfidence = 0.0f;
                return;
            }

            const int halfSize = bufferSize / 2;

            // 2. Difference Function d_t(tau)
            diffBuffer[0] = 0.0f;
            for (int tau = 1; tau < halfSize; ++tau)
            {
                float diffSum = 0.0f;
                for (int i = 0; i < halfSize; ++i)
                {
                    int idx1 = (writePos + i) % bufferSize;
                    int idx2 = (writePos + i + tau) % bufferSize;
                    float delta = audioBuffer[idx1] - audioBuffer[idx2];
                    diffSum += delta * delta;
                }
                diffBuffer[tau] = diffSum;
            }

            // 3. Cumulative Mean Normalized Difference Function (CMNDF)
            cmndBuffer[0] = 1.0f;
            float runningSum = 0.0f;
            for (int tau = 1; tau < halfSize; ++tau)
            {
                runningSum += diffBuffer[tau];
                if (runningSum > 1e-6f)
                    cmndBuffer[tau] = diffBuffer[tau] * static_cast<float>(tau) / runningSum;
                else
                    cmndBuffer[tau] = 1.0f;
            }

            // 4. Absolute thresholding & minimum peak finding
            int bestTau = -1;
            const float yinThreshold = 0.15f; // Standard YIN dip threshold

            for (int tau = minPeriod; tau <= maxPeriod; ++tau)
            {
                if (cmndBuffer[tau] < yinThreshold)
                {
                    while (tau + 1 <= maxPeriod && cmndBuffer[tau + 1] < cmndBuffer[tau])
                        tau++;
                    bestTau = tau;
                    break;
                }
            }

            // If no dip below threshold found, pick global minimum within bounds
            if (bestTau == -1)
            {
                float minVal = 9999.0f;
                for (int tau = minPeriod; tau <= maxPeriod; ++tau)
                {
                    if (cmndBuffer[tau] < minVal)
                    {
                        minVal = cmndBuffer[tau];
                        bestTau = tau;
                    }
                }
                if (minVal > 0.45f) // Not voiced enough
                {
                    lastDetectedFreq = 0.0f;
                    lastConfidence = 0.0f;
                    return;
                }
            }

            // 5. Parabolic Interpolation for Sub-sample Accuracy
            float interpolatedTau = static_cast<float>(bestTau);
            if (bestTau > 1 && bestTau < halfSize - 1)
            {
                float s0 = cmndBuffer[bestTau - 1];
                float s1 = cmndBuffer[bestTau];
                float s2 = cmndBuffer[bestTau + 1];
                float denom = 2.0f * (2.0f * s1 - s0 - s2);
                if (std::abs(denom) > 1e-6f)
                {
                    float delta = (s0 - s2) / denom;
                    interpolatedTau += delta;
                }
            }

            if (interpolatedTau > 0.0f)
            {
                float freq = static_cast<float>(sr / interpolatedTau);
                if (freq >= 50.0f && freq <= 1200.0f)
                {
                    lastDetectedFreq = freq;
                    lastConfidence = std::clamp(1.0f - cmndBuffer[bestTau], 0.0f, 1.0f);
                    return;
                }
            }

            lastDetectedFreq = 0.0f;
            lastConfidence = 0.0f;
        }

        double sr { 44100.0 };
        int bufferSize { 2048 };
        int minPeriod { 40 };
        int maxPeriod { 735 };
        int writePos { 0 };
        int hopCounter { 0 };
        float lastDetectedFreq { 0.0f };
        float lastConfidence { 0.0f };
        float voicingThreshold { 0.55f };

        std::vector<float> audioBuffer;
        std::vector<float> diffBuffer;
        std::vector<float> cmndBuffer;
    };
}
