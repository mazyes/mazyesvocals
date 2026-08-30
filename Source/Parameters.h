#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

namespace MazyesParams
{
    // Global IO & Mix
    inline const juce::ParameterID inGain       { "in_gain", 1 };
    inline const juce::ParameterID outGain      { "out_gain", 1 };
    inline const juce::ParameterID dryWet       { "dry_wet", 1 };

    // Noise Gate
    inline const juce::ParameterID gateEnable   { "gate_enable", 1 };
    inline const juce::ParameterID gateThresh   { "gate_thresh", 1 };
    inline const juce::ParameterID gateAttack   { "gate_attack", 1 };
    inline const juce::ParameterID gateRelease  { "gate_release", 1 };
    inline const juce::ParameterID gateRange    { "gate_range", 1 };

    // Pitch Correction
    inline const juce::ParameterID pitchEnable     { "pitch_enable", 1 };
    inline const juce::ParameterID pitchKey        { "pitch_key", 1 };
    inline const juce::ParameterID pitchScale      { "pitch_scale", 1 };
    inline const juce::ParameterID pitchSpeed      { "pitch_speed", 1 };
    inline const juce::ParameterID pitchAmount     { "pitch_amount", 1 };
    inline const juce::ParameterID pitchTransition { "pitch_transition", 1 };
    inline const juce::ParameterID pitchHumanize   { "pitch_humanize", 1 };
    inline const juce::ParameterID pitchFormant    { "pitch_formant", 1 };

    // EQ
    inline const juce::ParameterID eqEnable    { "eq_enable", 1 };
    inline const juce::ParameterID eqHpfFreq   { "eq_hpf_freq", 1 };
    inline const juce::ParameterID eqLowFreq   { "eq_low_freq", 1 };
    inline const juce::ParameterID eqLowGain   { "eq_low_gain", 1 };
    inline const juce::ParameterID eqMidFreq   { "eq_mid_freq", 1 };
    inline const juce::ParameterID eqMidGain   { "eq_mid_gain", 1 };
    inline const juce::ParameterID eqMidQ      { "eq_mid_q", 1 };
    inline const juce::ParameterID eqHighFreq  { "eq_high_freq", 1 };
    inline const juce::ParameterID eqHighGain  { "eq_high_gain", 1 };
    inline const juce::ParameterID eqAirGain   { "eq_air_gain", 1 };
    inline const juce::ParameterID eqLpfFreq   { "eq_lpf_freq", 1 };

    // Compressor
    inline const juce::ParameterID compEnable   { "comp_enable", 1 };
    inline const juce::ParameterID compThresh   { "comp_thresh", 1 };
    inline const juce::ParameterID compRatio    { "comp_ratio", 1 };
    inline const juce::ParameterID compAttack   { "comp_attack", 1 };
    inline const juce::ParameterID compRelease  { "comp_release", 1 };
    inline const juce::ParameterID compMakeup   { "comp_makeup", 1 };
    inline const juce::ParameterID compKnee     { "comp_knee", 1 };

    // De-Esser
    inline const juce::ParameterID deessEnable  { "deess_enable", 1 };
    inline const juce::ParameterID deessFreq    { "deess_freq", 1 };
    inline const juce::ParameterID deessThresh  { "deess_thresh", 1 };
    inline const juce::ParameterID deessAmount  { "deess_amount", 1 };
    inline const juce::ParameterID deessListen  { "deess_listen", 1 };

    // Saturation
    inline const juce::ParameterID satEnable    { "sat_enable", 1 };
    inline const juce::ParameterID satDrive     { "sat_drive", 1 };
    inline const juce::ParameterID satTone      { "sat_tone", 1 };
    inline const juce::ParameterID satType      { "sat_type", 1 };
    inline const juce::ParameterID satMix       { "sat_mix", 1 };

    // Exciter
    inline const juce::ParameterID exciterEnable    { "exciter_enable", 1 };
    inline const juce::ParameterID exciterFreq      { "exciter_freq", 1 };
    inline const juce::ParameterID exciterDrive     { "exciter_drive", 1 };
    inline const juce::ParameterID exciterHarmonics { "exciter_harmonics", 1 };
    inline const juce::ParameterID exciterMix       { "exciter_mix", 1 };

    // Delay
    inline const juce::ParameterID delayEnable   { "delay_enable", 1 };
    inline const juce::ParameterID delayTime     { "delay_time", 1 };
    inline const juce::ParameterID delaySync     { "delay_sync", 1 };
    inline const juce::ParameterID delayFeedback { "delay_feedback", 1 };
    inline const juce::ParameterID delayPingPong { "delay_pingpong", 1 };
    inline const juce::ParameterID delayDucking  { "delay_ducking", 1 };
    inline const juce::ParameterID delayLowCut   { "delay_lowcut", 1 };
    inline const juce::ParameterID delayHighCut  { "delay_highcut", 1 };
    inline const juce::ParameterID delayMix      { "delay_mix", 1 };

    // Reverb
    inline const juce::ParameterID reverbEnable   { "reverb_enable", 1 };
    inline const juce::ParameterID reverbType     { "reverb_type", 1 };
    inline const juce::ParameterID reverbPredelay { "reverb_predelay", 1 };
    inline const juce::ParameterID reverbDecay    { "reverb_decay", 1 };
    inline const juce::ParameterID reverbDamping  { "reverb_damping", 1 };
    inline const juce::ParameterID reverbWidth    { "reverb_width", 1 };
    inline const juce::ParameterID reverbMix      { "reverb_mix", 1 };

    // Limiter
    inline const juce::ParameterID limiterEnable  { "limiter_enable", 1 };
    inline const juce::ParameterID limiterCeiling { "limiter_ceiling", 1 };
    inline const juce::ParameterID limiterRelease { "limiter_release", 1 };

    inline const juce::StringArray keyChoices { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    inline const juce::StringArray scaleChoices { "Major", "Minor", "Harmonic Minor", "Melodic Minor", "Chromatic" };
    inline const juce::StringArray satTypeChoices { "Tape", "Tube", "Warm", "Foldback", "Clip" };
    inline const juce::StringArray exciterHarmonicChoices { "Even", "Odd", "Mixed" };
    inline const juce::StringArray delaySyncChoices { "Off (ms)", "1/16", "1/8", "1/4", "1/2", "1/8 Dotted", "1/8 Triplet", "1/4 Dotted", "1/4 Triplet" };
    inline const juce::StringArray reverbTypeChoices { "Room", "Plate", "Hall", "Space" };

    inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

        // Global IO
        params.push_back(std::make_unique<juce::AudioParameterFloat>(inGain, "Input Gain", juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f, juce::AudioParameterFloatAttributes().withLabel("dB")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(outGain, "Output Gain", juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f, juce::AudioParameterFloatAttributes().withLabel("dB")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(dryWet, "Global Dry/Wet", juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 100.0f, juce::AudioParameterFloatAttributes().withLabel("%")));

        // Noise Gate
        params.push_back(std::make_unique<juce::AudioParameterBool>(gateEnable, "Gate Enable", false));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(gateThresh, "Gate Thresh", juce::NormalisableRange<float>(-80.0f, 0.0f, 0.5f), -50.0f, juce::AudioParameterFloatAttributes().withLabel("dB")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(gateAttack, "Gate Attack", juce::NormalisableRange<float>(0.1f, 50.0f, 0.1f, 0.4f), 2.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(gateRelease, "Gate Release", juce::NormalisableRange<float>(10.0f, 500.0f, 1.0f, 0.4f), 100.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(gateRange, "Gate Range", juce::NormalisableRange<float>(-80.0f, 0.0f, 1.0f), -60.0f, juce::AudioParameterFloatAttributes().withLabel("dB")));

        // Pitch Correction
        params.push_back(std::make_unique<juce::AudioParameterBool>(pitchEnable, "Pitch Enable", true));
        params.push_back(std::make_unique<juce::AudioParameterChoice>(pitchKey, "Pitch Key", keyChoices, 0));
        params.push_back(std::make_unique<juce::AudioParameterChoice>(pitchScale, "Pitch Scale", scaleChoices, 4)); // Default Chromatic
        params.push_back(std::make_unique<juce::AudioParameterFloat>(pitchSpeed, "Retune Speed", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 0.4f), 0.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(pitchAmount, "Pitch Amount", juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 100.0f, juce::AudioParameterFloatAttributes().withLabel("%")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(pitchTransition, "Note Transition", juce::NormalisableRange<float>(0.0f, 50.0f, 0.5f), 5.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(pitchHumanize, "Humanize", juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 0.0f, juce::AudioParameterFloatAttributes().withLabel("%")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(pitchFormant, "Formant Shift", juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f, juce::AudioParameterFloatAttributes().withLabel("st")));

        // EQ
        params.push_back(std::make_unique<juce::AudioParameterBool>(eqEnable, "EQ Enable", true));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(eqHpfFreq, "HPF Cutoff", juce::NormalisableRange<float>(20.0f, 500.0f, 1.0f, 0.35f), 80.0f, juce::AudioParameterFloatAttributes().withLabel("Hz")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(eqLowFreq, "Low Freq", juce::NormalisableRange<float>(80.0f, 600.0f, 1.0f, 0.4f), 200.0f, juce::AudioParameterFloatAttributes().withLabel("Hz")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(eqLowGain, "Low Gain", juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f), 0.0f, juce::AudioParameterFloatAttributes().withLabel("dB")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(eqMidFreq, "Mid Freq", juce::NormalisableRange<float>(400.0f, 6000.0f, 1.0f, 0.4f), 2500.0f, juce::AudioParameterFloatAttributes().withLabel("Hz")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(eqMidGain, "Mid Gain", juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f), 0.0f, juce::AudioParameterFloatAttributes().withLabel("dB")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(eqMidQ, "Mid Q", juce::NormalisableRange<float>(0.3f, 5.0f, 0.05f), 1.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(eqHighFreq, "High Freq", juce::NormalisableRange<float>(3000.0f, 12000.0f, 1.0f, 0.4f), 5000.0f, juce::AudioParameterFloatAttributes().withLabel("Hz")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(eqHighGain, "High Gain", juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f), 0.0f, juce::AudioParameterFloatAttributes().withLabel("dB")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(eqAirGain, "Air Boost", juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f), 2.5f, juce::AudioParameterFloatAttributes().withLabel("dB")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(eqLpfFreq, "LPF Cutoff", juce::NormalisableRange<float>(10000.0f, 20000.0f, 1.0f, 0.5f), 20000.0f, juce::AudioParameterFloatAttributes().withLabel("Hz")));

        // Compressor
        params.push_back(std::make_unique<juce::AudioParameterBool>(compEnable, "Comp Enable", true));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(compThresh, "Comp Thresh", juce::NormalisableRange<float>(-40.0f, 0.0f, 0.5f), -18.0f, juce::AudioParameterFloatAttributes().withLabel("dB")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(compRatio, "Comp Ratio", juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f, 0.5f), 4.0f, juce::AudioParameterFloatAttributes().withLabel(":1")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(compAttack, "Comp Attack", juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.4f), 10.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(compRelease, "Comp Release", juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.4f), 100.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(compMakeup, "Comp Makeup", juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 4.0f, juce::AudioParameterFloatAttributes().withLabel("dB")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(compKnee, "Comp Knee", juce::NormalisableRange<float>(0.0f, 12.0f, 0.5f), 4.0f, juce::AudioParameterFloatAttributes().withLabel("dB")));

        // De-Esser
        params.push_back(std::make_unique<juce::AudioParameterBool>(deessEnable, "De-Ess Enable", true));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(deessFreq, "De-Ess Freq", juce::NormalisableRange<float>(3000.0f, 10000.0f, 1.0f, 0.4f), 6500.0f, juce::AudioParameterFloatAttributes().withLabel("Hz")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(deessThresh, "De-Ess Thresh", juce::NormalisableRange<float>(-40.0f, 0.0f, 0.5f), -20.0f, juce::AudioParameterFloatAttributes().withLabel("dB")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(deessAmount, "De-Ess Amount", juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 60.0f, juce::AudioParameterFloatAttributes().withLabel("%")));
        params.push_back(std::make_unique<juce::AudioParameterBool>(deessListen, "De-Ess Listen", false));

        // Saturation
        params.push_back(std::make_unique<juce::AudioParameterBool>(satEnable, "Sat Enable", true));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(satDrive, "Sat Drive", juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 6.0f, juce::AudioParameterFloatAttributes().withLabel("dB")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(satTone, "Sat Tone", juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f), 15.0f, juce::AudioParameterFloatAttributes().withLabel("%")));
        params.push_back(std::make_unique<juce::AudioParameterChoice>(satType, "Sat Type", satTypeChoices, 0));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(satMix, "Sat Mix", juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 80.0f, juce::AudioParameterFloatAttributes().withLabel("%")));

        // Exciter
        params.push_back(std::make_unique<juce::AudioParameterBool>(exciterEnable, "Exciter Enable", true));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(exciterFreq, "Exciter Freq", juce::NormalisableRange<float>(3000.0f, 14000.0f, 1.0f, 0.4f), 8000.0f, juce::AudioParameterFloatAttributes().withLabel("Hz")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(exciterDrive, "Exciter Drive", juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 40.0f, juce::AudioParameterFloatAttributes().withLabel("%")));
        params.push_back(std::make_unique<juce::AudioParameterChoice>(exciterHarmonics, "Exciter Harmonics", exciterHarmonicChoices, 2));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(exciterMix, "Exciter Mix", juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 35.0f, juce::AudioParameterFloatAttributes().withLabel("%")));

        // Delay
        params.push_back(std::make_unique<juce::AudioParameterBool>(delayEnable, "Delay Enable", false));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(delayTime, "Delay Time", juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.4f), 250.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
        params.push_back(std::make_unique<juce::AudioParameterChoice>(delaySync, "Delay Sync", delaySyncChoices, 3)); // 1/4
        params.push_back(std::make_unique<juce::AudioParameterFloat>(delayFeedback, "Delay Feedback", juce::NormalisableRange<float>(0.0f, 95.0f, 0.5f), 35.0f, juce::AudioParameterFloatAttributes().withLabel("%")));
        params.push_back(std::make_unique<juce::AudioParameterBool>(delayPingPong, "Delay PingPong", true));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(delayDucking, "Delay Ducking", juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 40.0f, juce::AudioParameterFloatAttributes().withLabel("%")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(delayLowCut, "Delay Low Cut", juce::NormalisableRange<float>(20.0f, 1000.0f, 1.0f, 0.4f), 200.0f, juce::AudioParameterFloatAttributes().withLabel("Hz")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(delayHighCut, "Delay High Cut", juce::NormalisableRange<float>(2000.0f, 20000.0f, 1.0f, 0.4f), 6000.0f, juce::AudioParameterFloatAttributes().withLabel("Hz")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(delayMix, "Delay Mix", juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 25.0f, juce::AudioParameterFloatAttributes().withLabel("%")));

        // Reverb
        params.push_back(std::make_unique<juce::AudioParameterBool>(reverbEnable, "Reverb Enable", true));
        params.push_back(std::make_unique<juce::AudioParameterChoice>(reverbType, "Reverb Type", reverbTypeChoices, 1)); // Plate
        params.push_back(std::make_unique<juce::AudioParameterFloat>(reverbPredelay, "Reverb PreDelay", juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 20.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(reverbDecay, "Reverb Decay", juce::NormalisableRange<float>(0.2f, 10.0f, 0.05f, 0.4f), 2.2f, juce::AudioParameterFloatAttributes().withLabel("s")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(reverbDamping, "Reverb Damping", juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 40.0f, juce::AudioParameterFloatAttributes().withLabel("%")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(reverbWidth, "Reverb Width", juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 100.0f, juce::AudioParameterFloatAttributes().withLabel("%")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(reverbMix, "Reverb Mix", juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 20.0f, juce::AudioParameterFloatAttributes().withLabel("%")));

        // Limiter
        params.push_back(std::make_unique<juce::AudioParameterBool>(limiterEnable, "Limiter Enable", true));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(limiterCeiling, "Limiter Ceiling", juce::NormalisableRange<float>(-12.0f, 0.0f, 0.1f), -0.3f, juce::AudioParameterFloatAttributes().withLabel("dB")));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(limiterRelease, "Limiter Release", juce::NormalisableRange<float>(1.0f, 200.0f, 0.5f, 0.4f), 20.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));

        return { params.begin(), params.end() };
    }
}
