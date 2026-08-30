#include "PluginProcessor.h"
#include "PluginEditor.h"

MazyesVocalsAudioProcessorEditor::MazyesVocalsAudioProcessorEditor (MazyesVocalsAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), pitchVisualizer(p.getPitchFIFO())
{
    setLookAndFeel(&customLookAndFeel);

    auto& apvts = audioProcessor.getAPVTS();

    // 1. Header Components
    setupPresets();
    addAndMakeVisible(presetBox);
    addAndMakeVisible(prevPresetButton);
    addAndMakeVisible(nextPresetButton);

    prevPresetButton.onClick = [this] { changePreset(-1); };
    nextPresetButton.onClick = [this] { changePreset(1); };

    inGainKnob  = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::inGain.getParamID(), "IN GAIN", "dB");
    outGainKnob = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::outGain.getParamID(), "OUT GAIN", "dB");
    dryWetKnob  = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::dryWet.getParamID(), "DRY / WET", "%");

    addAndMakeVisible(*inGainKnob);
    addAndMakeVisible(*outGainKnob);
    addAndMakeVisible(*dryWetKnob);
    addAndMakeVisible(inMeter);
    addAndMakeVisible(outMeter);

    // 2. Pitch Correction Hero Section
    pitchHeader = std::make_unique<MazyesUI::ModuleHeader>(apvts, MazyesParams::pitchEnable.getParamID(), "PITCH CORRECTION", MazyesUI::MazyesLookAndFeel::accentCyan);
    addAndMakeVisible(*pitchHeader);

    pitchKeyBox.addItemList(MazyesParams::keyChoices, 1);
    pitchKeyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, MazyesParams::pitchKey.getParamID(), pitchKeyBox);
    addAndMakeVisible(pitchKeyBox);

    pitchScaleBox.addItemList(MazyesParams::scaleChoices, 1);
    pitchScaleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, MazyesParams::pitchScale.getParamID(), pitchScaleBox);
    addAndMakeVisible(pitchScaleBox);

    pitchKeyBox.onChange = [this] {
        pitchVisualizer.setScaleInfo(pitchKeyBox.getSelectedItemIndex(), pitchScaleBox.getSelectedItemIndex());
    };
    pitchScaleBox.onChange = [this] {
        pitchVisualizer.setScaleInfo(pitchKeyBox.getSelectedItemIndex(), pitchScaleBox.getSelectedItemIndex());
    };

    addAndMakeVisible(pitchVisualizer);

    pitchSpeedKnob      = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::pitchSpeed.getParamID(), "SPEED", "ms");
    pitchAmountKnob     = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::pitchAmount.getParamID(), "AMOUNT", "%");
    pitchTransitionKnob = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::pitchTransition.getParamID(), "TRANSITION", "ms");
    pitchHumanizeKnob   = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::pitchHumanize.getParamID(), "HUMANIZE", "%");
    pitchFormantKnob    = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::pitchFormant.getParamID(), "FORMANT", "st");

    addAndMakeVisible(*pitchSpeedKnob);
    addAndMakeVisible(*pitchAmountKnob);
    addAndMakeVisible(*pitchTransitionKnob);
    addAndMakeVisible(*pitchHumanizeKnob);
    addAndMakeVisible(*pitchFormantKnob);

    // 3. Gate Module
    gateHeader = std::make_unique<MazyesUI::ModuleHeader>(apvts, MazyesParams::gateEnable.getParamID(), "GATE", MazyesUI::MazyesLookAndFeel::accentCoral);
    gateThreshKnob  = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::gateThresh.getParamID(), "THRESH", "dB");
    gateAttackKnob  = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::gateAttack.getParamID(), "ATTACK", "ms");
    gateReleaseKnob = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::gateRelease.getParamID(), "RELEASE", "ms");
    gateRangeKnob   = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::gateRange.getParamID(), "RANGE", "dB");

    addAndMakeVisible(*gateHeader);
    addAndMakeVisible(*gateThreshKnob);
    addAndMakeVisible(*gateAttackKnob);
    addAndMakeVisible(*gateReleaseKnob);
    addAndMakeVisible(*gateRangeKnob);

    // 4. EQ Module
    eqHeader = std::make_unique<MazyesUI::ModuleHeader>(apvts, MazyesParams::eqEnable.getParamID(), "VOCAL EQ", MazyesUI::MazyesLookAndFeel::accentCyan);
    eqHpfKnob      = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::eqHpfFreq.getParamID(), "HPF", "Hz");
    eqLowGainKnob  = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::eqLowGain.getParamID(), "BODY", "dB");
    eqMidFreqKnob  = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::eqMidFreq.getParamID(), "MID FREQ", "Hz");
    eqMidGainKnob  = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::eqMidGain.getParamID(), "MID GAIN", "dB");
    eqHighGainKnob = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::eqHighGain.getParamID(), "PRESENCE", "dB");
    eqAirGainKnob  = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::eqAirGain.getParamID(), "AIR", "dB");

    addAndMakeVisible(*eqHeader);
    addAndMakeVisible(*eqHpfKnob);
    addAndMakeVisible(*eqLowGainKnob);
    addAndMakeVisible(*eqMidFreqKnob);
    addAndMakeVisible(*eqMidGainKnob);
    addAndMakeVisible(*eqHighGainKnob);
    addAndMakeVisible(*eqAirGainKnob);

    // 5. Compressor Module
    compHeader = std::make_unique<MazyesUI::ModuleHeader>(apvts, MazyesParams::compEnable.getParamID(), "COMPRESSOR", MazyesUI::MazyesLookAndFeel::accentCoral);
    compThreshKnob  = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::compThresh.getParamID(), "THRESH", "dB");
    compRatioKnob   = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::compRatio.getParamID(), "RATIO", ":1");
    compAttackKnob  = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::compAttack.getParamID(), "ATTACK", "ms");
    compReleaseKnob = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::compRelease.getParamID(), "RELEASE", "ms");
    compMakeupKnob  = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::compMakeup.getParamID(), "MAKEUP", "dB");

    addAndMakeVisible(*compHeader);
    addAndMakeVisible(*compThreshKnob);
    addAndMakeVisible(*compRatioKnob);
    addAndMakeVisible(*compAttackKnob);
    addAndMakeVisible(*compReleaseKnob);
    addAndMakeVisible(*compMakeupKnob);
    addAndMakeVisible(compGrMeter);

    // 6. De-Esser Module
    deessHeader = std::make_unique<MazyesUI::ModuleHeader>(apvts, MazyesParams::deessEnable.getParamID(), "DE-ESSER", MazyesUI::MazyesLookAndFeel::accentCoral);
    deessFreqKnob   = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::deessFreq.getParamID(), "FREQ", "Hz");
    deessThreshKnob = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::deessThresh.getParamID(), "THRESH", "dB");
    deessAmountKnob = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::deessAmount.getParamID(), "AMOUNT", "%");

    deessListenAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, MazyesParams::deessListen.getParamID(), deessListenButton);

    addAndMakeVisible(*deessHeader);
    addAndMakeVisible(*deessFreqKnob);
    addAndMakeVisible(*deessThreshKnob);
    addAndMakeVisible(*deessAmountKnob);
    addAndMakeVisible(deessListenButton);
    addAndMakeVisible(deessGrMeter);

    // 7. Saturation Module
    satHeader = std::make_unique<MazyesUI::ModuleHeader>(apvts, MazyesParams::satEnable.getParamID(), "SATURATION", MazyesUI::MazyesLookAndFeel::accentAmber);
    satDriveKnob = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::satDrive.getParamID(), "DRIVE", "dB");
    satToneKnob  = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::satTone.getParamID(), "TONE", "%");
    satMixKnob   = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::satMix.getParamID(), "MIX", "%");

    satTypeBox.addItemList(MazyesParams::satTypeChoices, 1);
    satTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, MazyesParams::satType.getParamID(), satTypeBox);

    addAndMakeVisible(*satHeader);
    addAndMakeVisible(*satDriveKnob);
    addAndMakeVisible(*satToneKnob);
    addAndMakeVisible(*satMixKnob);
    addAndMakeVisible(satTypeBox);

    // 8. Exciter Module
    exciterHeader = std::make_unique<MazyesUI::ModuleHeader>(apvts, MazyesParams::exciterEnable.getParamID(), "EXCITER", MazyesUI::MazyesLookAndFeel::accentAmber);
    exciterFreqKnob  = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::exciterFreq.getParamID(), "FREQ", "Hz");
    exciterDriveKnob = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::exciterDrive.getParamID(), "DRIVE", "%");
    exciterMixKnob   = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::exciterMix.getParamID(), "MIX", "%");

    exciterHarmonicsBox.addItemList(MazyesParams::exciterHarmonicChoices, 1);
    exciterHarmonicsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, MazyesParams::exciterHarmonics.getParamID(), exciterHarmonicsBox);

    addAndMakeVisible(*exciterHeader);
    addAndMakeVisible(*exciterFreqKnob);
    addAndMakeVisible(*exciterDriveKnob);
    addAndMakeVisible(*exciterMixKnob);
    addAndMakeVisible(exciterHarmonicsBox);

    // 9. Delay Module
    delayHeader = std::make_unique<MazyesUI::ModuleHeader>(apvts, MazyesParams::delayEnable.getParamID(), "DELAY", MazyesUI::MazyesLookAndFeel::accentPurple);
    delayTimeKnob     = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::delayTime.getParamID(), "TIME", "ms");
    delayFeedbackKnob = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::delayFeedback.getParamID(), "FEEDBACK", "%");
    delayDuckingKnob  = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::delayDucking.getParamID(), "DUCKING", "%");
    delayMixKnob      = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::delayMix.getParamID(), "MIX", "%");

    delaySyncBox.addItemList(MazyesParams::delaySyncChoices, 1);
    delaySyncAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, MazyesParams::delaySync.getParamID(), delaySyncBox);

    delayPingPongAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, MazyesParams::delayPingPong.getParamID(), delayPingPongButton);

    addAndMakeVisible(*delayHeader);
    addAndMakeVisible(*delayTimeKnob);
    addAndMakeVisible(*delayFeedbackKnob);
    addAndMakeVisible(*delayDuckingKnob);
    addAndMakeVisible(*delayMixKnob);
    addAndMakeVisible(delaySyncBox);
    addAndMakeVisible(delayPingPongButton);

    // 10. Reverb Module
    reverbHeader = std::make_unique<MazyesUI::ModuleHeader>(apvts, MazyesParams::reverbEnable.getParamID(), "REVERB", MazyesUI::MazyesLookAndFeel::accentPurple);
    reverbPredelayKnob = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::reverbPredelay.getParamID(), "PRE-DLY", "ms");
    reverbDecayKnob    = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::reverbDecay.getParamID(), "DECAY", "s");
    reverbDampingKnob  = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::reverbDamping.getParamID(), "DAMPING", "%");
    reverbMixKnob      = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::reverbMix.getParamID(), "MIX", "%");

    reverbTypeBox.addItemList(MazyesParams::reverbTypeChoices, 1);
    reverbTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, MazyesParams::reverbType.getParamID(), reverbTypeBox);

    addAndMakeVisible(*reverbHeader);
    addAndMakeVisible(*reverbPredelayKnob);
    addAndMakeVisible(*reverbDecayKnob);
    addAndMakeVisible(*reverbDampingKnob);
    addAndMakeVisible(*reverbMixKnob);
    addAndMakeVisible(reverbTypeBox);

    // 11. Limiter Module
    limiterHeader = std::make_unique<MazyesUI::ModuleHeader>(apvts, MazyesParams::limiterEnable.getParamID(), "LIMITER", MazyesUI::MazyesLookAndFeel::accentCyan);
    limiterCeilingKnob = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::limiterCeiling.getParamID(), "CEILING", "dB");
    limiterReleaseKnob = std::make_unique<MazyesUI::RotaryKnob>(apvts, MazyesParams::limiterRelease.getParamID(), "RELEASE", "ms");

    addAndMakeVisible(*limiterHeader);
    addAndMakeVisible(*limiterCeilingKnob);
    addAndMakeVisible(*limiterReleaseKnob);
    addAndMakeVisible(limiterGrMeter);

    // Update initial scale in HUD
    pitchVisualizer.setScaleInfo(pitchKeyBox.getSelectedItemIndex(), pitchScaleBox.getSelectedItemIndex());

    setSize (1060, 720);
    startTimerHz(30);
}

MazyesVocalsAudioProcessorEditor::~MazyesVocalsAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void MazyesVocalsAudioProcessorEditor::setupPresets()
{
    const auto& presets = MazyesPresets::getFactoryPresets();
    presetBox.clear();
    for (size_t i = 0; i < presets.size(); ++i)
    {
        presetBox.addItem(presets[i].name, static_cast<int>(i + 1));
    }
    presetBox.setSelectedId(1, juce::dontSendNotification);

    presetBox.onChange = [this] {
        int selectedIndex = presetBox.getSelectedItemIndex();
        if (selectedIndex >= 0)
        {
            audioProcessor.setCurrentProgram(selectedIndex);
        }
    };
}

void MazyesVocalsAudioProcessorEditor::changePreset(int delta)
{
    int current = presetBox.getSelectedItemIndex();
    int total = presetBox.getNumItems();
    int next = (current + delta + total) % total;
    presetBox.setSelectedItemIndex(next);
}

void MazyesVocalsAudioProcessorEditor::timerCallback()
{
    MazyesDSP::MeterTelemetry m;
    while (audioProcessor.getMeterFIFO().pop(m))
    {
        inMeter.setLevels(m.inputPeakL, m.inputPeakR);
        outMeter.setLevels(m.outputPeakL, m.outputPeakR);
        compGrMeter.setReduction(m.compGainReduction);
        deessGrMeter.setReduction(m.deessGainReduction);
        limiterGrMeter.setReduction(m.limiterGainReduction);
    }
}

void MazyesVocalsAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Metallic brushed dark chassis background
    g.setColour(MazyesUI::MazyesLookAndFeel::bgDark);
    g.fillRect(bounds);

    // Subtle metallic noise/texture gradient
    juce::ColourGradient grad(MazyesUI::MazyesLookAndFeel::bgPanel.darker(0.2f), 0.0f, 0.0f,
                              MazyesUI::MazyesLookAndFeel::bgDark, 0.0f, bounds.getHeight(), false);
    g.setGradientFill(grad);
    g.fillRect(bounds);

    // Outer Chassis Bezel
    g.setColour(MazyesUI::MazyesLookAndFeel::borderMetallic);
    g.drawRect(bounds, 1.5f);

    // Brand Logo & Subtitle in Header
    auto headerArea = bounds.removeFromTop(60.0f).reduced(12.0f, 6.0f);

    g.setColour(MazyesUI::MazyesLookAndFeel::accentCyan);
    g.setFont(juce::Font(22.0f, juce::Font::bold));
    g.drawText("MAZYES", 16, 12, 110, 24, juce::Justification::centredLeft, false);

    g.setColour(MazyesUI::MazyesLookAndFeel::textPrimary);
    g.setFont(juce::Font(22.0f, juce::Font::plain));
    g.drawText("VOCALS", 126, 12, 110, 24, juce::Justification::centredLeft, false);

    g.setColour(MazyesUI::MazyesLookAndFeel::textDim);
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.drawText("PROFESSIONAL REAL-TIME VOCAL SUITE", 16, 36, 220, 14, juce::Justification::centredLeft, false);

    // Key / Scale Labels
    g.setColour(MazyesUI::MazyesLookAndFeel::textDim);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.drawText("KEY", 260, 72, 50, 14, juce::Justification::centredLeft, false);
    g.drawText("SCALE", 350, 72, 80, 14, juce::Justification::centredLeft, false);
}

void MazyesVocalsAudioProcessorEditor::resized()
{
    auto totalArea = getLocalBounds().reduced(10);

    // 1. Top Header Bar (Height: 56)
    auto headerArea = totalArea.removeFromTop(52);

    // Logo area takes 230 px
    headerArea.removeFromLeft(230);

    // Preset selector in center
    auto presetArea = headerArea.removeFromLeft(280).reduced(0, 10);
    prevPresetButton.setBounds(presetArea.removeFromLeft(24));
    nextPresetButton.setBounds(presetArea.removeFromRight(24));
    presetArea.removeFromLeft(4);
    presetArea.removeFromRight(4);
    presetBox.setBounds(presetArea);

    // Global Mix & Master IO on right
    auto ioArea = headerArea.removeFromRight(320);
    inMeter.setBounds(ioArea.removeFromLeft(24).reduced(2, 2));
    inGainKnob->setBounds(ioArea.removeFromLeft(60));
    ioArea.removeFromLeft(10);
    dryWetKnob->setBounds(ioArea.removeFromLeft(64));
    ioArea.removeFromLeft(10);
    outGainKnob->setBounds(ioArea.removeFromLeft(60));
    outMeter.setBounds(ioArea.removeFromLeft(24).reduced(2, 2));

    totalArea.removeFromTop(8);

    // 2. Central Pitch Correction Hero Panel (Height: 210)
    auto pitchPanelArea = totalArea.removeFromTop(210);

    // Pitch Header & Key/Scale Selectors
    auto pitchTopBar = pitchPanelArea.removeFromTop(28);
    pitchHeader->setBounds(pitchTopBar.removeFromLeft(240));
    pitchTopBar.removeFromLeft(10);
    pitchKeyBox.setBounds(pitchTopBar.removeFromLeft(75).reduced(0, 2));
    pitchTopBar.removeFromLeft(8);
    pitchScaleBox.setBounds(pitchTopBar.removeFromLeft(140).reduced(0, 2));

    pitchPanelArea.removeFromTop(6);

    // Visualizer on Left (Width: 620), Pitch Knobs on Right (Width: 410)
    auto hudArea = pitchPanelArea.removeFromLeft(620);
    pitchVisualizer.setBounds(hudArea);

    pitchPanelArea.removeFromLeft(12);
    auto pitchKnobGrid = pitchPanelArea;
    int knobW = 76;
    pitchSpeedKnob->setBounds(pitchKnobGrid.removeFromLeft(knobW));
    pitchAmountKnob->setBounds(pitchKnobGrid.removeFromLeft(knobW));
    pitchTransitionKnob->setBounds(pitchKnobGrid.removeFromLeft(knobW));
    pitchHumanizeKnob->setBounds(pitchKnobGrid.removeFromLeft(knobW));
    pitchFormantKnob->setBounds(pitchKnobGrid.removeFromLeft(knobW));

    totalArea.removeFromTop(10);

    // 3. Bottom Modular Racks - Two Rows of Studio Strips
    auto bottomRow1 = totalArea.removeFromTop(205);
    totalArea.removeFromTop(8);
    auto bottomRow2 = totalArea;

    // Row 1: Gate (200px) | EQ (340px) | Compressor (270px) | De-Esser (210px)
    // GATE
    auto gateArea = bottomRow1.removeFromLeft(215);
    gateHeader->setBounds(gateArea.removeFromTop(24));
    gateArea.removeFromTop(4);
    int gkW = gateArea.getWidth() / 4;
    gateThreshKnob->setBounds(gateArea.removeFromLeft(gkW));
    gateAttackKnob->setBounds(gateArea.removeFromLeft(gkW));
    gateReleaseKnob->setBounds(gateArea.removeFromLeft(gkW));
    gateRangeKnob->setBounds(gateArea);

    bottomRow1.removeFromLeft(8);

    // VOCAL EQ
    auto eqArea = bottomRow1.removeFromLeft(330);
    eqHeader->setBounds(eqArea.removeFromTop(24));
    eqArea.removeFromTop(4);
    int eqkW = eqArea.getWidth() / 6;
    eqHpfKnob->setBounds(eqArea.removeFromLeft(eqkW));
    eqLowGainKnob->setBounds(eqArea.removeFromLeft(eqkW));
    eqMidFreqKnob->setBounds(eqArea.removeFromLeft(eqkW));
    eqMidGainKnob->setBounds(eqArea.removeFromLeft(eqkW));
    eqHighGainKnob->setBounds(eqArea.removeFromLeft(eqkW));
    eqAirGainKnob->setBounds(eqArea);

    bottomRow1.removeFromLeft(8);

    // COMPRESSOR
    auto compArea = bottomRow1.removeFromLeft(265);
    compHeader->setBounds(compArea.removeFromTop(24));
    compArea.removeFromTop(4);
    compGrMeter.setBounds(compArea.removeFromRight(20).reduced(2, 2));
    int ckW = compArea.getWidth() / 5;
    compThreshKnob->setBounds(compArea.removeFromLeft(ckW));
    compRatioKnob->setBounds(compArea.removeFromLeft(ckW));
    compAttackKnob->setBounds(compArea.removeFromLeft(ckW));
    compReleaseKnob->setBounds(compArea.removeFromLeft(ckW));
    compMakeupKnob->setBounds(compArea);

    bottomRow1.removeFromLeft(8);

    // DE-ESSER
    auto deessArea = bottomRow1;
    deessHeader->setBounds(deessArea.removeFromTop(24));
    deessArea.removeFromTop(4);
    deessGrMeter.setBounds(deessArea.removeFromRight(20).reduced(2, 2));
    int dkW = (deessArea.getWidth() - 40) / 3;
    deessFreqKnob->setBounds(deessArea.removeFromLeft(dkW));
    deessThreshKnob->setBounds(deessArea.removeFromLeft(dkW));
    deessAmountKnob->setBounds(deessArea.removeFromLeft(dkW));
    deessListenButton.setBounds(deessArea.reduced(2, 35));

    // Row 2: Saturation (230px) | Exciter (230px) | Delay (280px) | Reverb (210px) | Limiter (70px)
    // SATURATION
    auto satArea = bottomRow2.removeFromLeft(230);
    satHeader->setBounds(satArea.removeFromTop(24));
    satArea.removeFromTop(4);
    satTypeBox.setBounds(satArea.removeFromBottom(22).reduced(4, 1));
    int skW = satArea.getWidth() / 3;
    satDriveKnob->setBounds(satArea.removeFromLeft(skW));
    satToneKnob->setBounds(satArea.removeFromLeft(skW));
    satMixKnob->setBounds(satArea);

    bottomRow2.removeFromLeft(8);

    // EXCITER
    auto exciterArea = bottomRow2.removeFromLeft(230);
    exciterHeader->setBounds(exciterArea.removeFromTop(24));
    exciterArea.removeFromTop(4);
    exciterHarmonicsBox.setBounds(exciterArea.removeFromBottom(22).reduced(4, 1));
    int exkW = exciterArea.getWidth() / 3;
    exciterFreqKnob->setBounds(exciterArea.removeFromLeft(exkW));
    exciterDriveKnob->setBounds(exciterArea.removeFromLeft(exkW));
    exciterMixKnob->setBounds(exciterArea);

    bottomRow2.removeFromLeft(8);

    // DELAY
    auto delayArea = bottomRow2.removeFromLeft(280);
    delayHeader->setBounds(delayArea.removeFromTop(24));
    delayArea.removeFromTop(4);
    auto delayBottomControls = delayArea.removeFromBottom(22);
    delaySyncBox.setBounds(delayBottomControls.removeFromLeft(140).reduced(4, 1));
    delayPingPongButton.setBounds(delayBottomControls.reduced(4, 1));

    int dlykW = delayArea.getWidth() / 4;
    delayTimeKnob->setBounds(delayArea.removeFromLeft(dlykW));
    delayFeedbackKnob->setBounds(delayArea.removeFromLeft(dlykW));
    delayDuckingKnob->setBounds(delayArea.removeFromLeft(dlykW));
    delayMixKnob->setBounds(delayArea);

    bottomRow2.removeFromLeft(8);

    // REVERB
    auto reverbArea = bottomRow2.removeFromLeft(200);
    reverbHeader->setBounds(reverbArea.removeFromTop(24));
    reverbArea.removeFromTop(4);
    reverbTypeBox.setBounds(reverbArea.removeFromBottom(22).reduced(4, 1));
    int rkW = reverbArea.getWidth() / 4;
    reverbPredelayKnob->setBounds(reverbArea.removeFromLeft(rkW));
    reverbDecayKnob->setBounds(reverbArea.removeFromLeft(rkW));
    reverbDampingKnob->setBounds(reverbArea.removeFromLeft(rkW));
    reverbMixKnob->setBounds(reverbArea);

    bottomRow2.removeFromLeft(8);

    // LIMITER
    auto limiterArea = bottomRow2;
    limiterHeader->setBounds(limiterArea.removeFromTop(24));
    limiterArea.removeFromTop(4);
    limiterGrMeter.setBounds(limiterArea.removeFromRight(18).reduced(1, 2));
    int limkW = limiterArea.getWidth() / 2;
    limiterCeilingKnob->setBounds(limiterArea.removeFromLeft(limkW));
    limiterReleaseKnob->setBounds(limiterArea);
}
