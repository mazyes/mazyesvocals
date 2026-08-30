#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "MazyesLookAndFeel.h"
#include <cmath>

namespace MazyesUI
{
    class LevelMeter : public juce::Component
    {
    public:
        LevelMeter(const juce::String& meterName = "") : name(meterName) {}

        void setLevels(float peakL, float peakR)
        {
            currentPeakL += 0.4f * (peakL - currentPeakL);
            currentPeakR += 0.4f * (peakR - currentPeakR);
            if (peakL > peakHoldL) peakHoldL = peakL; else peakHoldL *= 0.96f;
            if (peakR > peakHoldR) peakHoldR = peakR; else peakHoldR *= 0.96f;
            repaint();
        }

        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds().toFloat();

            if (name.isNotEmpty())
            {
                auto labelArea = bounds.removeFromTop(12.0f);
                g.setColour(MazyesLookAndFeel::textDim);
                g.setFont(juce::Font(9.0f, juce::Font::bold));
                g.drawText(name, labelArea.toNearestInt(), juce::Justification::centred, true);
            }

            bounds.reduce(2.0f, 2.0f);
            float barW = (bounds.getWidth() - 2.0f) * 0.5f;

            drawBar(g, bounds.removeFromLeft(barW), currentPeakL, peakHoldL);
            bounds.removeFromLeft(2.0f);
            drawBar(g, bounds, currentPeakR, peakHoldR);
        }

    private:
        void drawBar(juce::Graphics& g, juce::Rectangle<float> area, float levelLinear, float holdLinear)
        {
            g.setColour(MazyesLookAndFeel::bgDark);
            g.fillRoundedRectangle(area, 2.0f);

            float levelDb = 20.0f * std::log10(std::max(levelLinear, 1e-4f)); // -80 to 0 dB
            float norm = std::clamp((levelDb + 60.0f) / 60.0f, 0.0f, 1.0f);

            float h = area.getHeight() * norm;
            auto fillRect = area.removeFromBottom(h);

            juce::Colour meterColor = (levelDb > -0.5f) ? MazyesLookAndFeel::accentCoral
                                    : (levelDb > -6.0f) ? MazyesLookAndFeel::accentAmber
                                    : MazyesLookAndFeel::accentCyan;

            g.setColour(meterColor);
            g.fillRoundedRectangle(fillRect, 1.5f);

            // Peak hold tick
            float holdDb = 20.0f * std::log10(std::max(holdLinear, 1e-4f));
            float holdNorm = std::clamp((holdDb + 60.0f) / 60.0f, 0.0f, 1.0f);
            float holdY = area.getBottom() - (area.getHeight() * holdNorm);

            g.setColour(juce::Colours::white.withAlpha(0.85f));
            g.fillRect(area.getX(), holdY - 1.0f, area.getWidth(), 2.0f);
        }

        juce::String name;
        float currentPeakL { 0.0f };
        float currentPeakR { 0.0f };
        float peakHoldL { 0.0f };
        float peakHoldR { 0.0f };
    };

    class GainReductionMeter : public juce::Component
    {
    public:
        GainReductionMeter(const juce::String& meterName = "GR") : name(meterName) {}

        void setReduction(float grDb)
        {
            float targetGr = std::abs(grDb);
            currentGr += 0.35f * (targetGr - currentGr);
            repaint();
        }

        void paint(juce::Graphics& g) override
        {
            auto bounds = getLocalBounds().toFloat();
            auto labelArea = bounds.removeFromTop(10.0f);
            g.setColour(MazyesLookAndFeel::textDim);
            g.setFont(juce::Font(8.0f, juce::Font::bold));
            g.drawText(name, labelArea.toNearestInt(), juce::Justification::centred, true);

            bounds.reduce(2.0f, 1.0f);
            g.setColour(MazyesLookAndFeel::bgDark);
            g.fillRoundedRectangle(bounds, 2.0f);

            // GR fills downwards from top (0 dB at top, 20 dB at bottom)
            float norm = std::clamp(currentGr / 20.0f, 0.0f, 1.0f);
            float h = bounds.getHeight() * norm;
            auto grRect = bounds.removeFromTop(h);

            g.setColour(MazyesLookAndFeel::accentCoral);
            g.fillRoundedRectangle(grRect, 1.5f);
        }

    private:
        juce::String name;
        float currentGr { 0.0f };
    };
}
