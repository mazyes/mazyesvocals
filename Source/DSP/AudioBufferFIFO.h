#pragma once
#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <atomic>
#include <vector>

namespace MazyesDSP
{
    /** Lock-free Single-Producer Single-Consumer FIFO for passing audio/visualizer metrics to UI thread */
    template <typename T, size_t Capacity = 2048>
    class LockFreeFIFO
    {
    public:
        LockFreeFIFO() : writeIndex(0), readIndex(0)
        {
            buffer.resize(Capacity);
        }

        bool push(const T& item)
        {
            const size_t currentWrite = writeIndex.load(std::memory_order_relaxed);
            const size_t nextWrite = (currentWrite + 1) % Capacity;
            if (nextWrite == readIndex.load(std::memory_order_acquire))
                return false; // Full

            buffer[currentWrite] = item;
            writeIndex.store(nextWrite, std::memory_order_release);
            return true;
        }

        bool pop(T& item)
        {
            const size_t currentRead = readIndex.load(std::memory_order_relaxed);
            if (currentRead == writeIndex.load(std::memory_order_acquire))
                return false; // Empty

            item = buffer[currentRead];
            readIndex.store((currentRead + 1) % Capacity, std::memory_order_release);
            return true;
        }

        size_t availableToRead() const
        {
            const size_t w = writeIndex.load(std::memory_order_acquire);
            const size_t r = readIndex.load(std::memory_order_relaxed);
            if (w >= r) return w - r;
            return Capacity - (r - w);
        }

        void clear()
        {
            readIndex.store(writeIndex.load(std::memory_order_relaxed), std::memory_order_release);
        }

    private:
        std::vector<T> buffer;
        std::atomic<size_t> writeIndex;
        std::atomic<size_t> readIndex;
    };

    struct PitchTelemetry
    {
        float detectedFreqHz { 0.0f };
        float targetFreqHz { 0.0f };
        int detectedMidiNote { -1 };
        int targetMidiNote { -1 };
        float centDeviation { 0.0f }; // -50 to +50 cents
        float confidence { 0.0f };    // 0.0 to 1.0
        bool isVoiced { false };
    };

    struct MeterTelemetry
    {
        float inputPeakL { 0.0f };
        float inputPeakR { 0.0f };
        float outputPeakL { 0.0f };
        float outputPeakR { 0.0f };
        float gateGainReduction { 0.0f }; // dB (negative or 0)
        float compGainReduction { 0.0f }; // dB (negative or 0)
        float deessGainReduction { 0.0f }; // dB (negative or 0)
        float limiterGainReduction { 0.0f }; // dB (negative or 0)
    };
}
