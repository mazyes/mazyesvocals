#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UI/MazyesLookAndFeel.h"
#include "UI/RotaryKnob.h"
#include "UI/MeterComponent.h"
#include "UI/ModuleHeader.h"
#include "UI/PitchVisualizer.h"

class MazyesVocalsAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    MazyesVocalsAudioProcessorEditor (MazyesVocalsAudioProcessor&);
    ~MazyesVocalsAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    MazyesVocalsAudioProcessor& audioProcessor;
    MazyesUI::MazyesLookAndFeel customLookAndFeel;

    // Header Controls
    juce::ComboBox presetBox;
    juce::TextButton prevPresetButton { "<" };
    juce::TextButton nextPresetButton { ">" };
    std::unique_ptr<MazyesUI::RotaryKnob> inGainKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> outGainKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> dryWetKnob;
    MazyesUI::LevelMeter inMeter { "IN" };
    MazyesUI::LevelMeter outMeter { "OUT" };

    // Pitch Correction Section
    std::unique_ptr<MazyesUI::ModuleHeader> pitchHeader;
    juce::ComboBox pitchKeyBox;
    juce::ComboBox pitchScaleBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> pitchKeyAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> pitchScaleAttachment;

    MazyesUI::PitchVisualizer pitchVisualizer;

    std::unique_ptr<MazyesUI::RotaryKnob> pitchSpeedKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> pitchAmountKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> pitchTransitionKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> pitchHumanizeKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> pitchFormantKnob;

    // Gate Module
    std::unique_ptr<MazyesUI::ModuleHeader> gateHeader;
    std::unique_ptr<MazyesUI::RotaryKnob> gateThreshKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> gateAttackKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> gateReleaseKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> gateRangeKnob;

    // EQ Module
    std::unique_ptr<MazyesUI::ModuleHeader> eqHeader;
    std::unique_ptr<MazyesUI::RotaryKnob> eqHpfKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> eqLowGainKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> eqMidFreqKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> eqMidGainKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> eqHighGainKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> eqAirGainKnob;

    // Compressor Module
    std::unique_ptr<MazyesUI::ModuleHeader> compHeader;
    std::unique_ptr<MazyesUI::RotaryKnob> compThreshKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> compRatioKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> compAttackKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> compReleaseKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> compMakeupKnob;
    MazyesUI::GainReductionMeter compGrMeter { "GR" };

    // De-Esser Module
    std::unique_ptr<MazyesUI::ModuleHeader> deessHeader;
    std::unique_ptr<MazyesUI::RotaryKnob> deessFreqKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> deessThreshKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> deessAmountKnob;
    juce::ToggleButton deessListenButton { "LISTEN" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> deessListenAttachment;
    MazyesUI::GainReductionMeter deessGrMeter { "DS" };

    // Saturation Module
    std::unique_ptr<MazyesUI::ModuleHeader> satHeader;
    std::unique_ptr<MazyesUI::RotaryKnob> satDriveKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> satToneKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> satMixKnob;
    juce::ComboBox satTypeBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> satTypeAttachment;

    // Exciter Module
    std::unique_ptr<MazyesUI::ModuleHeader> exciterHeader;
    std::unique_ptr<MazyesUI::RotaryKnob> exciterFreqKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> exciterDriveKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> exciterMixKnob;
    juce::ComboBox exciterHarmonicsBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> exciterHarmonicsAttachment;

    // Delay Module
    std::unique_ptr<MazyesUI::ModuleHeader> delayHeader;
    std::unique_ptr<MazyesUI::RotaryKnob> delayTimeKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> delayFeedbackKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> delayDuckingKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> delayMixKnob;
    juce::ComboBox delaySyncBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> delaySyncAttachment;
    juce::ToggleButton delayPingPongButton { "PING PONG" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> delayPingPongAttachment;

    // Reverb Module
    std::unique_ptr<MazyesUI::ModuleHeader> reverbHeader;
    std::unique_ptr<MazyesUI::RotaryKnob> reverbPredelayKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> reverbDecayKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> reverbDampingKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> reverbMixKnob;
    juce::ComboBox reverbTypeBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> reverbTypeAttachment;

    // Limiter Module
    std::unique_ptr<MazyesUI::ModuleHeader> limiterHeader;
    std::unique_ptr<MazyesUI::RotaryKnob> limiterCeilingKnob;
    std::unique_ptr<MazyesUI::RotaryKnob> limiterReleaseKnob;
    MazyesUI::GainReductionMeter limiterGrMeter { "LIMIT" };

    void setupPresets();
    void changePreset(int delta);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MazyesVocalsAudioProcessorEditor)
};
