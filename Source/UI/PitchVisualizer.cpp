#include "PitchVisualizer.h"
#include "../DSP/PitchCorrectionEngine.h"

namespace MazyesUI
{
    PitchVisualizer::PitchVisualizer(MazyesDSP::LockFreeFIFO<MazyesDSP::PitchTelemetry>& fifo)
        : telemetryFIFO(fifo)
    {
        startTimerHz(30); // 30 FPS smooth rendering
    }

    PitchVisualizer::~PitchVisualizer()
    {
        stopTimer();
    }

    void PitchVisualizer::setScaleInfo(int rootKey, int scaleType)
    {
        currentRootKey = rootKey;
        currentScaleType = scaleType;
        repaint();
    }

    void PitchVisualizer::timerCallback()
    {
        MazyesDSP::PitchTelemetry t;
        bool hasUpdate = false;
        while (telemetryFIFO.pop(t))
        {
            currentTelemetry = t;
            hasUpdate = true;
        }

        if (hasUpdate)
        {
            if (currentTelemetry.isVoiced)
            {
                pitchHistory.push_back(currentTelemetry.detectedFreqHz);
                targetHistory.push_back(currentTelemetry.targetFreqHz);
            }
            else
            {
                pitchHistory.push_back(0.0f);
                targetHistory.push_back(0.0f);
            }

            if (pitchHistory.size() > maxHistory) pitchHistory.pop_front();
            if (targetHistory.size() > maxHistory) targetHistory.pop_front();

            repaint();
        }
    }

    void PitchVisualizer::resized() {}

    juce::String PitchVisualizer::getMidiNoteName(int midiNote)
    {
        if (midiNote < 0 || midiNote > 127) return "--";
        const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        int octave = (midiNote / 12) - 1;
        int noteIndex = midiNote % 12;
        return juce::String(noteNames[noteIndex]) + juce::String(octave);
    }

    void PitchVisualizer::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds().toFloat();

        // 1. Background HUD Panel
        g.setColour(MazyesLookAndFeel::bgDark);
        g.fillRoundedRectangle(bounds, 6.0f);

        g.setColour(MazyesLookAndFeel::borderMetallic);
        g.drawRoundedRectangle(bounds, 6.0f, 1.2f);

        // Header strip
        auto headerArea = bounds.removeFromTop(24.0f).reduced(8.0f, 2.0f);
        g.setColour(MazyesLookAndFeel::textDim);
        g.setFont(juce::Font(10.0f, juce::Font::bold));
        g.drawText("REAL-TIME PITCH CORRECTION HUD", headerArea, juce::Justification::centredLeft, true);

        juce::String statusStr = currentTelemetry.isVoiced ? "VOICED TRACKING" : "STANDBY";
        g.setColour(currentTelemetry.isVoiced ? MazyesLookAndFeel::accentCyan : MazyesLookAndFeel::textDim);
        g.drawText(statusStr, headerArea, juce::Justification::centredRight, true);

        // 2. Note Radar Circle (Left area)
        float radarSize = bounds.getHeight() - 16.0f;
        auto radarArea = bounds.removeFromLeft(radarSize + 20.0f).reduced(10.0f);
        auto center = radarArea.getCentre();
        float radius = radarSize * 0.42f;

        // Radar background rings
        g.setColour(MazyesLookAndFeel::bgPanel);
        g.fillEllipse(center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f);
        g.setColour(MazyesLookAndFeel::borderMetallic.withAlpha(0.7f));
        g.drawEllipse(center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f, 1.0f);
        g.drawEllipse(center.x - radius * 0.55f, center.y - radius * 0.55f, radius * 1.1f, radius * 1.1f, 0.8f);

        // Chromatic 12 Note positions
        const char* noteLabels[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        auto scaleIntervals = MazyesDSP::PitchCorrectionEngine::getScaleIntervals(static_cast<MazyesDSP::ScaleType>(currentScaleType));

        int detectedPitchClass = (currentTelemetry.detectedMidiNote >= 0) ? (currentTelemetry.detectedMidiNote % 12) : -1;
        int targetPitchClass = (currentTelemetry.targetMidiNote >= 0) ? (currentTelemetry.targetMidiNote % 12) : -1;

        for (int i = 0; i < 12; ++i)
        {
            float angle = (static_cast<float>(i) / 12.0f) * 2.0f * juce::MathConstants<float>::pi - juce::MathConstants<float>::halfPi;
            float nx = center.x + (radius * 0.78f) * std::cos(angle);
            float ny = center.y + (radius * 0.78f) * std::sin(angle);

            // Check if note is in scale
            int relClass = (i - currentRootKey) % 12;
            if (relClass < 0) relClass += 12;
            bool inScale = false;
            for (int interval : scaleIntervals)
            {
                if (relClass == interval) { inScale = true; break; }
            }

            bool isDetected = (detectedPitchClass == i && currentTelemetry.isVoiced);
            bool isTarget = (targetPitchClass == i && currentTelemetry.isVoiced);

            if (isTarget)
            {
                g.setColour(MazyesLookAndFeel::accentCyan.withAlpha(0.3f));
                g.fillEllipse(nx - 11.0f, ny - 9.0f, 22.0f, 18.0f);
                g.setColour(MazyesLookAndFeel::accentCyan);
                g.drawEllipse(nx - 11.0f, ny - 9.0f, 22.0f, 18.0f, 1.5f);
            }

            g.setFont(juce::Font(isTarget ? 11.0f : 9.5f, (inScale || isTarget) ? juce::Font::bold : juce::Font::plain));
            g.setColour(isTarget ? MazyesLookAndFeel::accentCyan : (inScale ? MazyesLookAndFeel::textPrimary : MazyesLookAndFeel::textDim.withAlpha(0.4f)));
            g.drawText(noteLabels[i], nx - 12.0f, ny - 8.0f, 24.0f, 16.0f, juce::Justification::centred, false);
        }

        // Center readout inside radar
        if (currentTelemetry.isVoiced && currentTelemetry.targetMidiNote >= 0)
        {
            g.setColour(MazyesLookAndFeel::accentCyan);
            g.setFont(juce::Font(18.0f, juce::Font::bold));
            g.drawText(getMidiNoteName(currentTelemetry.targetMidiNote), center.x - 30.0f, center.y - 12.0f, 60.0f, 24.0f, juce::Justification::centred, false);
        }
        else
        {
            g.setColour(MazyesLookAndFeel::textDim);
            g.setFont(juce::Font(12.0f, juce::Font::plain));
            g.drawText("--", center.x - 20.0f, center.y - 10.0f, 40.0f, 20.0f, juce::Justification::centred, false);
        }

        // 3. Right Section: Readouts, Cent Deviation Meter & Scrolling Trajectory
        auto rightArea = bounds.reduced(10.0f, 4.0f);

        // Readout cards (Detected vs Target)
        auto cardArea = rightArea.removeFromTop(48.0f);
        auto detectedCard = cardArea.removeFromLeft(cardArea.getWidth() * 0.48f);
        auto targetCard = cardArea.removeFromRight(detectedCard.getWidth());

        // Detected Card
        g.setColour(MazyesLookAndFeel::bgPanel);
        g.fillRoundedRectangle(detectedCard, 4.0f);
        g.setColour(MazyesLookAndFeel::borderMetallic);
        g.drawRoundedRectangle(detectedCard, 4.0f, 1.0f);

        g.setColour(MazyesLookAndFeel::textDim);
        g.setFont(juce::Font(9.0f, juce::Font::bold));
        g.drawText("DETECTED PITCH", detectedCard.reduced(6.0f, 2.0f).removeFromTop(12.0f), juce::Justification::centredLeft, true);

        juce::String detStr = currentTelemetry.isVoiced ? (getMidiNoteName(currentTelemetry.detectedMidiNote) + " (" + juce::String(currentTelemetry.detectedFreqHz, 1) + " Hz)") : "--";
        g.setColour(MazyesLookAndFeel::textPrimary);
        g.setFont(juce::Font(14.0f, juce::Font::bold));
        g.drawText(detStr, detectedCard.withTrimmedTop(14.0f).reduced(6.0f, 2.0f), juce::Justification::centredLeft, true);

        // Target Card
        g.setColour(MazyesLookAndFeel::bgPanel);
        g.fillRoundedRectangle(targetCard, 4.0f);
        g.setColour(MazyesLookAndFeel::borderMetallic);
        g.drawRoundedRectangle(targetCard, 4.0f, 1.0f);

        g.setColour(MazyesLookAndFeel::textDim);
        g.setFont(juce::Font(9.0f, juce::Font::bold));
        g.drawText("QUANTIZED TARGET", targetCard.reduced(6.0f, 2.0f).removeFromTop(12.0f), juce::Justification::centredLeft, true);

        juce::String tgtStr = currentTelemetry.isVoiced ? (getMidiNoteName(currentTelemetry.targetMidiNote) + " (" + juce::String(currentTelemetry.targetFreqHz, 1) + " Hz)") : "--";
        g.setColour(MazyesLookAndFeel::accentCyan);
        g.setFont(juce::Font(14.0f, juce::Font::bold));
        g.drawText(tgtStr, targetCard.withTrimmedTop(14.0f).reduced(6.0f, 2.0f), juce::Justification::centredLeft, true);

        // Cent Deviation Meter (-50c to +50c)
        rightArea.removeFromTop(6.0f);
        auto centArea = rightArea.removeFromTop(20.0f);

        g.setColour(MazyesLookAndFeel::bgPanel);
        g.fillRoundedRectangle(centArea, 3.0f);
        g.setColour(MazyesLookAndFeel::borderMetallic);
        g.drawRoundedRectangle(centArea, 3.0f, 1.0f);

        // Cent Center line (0 cents)
        float centMidX = centArea.getCentreX();
        g.setColour(MazyesLookAndFeel::accentCyan.withAlpha(0.6f));
        g.fillRect(centMidX - 1.0f, centArea.getY() + 2.0f, 2.0f, centArea.getHeight() - 4.0f);

        // Ticks for -25c, +25c
        g.setColour(MazyesLookAndFeel::borderMetallic);
        g.fillRect(centMidX - centArea.getWidth() * 0.25f, centArea.getY() + 4.0f, 1.0f, centArea.getHeight() - 8.0f);
        g.fillRect(centMidX + centArea.getWidth() * 0.25f, centArea.getY() + 4.0f, 1.0f, centArea.getHeight() - 8.0f);

        if (currentTelemetry.isVoiced)
        {
            float centNorm = std::clamp(currentTelemetry.centDeviation / 50.0f, -1.0f, 1.0f);
            float needleX = centMidX + centNorm * (centArea.getWidth() * 0.45f);

            // Cent bar fill
            auto fillRect = juce::Rectangle<float>(std::min(centMidX, needleX), centArea.getY() + 3.0f,
                                                  std::abs(needleX - centMidX), centArea.getHeight() - 6.0f);
            g.setColour(std::abs(currentTelemetry.centDeviation) < 5.0f ? MazyesLookAndFeel::accentCyan : MazyesLookAndFeel::accentCoral);
            g.fillRoundedRectangle(fillRect, 2.0f);

            // Needle
            g.setColour(juce::Colours::white);
            g.fillRect(needleX - 1.5f, centArea.getY() + 1.0f, 3.0f, centArea.getHeight() - 2.0f);
        }

        // 4. Scrolling Trajectory Graph
        rightArea.removeFromTop(6.0f);
        auto graphArea = rightArea;

        g.setColour(MazyesLookAndFeel::bgPanel.darker(0.3f));
        g.fillRoundedRectangle(graphArea, 4.0f);
        g.setColour(MazyesLookAndFeel::borderMetallic.withAlpha(0.6f));
        g.drawRoundedRectangle(graphArea, 4.0f, 1.0f);

        // Draw pitch trajectory path
        if (pitchHistory.size() > 1)
        {
            juce::Path wavePath;
            float stepX = graphArea.getWidth() / static_cast<float>(maxHistory);

            bool started = false;
            for (size_t i = 0; i < pitchHistory.size(); ++i)
            {
                float freq = pitchHistory[i];
                float x = graphArea.getX() + static_cast<float>(i) * stepX;

                if (freq > 40.0f && freq < 1200.0f)
                {
                    // Map log frequency to graph Y (100 Hz at bottom, 1000 Hz at top)
                    float normY = 1.0f - std::clamp((std::log2(freq) - std::log2(80.0f)) / (std::log2(1000.0f) - std::log2(80.0f)), 0.0f, 1.0f);
                    float y = graphArea.getY() + 4.0f + normY * (graphArea.getHeight() - 8.0f);

                    if (!started) { wavePath.startNewSubPath(x, y); started = true; }
                    else { wavePath.lineTo(x, y); }
                }
                else
                {
                    started = false;
                }
            }

            g.setColour(MazyesLookAndFeel::accentCyan.withAlpha(0.2f));
            g.strokePath(wavePath, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
            g.setColour(MazyesLookAndFeel::accentCyan);
            g.strokePath(wavePath, juce::PathStrokeType(1.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }
    }
}
