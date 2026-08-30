#include "MazyesLookAndFeel.h"

namespace MazyesUI
{
    const juce::Colour MazyesLookAndFeel::bgDark         { 0xff0d0f12 };
    const juce::Colour MazyesLookAndFeel::bgPanel        { 0xff16191f };
    const juce::Colour MazyesLookAndFeel::bgPanelLight   { 0xff212630 };
    const juce::Colour MazyesLookAndFeel::borderMetallic { 0xff323946 };
    const juce::Colour MazyesLookAndFeel::accentCyan     { 0xff00e5ff };
    const juce::Colour MazyesLookAndFeel::accentCoral    { 0xffff3366 };
    const juce::Colour MazyesLookAndFeel::accentPurple   { 0xffa855f7 };
    const juce::Colour MazyesLookAndFeel::accentAmber    { 0xffffb703 };
    const juce::Colour MazyesLookAndFeel::textPrimary    { 0xfff0f4f8 };
    const juce::Colour MazyesLookAndFeel::textDim        { 0xff7d889b };

    MazyesLookAndFeel::MazyesLookAndFeel()
    {
        setColour(juce::ResizableWindow::backgroundColourId, bgDark);
        setColour(juce::ComboBox::backgroundColourId, bgPanel);
        setColour(juce::ComboBox::textColourId, textPrimary);
        setColour(juce::ComboBox::outlineColourId, borderMetallic);
        setColour(juce::PopupMenu::backgroundColourId, bgPanel);
        setColour(juce::PopupMenu::textColourId, textPrimary);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, bgPanelLight);
        setColour(juce::PopupMenu::highlightedTextColourId, accentCyan);
    }

    void MazyesLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                            float sliderPos, float rotaryStartAngle,
                                            float rotaryEndAngle, juce::Slider& slider)
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        auto center = bounds.getCentre();
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // Outer track background arc
        float arcThickness = 3.5f;
        juce::Path trackBg;
        trackBg.addCentredArc(center.x, center.y, radius - 2.0f, radius - 2.0f, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(borderMetallic.withAlpha(0.6f));
        g.strokePath(trackBg, juce::PathStrokeType(arcThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Active value arc with neon glow
        juce::Colour arcColor = accentCyan;
        if (slider.getName().containsIgnoreCase("drive") || slider.getName().containsIgnoreCase("sat") || slider.getName().containsIgnoreCase("exciter"))
            arcColor = accentAmber;
        else if (slider.getName().containsIgnoreCase("comp") || slider.getName().containsIgnoreCase("gate") || slider.getName().containsIgnoreCase("deess"))
            arcColor = accentCoral;
        else if (slider.getName().containsIgnoreCase("reverb") || slider.getName().containsIgnoreCase("delay"))
            arcColor = accentPurple;

        if (slider.isEnabled() && sliderPos > 0.001f)
        {
            juce::Path activeArc;
            activeArc.addCentredArc(center.x, center.y, radius - 2.0f, radius - 2.0f, 0.0f, rotaryStartAngle, angle, true);

            // Subtle glow
            g.setColour(arcColor.withAlpha(0.25f));
            g.strokePath(activeArc, juce::PathStrokeType(arcThickness + 4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

            // Sharp active arc
            g.setColour(arcColor);
            g.strokePath(activeArc, juce::PathStrokeType(arcThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        // Tactile Metallic Knob Body
        auto knobRadius = radius - 8.0f;
        if (knobRadius < 6.0f) knobRadius = 6.0f;

        // Metallic gradient
        juce::ColourGradient knobGrad(bgPanelLight.brighter(0.15f), center.x - knobRadius, center.y - knobRadius,
                                      bgPanel.darker(0.3f), center.x + knobRadius, center.y + knobRadius, false);
        g.setGradientFill(knobGrad);
        g.fillEllipse(center.x - knobRadius, center.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);

        // Bezel outline
        g.setColour(borderMetallic);
        g.drawEllipse(center.x - knobRadius, center.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f, 1.2f);

        // Inner cap
        float capRadius = knobRadius * 0.72f;
        g.setColour(bgDark.withAlpha(0.9f));
        g.fillEllipse(center.x - capRadius, center.y - capRadius, capRadius * 2.0f, capRadius * 2.0f);

        // Tactile Pointer / Needle
        if (slider.isEnabled())
        {
            juce::Path pointer;
            float pointerLength = knobRadius * 0.85f;
            pointer.startNewSubPath(center.x + (capRadius * 0.3f) * std::sin(angle),
                                   center.y - (capRadius * 0.3f) * std::cos(angle));
            pointer.lineTo(center.x + pointerLength * std::sin(angle),
                           center.y - pointerLength * std::cos(angle));

            g.setColour(arcColor);
            g.strokePath(pointer, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }
    }

    void MazyesLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool,
                                         int, int, int, int, juce::ComboBox& box)
    {
        auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat().reduced(1.0f);

        g.setColour(bgPanel);
        g.fillRoundedRectangle(bounds, 4.0f);

        g.setColour(borderMetallic);
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

        // Dropdown Arrow
        auto arrowZone = bounds.removeFromRight(20.0f);
        juce::Path arrow;
        arrow.startNewSubPath(arrowZone.getCentreX() - 4.0f, arrowZone.getCentreY() - 2.0f);
        arrow.lineTo(arrowZone.getCentreX(), arrowZone.getCentreY() + 3.0f);
        arrow.lineTo(arrowZone.getCentreX() + 4.0f, arrowZone.getCentreY() - 2.0f);

        g.setColour(accentCyan.withAlpha(0.8f));
        g.strokePath(arrow, juce::PathStrokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    void MazyesLookAndFeel::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
    {
        g.setColour(bgPanel.darker(0.2f));
        g.fillRect(0, 0, width, height);

        g.setColour(borderMetallic);
        g.drawRect(0, 0, width, height, 1);
    }

    void MazyesLookAndFeel::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                              bool isHighlighted, bool isHeader, bool isSeparator,
                                              bool isEnabled, const juce::String& text,
                                              const juce::String&, const juce::Drawable*,
                                              const juce::Colour*)
    {
        if (isSeparator)
        {
            g.setColour(borderMetallic);
            g.fillRect(area.reduced(4, 0).withHeight(1));
            return;
        }

        if (isHighlighted && isEnabled)
        {
            g.setColour(bgPanelLight);
            g.fillRect(area);
            g.setColour(accentCyan);
        }
        else
        {
            g.setColour(isEnabled ? textPrimary : textDim);
        }

        g.setFont(juce::Font(13.0f, juce::Font::plain));
        g.drawText(text, area.reduced(8, 0), juce::Justification::centredLeft, true);
    }

    void MazyesLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                             bool, bool)
    {
        auto bounds = button.getLocalBounds().toFloat();
        bool state = button.getToggleState();

        // Mini switch pill design
        float switchW = 32.0f;
        float switchH = 16.0f;
        auto switchRect = juce::Rectangle<float>(bounds.getX() + 2.0f, bounds.getCentreY() - switchH * 0.5f, switchW, switchH);

        g.setColour(state ? accentCyan.withAlpha(0.35f) : bgDark);
        g.fillRoundedRectangle(switchRect, switchH * 0.5f);

        g.setColour(state ? accentCyan : borderMetallic);
        g.drawRoundedRectangle(switchRect, switchH * 0.5f, 1.2f);

        float knobX = state ? (switchRect.getRight() - switchH + 1.0f) : (switchRect.getX() + 1.0f);
        auto knobRect = juce::Rectangle<float>(knobX, switchRect.getY() + 1.0f, switchH - 2.0f, switchH - 2.0f);

        g.setColour(state ? accentCyan : textDim);
        g.fillEllipse(knobRect);

        // Draw Button Text Label if present
        if (button.getButtonText().isNotEmpty())
        {
            g.setColour(state ? textPrimary : textDim);
            g.setFont(juce::Font(11.0f, juce::Font::bold));
            g.drawText(button.getButtonText(), bounds.withTrimmedLeft(switchW + 8.0f).toNearestInt(), juce::Justification::centredLeft, true);
        }
    }
}
