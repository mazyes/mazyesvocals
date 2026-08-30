#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "MazyesLookAndFeel.h"

namespace MazyesUI
{
    class RotaryKnob : public juce::Component
    {
    public:
        RotaryKnob(juce::AudioProcessorValueTreeState& apvts,
                   const juce::String& paramId,
                   const juce::String& labelText,
                   const juce::String& unitSuffix = "")
            : title(labelText), suffix(unitSuffix)
        {
            slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
            slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 55, 14);
            slider.setName(paramId);
            addAndMakeVisible(slider);

            attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, paramId, slider);
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            labelBounds = bounds.removeFromTop(14);
            slider.setBounds(bounds);
        }

        void paint(juce::Graphics& g) override
        {
            g.setColour(MazyesLookAndFeel::textDim);
            g.setFont(juce::Font(10.0f, juce::Font::bold));
            g.drawText(title.toUpperCase(), labelBounds, juce::Justification::centred, true);
        }

        juce::Slider& getSlider() { return slider; }

    private:
        juce::String title;
        juce::String suffix;
        juce::Rectangle<int> labelBounds;
        juce::Slider slider;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };
}
