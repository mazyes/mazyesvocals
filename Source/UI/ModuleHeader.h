#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "MazyesLookAndFeel.h"

namespace MazyesUI
{
    class ModuleHeader : public juce::Component
    {
    public:
        ModuleHeader(juce::AudioProcessorValueTreeState& apvts,
                     const juce::String& enableParamId,
                     const juce::String& titleText,
                     juce::Colour accentColour = MazyesLookAndFeel::accentCyan)
            : title(titleText), accent(accentColour)
        {
            toggle.setName(enableParamId);
            toggle.setButtonText("");
            addAndMakeVisible(toggle);
            attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, enableParamId, toggle);
        }

        void resized() override
        {
            auto bounds = getLocalBounds();
            toggle.setBounds(bounds.removeFromRight(36).reduced(2, 4));
        }

        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds().toFloat();

            // Background header strip
            g.setColour(MazyesLookAndFeel::bgPanelLight);
            g.fillRoundedRectangle(bounds, 3.0f);

            // Accent status indicator LED
            bool enabled = toggle.getToggleState();
            float ledY = bounds.getCentreY() - 3.5f;
            g.setColour(enabled ? accent : MazyesLookAndFeel::textDim.withAlpha(0.3f));
            g.fillEllipse(6.0f, ledY, 7.0f, 7.0f);

            if (enabled)
            {
                g.setColour(accent.withAlpha(0.3f));
                g.fillEllipse(4.0f, ledY - 2.0f, 11.0f, 11.0f);
            }

            // Title Text
            g.setColour(enabled ? MazyesLookAndFeel::textPrimary : MazyesLookAndFeel::textDim);
            g.setFont(juce::Font(11.0f, juce::Font::bold));
            g.drawText(title.toUpperCase(), bounds.withTrimmedLeft(18.0f).withTrimmedRight(38.0f).toNearestInt(),
                       juce::Justification::centredLeft, true);
        }

    private:
        juce::String title;
        juce::Colour accent;
        juce::ToggleButton toggle;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;
    };
}
