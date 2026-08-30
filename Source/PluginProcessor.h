#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "Parameters.h"
#include "PresetManager.h"
#include "DSP/AudioBufferFIFO.h"
#include "DSP/PitchCorrectionEngine.h"
#include "DSP/NoiseGate.h"
#include "DSP/VocalEQ.h"
#include "DSP/VocalCompressor.h"
#include "DSP/VocalDeEsser.h"
#include "DSP/VocalSaturation.h"
#include "DSP/VocalExciter.h"
#include "DSP/VocalDelay.h"
#include "DSP/VocalReverb.h"
#include "DSP/VocalLimiter.h"

class MazyesVocalsAudioProcessor : public juce::AudioProcessor
{
public:
    MazyesVocalsAudioProcessor();
    ~MazyesVocalsAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    MazyesDSP::LockFreeFIFO<MazyesDSP::PitchTelemetry>& getPitchFIFO() { return pitchFIFO; }
    MazyesDSP::LockFreeFIFO<MazyesDSP::MeterTelemetry>& getMeterFIFO() { return meterFIFO; }

    int currentPresetIndex { 0 };

private:
    juce::AudioProcessorValueTreeState apvts;

    // DSP Instances
    MazyesDSP::NoiseGate noiseGate;
    MazyesDSP::PitchCorrectionEngine pitchEngine;
    MazyesDSP::VocalEQ vocalEQ;
    MazyesDSP::VocalCompressor vocalCompressor;
    MazyesDSP::VocalDeEsser vocalDeEsser;
    MazyesDSP::VocalSaturation vocalSaturation;
    MazyesDSP::VocalExciter vocalExciter;
    MazyesDSP::VocalDelay vocalDelay;
    MazyesDSP::VocalReverb vocalReverb;
    MazyesDSP::VocalLimiter vocalLimiter;

    // Dry buffer for global dry/wet mixing
    juce::AudioBuffer<float> dryBuffer;

    // Telemetry FIFOs for GUI
    MazyesDSP::LockFreeFIFO<MazyesDSP::PitchTelemetry> pitchFIFO;
    MazyesDSP::LockFreeFIFO<MazyesDSP::MeterTelemetry> meterFIFO;

    // Atomic cached parameter pointers for real-time safety
    std::atomic<float>* inGainParam { nullptr };
    std::atomic<float>* outGainParam { nullptr };
    std::atomic<float>* dryWetParam { nullptr };

    std::atomic<float>* gateEnableParam { nullptr };
    std::atomic<float>* gateThreshParam { nullptr };
    std::atomic<float>* gateAttackParam { nullptr };
    std::atomic<float>* gateReleaseParam { nullptr };
    std::atomic<float>* gateRangeParam { nullptr };

    std::atomic<float>* pitchEnableParam { nullptr };
    std::atomic<float>* pitchKeyParam { nullptr };
    std::atomic<float>* pitchScaleParam { nullptr };
    std::atomic<float>* pitchSpeedParam { nullptr };
    std::atomic<float>* pitchAmountParam { nullptr };
    std::atomic<float>* pitchTransitionParam { nullptr };
    std::atomic<float>* pitchHumanizeParam { nullptr };
    std::atomic<float>* pitchFormantParam { nullptr };

    std::atomic<float>* eqEnableParam { nullptr };
    std::atomic<float>* eqHpfFreqParam { nullptr };
    std::atomic<float>* eqLowFreqParam { nullptr };
    std::atomic<float>* eqLowGainParam { nullptr };
    std::atomic<float>* eqMidFreqParam { nullptr };
    std::atomic<float>* eqMidGainParam { nullptr };
    std::atomic<float>* eqMidQParam { nullptr };
    std::atomic<float>* eqHighFreqParam { nullptr };
    std::atomic<float>* eqHighGainParam { nullptr };
    std::atomic<float>* eqAirGainParam { nullptr };
    std::atomic<float>* eqLpfFreqParam { nullptr };

    std::atomic<float>* compEnableParam { nullptr };
    std::atomic<float>* compThreshParam { nullptr };
    std::atomic<float>* compRatioParam { nullptr };
    std::atomic<float>* compAttackParam { nullptr };
    std::atomic<float>* compReleaseParam { nullptr };
    std::atomic<float>* compMakeupParam { nullptr };
    std::atomic<float>* compKneeParam { nullptr };

    std::atomic<float>* deessEnableParam { nullptr };
    std::atomic<float>* deessFreqParam { nullptr };
    std::atomic<float>* deessThreshParam { nullptr };
    std::atomic<float>* deessAmountParam { nullptr };
    std::atomic<float>* deessListenParam { nullptr };

    std::atomic<float>* satEnableParam { nullptr };
    std::atomic<float>* satDriveParam { nullptr };
    std::atomic<float>* satToneParam { nullptr };
    std::atomic<float>* satTypeParam { nullptr };
    std::atomic<float>* satMixParam { nullptr };

    std::atomic<float>* exciterEnableParam { nullptr };
    std::atomic<float>* exciterFreqParam { nullptr };
    std::atomic<float>* exciterDriveParam { nullptr };
    std::atomic<float>* exciterHarmonicsParam { nullptr };
    std::atomic<float>* exciterMixParam { nullptr };

    std::atomic<float>* delayEnableParam { nullptr };
    std::atomic<float>* delayTimeParam { nullptr };
    std::atomic<float>* delaySyncParam { nullptr };
    std::atomic<float>* delayFeedbackParam { nullptr };
    std::atomic<float>* delayPingPongParam { nullptr };
    std::atomic<float>* delayDuckingParam { nullptr };
    std::atomic<float>* delayLowCutParam { nullptr };
    std::atomic<float>* delayHighCutParam { nullptr };
    std::atomic<float>* delayMixParam { nullptr };

    std::atomic<float>* reverbEnableParam { nullptr };
    std::atomic<float>* reverbTypeParam { nullptr };
    std::atomic<float>* reverbPredelayParam { nullptr };
    std::atomic<float>* reverbDecayParam { nullptr };
    std::atomic<float>* reverbDampingParam { nullptr };
    std::atomic<float>* reverbWidthParam { nullptr };
    std::atomic<float>* reverbMixParam { nullptr };

    std::atomic<float>* limiterEnableParam { nullptr };
    std::atomic<float>* limiterCeilingParam { nullptr };
    std::atomic<float>* limiterReleaseParam { nullptr };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MazyesVocalsAudioProcessor)
};
