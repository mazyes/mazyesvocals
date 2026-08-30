#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../DSP/AudioBufferFIFO.h"
#include "MazyesLookAndFeel.h"
#include <array>
#include <deque>

namespace MazyesUI
{
    class PitchVisualizer : public juce::Component, public juce::Timer
    {
    public:
        PitchVisualizer(MazyesDSP::LockFreeFIFO<MazyesDSP::PitchTelemetry>& fifo);
        ~PitchVisualizer() override;

        void timerCallback() override;
        void paint(juce::Graphics& g) override;
        void resized() override;

        void setScaleInfo(int rootKey, int scaleType);

    private:
        MazyesDSP::LockFreeFIFO<MazyesDSP::PitchTelemetry>& telemetryFIFO;

        int currentRootKey { 0 };
        int currentScaleType { 4 }; // Chromatic
        MazyesDSP::PitchTelemetry currentTelemetry;

        std::deque<float> pitchHistory;
        std::deque<float> targetHistory;
        static constexpr size_t maxHistory = 100;

        juce::String getMidiNoteName(int midiNote);

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchVisualizer)
    };
}
