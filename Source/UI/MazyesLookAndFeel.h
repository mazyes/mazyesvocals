#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace MazyesUI
{
    class MazyesLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        MazyesLookAndFeel();
        ~MazyesLookAndFeel() override = default;

        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                              float sliderPosProportional, float rotaryStartAngle,
                              float rotaryEndAngle, juce::Slider& slider) override;

        void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                          int buttonX, int buttonY, int buttonW, int buttonH,
                          juce::ComboBox& box) override;

        void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override;
        void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                               bool isSeparator, bool isActive, bool isHighlighted,
                               bool hasSubMenu, const juce::String& text,
                               const juce::String& shortcutKeyText,
                               const juce::Drawable* icon, const juce::Colour* textColour) override;

        void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

        // Custom Palette
        static const juce::Colour bgDark;
        static const juce::Colour bgPanel;
        static const juce::Colour bgPanelLight;
        static const juce::Colour borderMetallic;
        static const juce::Colour accentCyan;
        static const juce::Colour accentCoral;
        static const juce::Colour accentPurple;
        static const juce::Colour accentAmber;
        static const juce::Colour textPrimary;
        static const juce::Colour textDim;
    };
}
