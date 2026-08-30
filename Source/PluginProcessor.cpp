#include "PluginProcessor.h"
#include "PluginEditor.h"

MazyesVocalsAudioProcessor::MazyesVocalsAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
       apvts (*this, nullptr, "Parameters", MazyesParams::createParameterLayout())
{
    inGainParam       = apvts.getRawParameterValue(MazyesParams::inGain.getParamID());
    outGainParam      = apvts.getRawParameterValue(MazyesParams::outGain.getParamID());
    dryWetParam       = apvts.getRawParameterValue(MazyesParams::dryWet.getParamID());

    gateEnableParam   = apvts.getRawParameterValue(MazyesParams::gateEnable.getParamID());
    gateThreshParam   = apvts.getRawParameterValue(MazyesParams::gateThresh.getParamID());
    gateAttackParam   = apvts.getRawParameterValue(MazyesParams::gateAttack.getParamID());
    gateReleaseParam  = apvts.getRawParameterValue(MazyesParams::gateRelease.getParamID());
    gateRangeParam    = apvts.getRawParameterValue(MazyesParams::gateRange.getParamID());

    pitchEnableParam     = apvts.getRawParameterValue(MazyesParams::pitchEnable.getParamID());
    pitchKeyParam        = apvts.getRawParameterValue(MazyesParams::pitchKey.getParamID());
    pitchScaleParam      = apvts.getRawParameterValue(MazyesParams::pitchScale.getParamID());
    pitchSpeedParam      = apvts.getRawParameterValue(MazyesParams::pitchSpeed.getParamID());
    pitchAmountParam     = apvts.getRawParameterValue(MazyesParams::pitchAmount.getParamID());
    pitchTransitionParam = apvts.getRawParameterValue(MazyesParams::pitchTransition.getParamID());
    pitchHumanizeParam   = apvts.getRawParameterValue(MazyesParams::pitchHumanize.getParamID());
    pitchFormantParam    = apvts.getRawParameterValue(MazyesParams::pitchFormant.getParamID());

    eqEnableParam    = apvts.getRawParameterValue(MazyesParams::eqEnable.getParamID());
    eqHpfFreqParam   = apvts.getRawParameterValue(MazyesParams::eqHpfFreq.getParamID());
    eqLowFreqParam   = apvts.getRawParameterValue(MazyesParams::eqLowFreq.getParamID());
    eqLowGainParam   = apvts.getRawParameterValue(MazyesParams::eqLowGain.getParamID());
    eqMidFreqParam   = apvts.getRawParameterValue(MazyesParams::eqMidFreq.getParamID());
    eqMidGainParam   = apvts.getRawParameterValue(MazyesParams::eqMidGain.getParamID());
    eqMidQParam      = apvts.getRawParameterValue(MazyesParams::eqMidQ.getParamID());
    eqHighFreqParam  = apvts.getRawParameterValue(MazyesParams::eqHighFreq.getParamID());
    eqHighGainParam  = apvts.getRawParameterValue(MazyesParams::eqHighGain.getParamID());
    eqAirGainParam   = apvts.getRawParameterValue(MazyesParams::eqAirGain.getParamID());
    eqLpfFreqParam   = apvts.getRawParameterValue(MazyesParams::eqLpfFreq.getParamID());

    compEnableParam  = apvts.getRawParameterValue(MazyesParams::compEnable.getParamID());
    compThreshParam  = apvts.getRawParameterValue(MazyesParams::compThresh.getParamID());
    compRatioParam   = apvts.getRawParameterValue(MazyesParams::compRatio.getParamID());
    compAttackParam  = apvts.getRawParameterValue(MazyesParams::compAttack.getParamID());
    compReleaseParam = apvts.getRawParameterValue(MazyesParams::compRelease.getParamID());
    compMakeupParam  = apvts.getRawParameterValue(MazyesParams::compMakeup.getParamID());
    compKneeParam    = apvts.getRawParameterValue(MazyesParams::compKnee.getParamID());

    deessEnableParam = apvts.getRawParameterValue(MazyesParams::deessEnable.getParamID());
    deessFreqParam   = apvts.getRawParameterValue(MazyesParams::deessFreq.getParamID());
    deessThreshParam = apvts.getRawParameterValue(MazyesParams::deessThresh.getParamID());
    deessAmountParam = apvts.getRawParameterValue(MazyesParams::deessAmount.getParamID());
    deessListenParam = apvts.getRawParameterValue(MazyesParams::deessListen.getParamID());

    satEnableParam = apvts.getRawParameterValue(MazyesParams::satEnable.getParamID());
    satDriveParam  = apvts.getRawParameterValue(MazyesParams::satDrive.getParamID());
    satToneParam   = apvts.getRawParameterValue(MazyesParams::satTone.getParamID());
    satTypeParam   = apvts.getRawParameterValue(MazyesParams::satType.getParamID());
    satMixParam    = apvts.getRawParameterValue(MazyesParams::satMix.getParamID());

    exciterEnableParam    = apvts.getRawParameterValue(MazyesParams::exciterEnable.getParamID());
    exciterFreqParam      = apvts.getRawParameterValue(MazyesParams::exciterFreq.getParamID());
    exciterDriveParam     = apvts.getRawParameterValue(MazyesParams::exciterDrive.getParamID());
    exciterHarmonicsParam = apvts.getRawParameterValue(MazyesParams::exciterHarmonics.getParamID());
    exciterMixParam       = apvts.getRawParameterValue(MazyesParams::exciterMix.getParamID());

    delayEnableParam   = apvts.getRawParameterValue(MazyesParams::delayEnable.getParamID());
    delayTimeParam     = apvts.getRawParameterValue(MazyesParams::delayTime.getParamID());
    delaySyncParam     = apvts.getRawParameterValue(MazyesParams::delaySync.getParamID());
    delayFeedbackParam = apvts.getRawParameterValue(MazyesParams::delayFeedback.getParamID());
    delayPingPongParam = apvts.getRawParameterValue(MazyesParams::delayPingPong.getParamID());
    delayDuckingParam  = apvts.getRawParameterValue(MazyesParams::delayDucking.getParamID());
    delayLowCutParam   = apvts.getRawParameterValue(MazyesParams::delayLowCut.getParamID());
    delayHighCutParam  = apvts.getRawParameterValue(MazyesParams::delayHighCut.getParamID());
    delayMixParam      = apvts.getRawParameterValue(MazyesParams::delayMix.getParamID());

    reverbEnableParam   = apvts.getRawParameterValue(MazyesParams::reverbEnable.getParamID());
    reverbTypeParam     = apvts.getRawParameterValue(MazyesParams::reverbType.getParamID());
    reverbPredelayParam = apvts.getRawParameterValue(MazyesParams::reverbPredelay.getParamID());
    reverbDecayParam    = apvts.getRawParameterValue(MazyesParams::reverbDecay.getParamID());
    reverbDampingParam  = apvts.getRawParameterValue(MazyesParams::reverbDamping.getParamID());
    reverbWidthParam    = apvts.getRawParameterValue(MazyesParams::reverbWidth.getParamID());
    reverbMixParam      = apvts.getRawParameterValue(MazyesParams::reverbMix.getParamID());

    limiterEnableParam  = apvts.getRawParameterValue(MazyesParams::limiterEnable.getParamID());
    limiterCeilingParam = apvts.getRawParameterValue(MazyesParams::limiterCeiling.getParamID());
    limiterReleaseParam = apvts.getRawParameterValue(MazyesParams::limiterRelease.getParamID());
}

MazyesVocalsAudioProcessor::~MazyesVocalsAudioProcessor() {}

const juce::String MazyesVocalsAudioProcessor::getName() const { return "MAZYES VOCALS"; }
bool MazyesVocalsAudioProcessor::acceptsMidi() const { return true; }
bool MazyesVocalsAudioProcessor::producesMidi() const { return false; }
bool MazyesVocalsAudioProcessor::isMidiEffect() const { return false; }
double MazyesVocalsAudioProcessor::getTailLengthSeconds() const { return 3.0; }

int MazyesVocalsAudioProcessor::getNumPrograms()
{
    return static_cast<int>(MazyesPresets::getFactoryPresets().size());
}

int MazyesVocalsAudioProcessor::getCurrentProgram()
{
    return currentPresetIndex;
}

void MazyesVocalsAudioProcessor::setCurrentProgram(int index)
{
    currentPresetIndex = index;
    MazyesPresets::applyPreset(index, apvts);
}

const juce::String MazyesVocalsAudioProcessor::getProgramName(int index)
{
    const auto& presets = MazyesPresets::getFactoryPresets();
    if (index >= 0 && index < static_cast<int>(presets.size()))
        return presets[index].name;
    return {};
}

void MazyesVocalsAudioProcessor::changeProgramName(int, const juce::String&) {}

void MazyesVocalsAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    noiseGate.prepare(sampleRate, samplesPerBlock);
    pitchEngine.prepare(sampleRate, samplesPerBlock);
    vocalEQ.prepare(sampleRate, samplesPerBlock);
    vocalCompressor.prepare(sampleRate, samplesPerBlock);
    vocalDeEsser.prepare(sampleRate, samplesPerBlock);
    vocalSaturation.prepare(sampleRate, samplesPerBlock);
    vocalExciter.prepare(sampleRate, samplesPerBlock);
    vocalDelay.prepare(sampleRate, samplesPerBlock);
    vocalReverb.prepare(sampleRate, samplesPerBlock);
    vocalLimiter.prepare(sampleRate, samplesPerBlock);

    dryBuffer.setSize(2, samplesPerBlock);
    pitchFIFO.clear();
    meterFIFO.clear();
}

void MazyesVocalsAudioProcessor::releaseResources()
{
    noiseGate.reset();
    pitchEngine.reset();
    vocalEQ.reset();
    vocalCompressor.reset();
    vocalDeEsser.reset();
    vocalSaturation.reset();
    vocalExciter.reset();
    vocalDelay.reset();
    vocalReverb.reset();
    vocalLimiter.reset();
}

bool MazyesVocalsAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void MazyesVocalsAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    if (numSamples == 0 || totalNumInputChannels == 0)
        return;

    // Get Host BPM
    double bpm = 120.0;
    if (auto* playHead = getPlayHead())
    {
        if (auto posOpt = playHead->getPosition())
        {
            if (posOpt->getBpm().hasValue())
                bpm = *posOpt->getBpm();
        }
    }

    // Measure input peaks before processing
    float inPeakL = buffer.getMagnitude(0, 0, numSamples);
    float inPeakR = (totalNumInputChannels > 1) ? buffer.getMagnitude(1, 0, numSamples) : inPeakL;

    // 1. Input Gain
    float inGainDb = inGainParam ? inGainParam->load(std::memory_order_relaxed) : 0.0f;
    float inGainLinear = std::pow(10.0f, inGainDb / 20.0f);
    buffer.applyGain(inGainLinear);

    // Copy to dry buffer for Global Dry/Wet blending
    if (dryBuffer.getNumSamples() < numSamples)
        dryBuffer.setSize(2, numSamples, false, false, true);

    for (int ch = 0; ch < totalNumInputChannels; ++ch)
        dryBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);

    // 2. Noise Gate
    float gateGrDb = 0.0f;
    if (gateEnableParam)
    {
        noiseGate.setParameters(gateEnableParam->load(std::memory_order_relaxed) > 0.5f,
                                gateThreshParam->load(std::memory_order_relaxed),
                                gateAttackParam->load(std::memory_order_relaxed),
                                gateReleaseParam->load(std::memory_order_relaxed),
                                gateRangeParam->load(std::memory_order_relaxed));
        noiseGate.process(buffer, gateGrDb);
    }

    // 3. Pitch Correction
    if (pitchEnableParam)
    {
        pitchEngine.setParameters(pitchEnableParam->load(std::memory_order_relaxed) > 0.5f,
                                  static_cast<int>(pitchKeyParam->load(std::memory_order_relaxed)),
                                  static_cast<MazyesDSP::ScaleType>(static_cast<int>(pitchScaleParam->load(std::memory_order_relaxed))),
                                  pitchSpeedParam->load(std::memory_order_relaxed),
                                  pitchAmountParam->load(std::memory_order_relaxed),
                                  pitchTransitionParam->load(std::memory_order_relaxed),
                                  pitchHumanizeParam->load(std::memory_order_relaxed),
                                  pitchFormantParam->load(std::memory_order_relaxed));
        pitchEngine.process(buffer, pitchFIFO);
    }

    // 4. Vocal EQ
    if (eqEnableParam)
    {
        vocalEQ.setParameters(eqEnableParam->load(std::memory_order_relaxed) > 0.5f,
                              eqHpfFreqParam->load(std::memory_order_relaxed),
                              eqLowFreqParam->load(std::memory_order_relaxed),
                              eqLowGainParam->load(std::memory_order_relaxed),
                              eqMidFreqParam->load(std::memory_order_relaxed),
                              eqMidGainParam->load(std::memory_order_relaxed),
                              eqMidQParam->load(std::memory_order_relaxed),
                              eqHighFreqParam->load(std::memory_order_relaxed),
                              eqHighGainParam->load(std::memory_order_relaxed),
                              eqAirGainParam->load(std::memory_order_relaxed),
                              eqLpfFreqParam->load(std::memory_order_relaxed));
        vocalEQ.process(buffer);
    }

    // 5. De-Esser
    float deessGrDb = 0.0f;
    if (deessEnableParam)
    {
        vocalDeEsser.setParameters(deessEnableParam->load(std::memory_order_relaxed) > 0.5f,
                                   deessFreqParam->load(std::memory_order_relaxed),
                                   deessThreshParam->load(std::memory_order_relaxed),
                                   deessAmountParam->load(std::memory_order_relaxed),
                                   deessListenParam->load(std::memory_order_relaxed) > 0.5f);
        vocalDeEsser.process(buffer, deessGrDb);
    }

    // 6. Compressor
    float compGrDb = 0.0f;
    if (compEnableParam)
    {
        vocalCompressor.setParameters(compEnableParam->load(std::memory_order_relaxed) > 0.5f,
                                      compThreshParam->load(std::memory_order_relaxed),
                                      compRatioParam->load(std::memory_order_relaxed),
                                      compAttackParam->load(std::memory_order_relaxed),
                                      compReleaseParam->load(std::memory_order_relaxed),
                                      compMakeupParam->load(std::memory_order_relaxed),
                                      compKneeParam->load(std::memory_order_relaxed));
        vocalCompressor.process(buffer, compGrDb);
    }

    // 7. Saturation (2x Oversampled)
    if (satEnableParam)
    {
        vocalSaturation.setParameters(satEnableParam->load(std::memory_order_relaxed) > 0.5f,
                                      satDriveParam->load(std::memory_order_relaxed),
                                      satToneParam->load(std::memory_order_relaxed),
                                      static_cast<MazyesDSP::SaturationType>(static_cast<int>(satTypeParam->load(std::memory_order_relaxed))),
                                      satMixParam->load(std::memory_order_relaxed));
        vocalSaturation.process(buffer);
    }

    // 8. Vocal Exciter
    if (exciterEnableParam)
    {
        vocalExciter.setParameters(exciterEnableParam->load(std::memory_order_relaxed) > 0.5f,
                                   exciterFreqParam->load(std::memory_order_relaxed),
                                   exciterDriveParam->load(std::memory_order_relaxed),
                                   static_cast<MazyesDSP::ExciterHarmonics>(static_cast<int>(exciterHarmonicsParam->load(std::memory_order_relaxed))),
                                   exciterMixParam->load(std::memory_order_relaxed));
        vocalExciter.process(buffer);
    }

    // 9. Delay (Ping-Pong / Ducking / Tempo Sync)
    if (delayEnableParam)
    {
        vocalDelay.setParameters(delayEnableParam->load(std::memory_order_relaxed) > 0.5f,
                                 delayTimeParam->load(std::memory_order_relaxed),
                                 static_cast<int>(delaySyncParam->load(std::memory_order_relaxed)),
                                 bpm,
                                 delayFeedbackParam->load(std::memory_order_relaxed),
                                 delayPingPongParam->load(std::memory_order_relaxed) > 0.5f,
                                 delayDuckingParam->load(std::memory_order_relaxed),
                                 delayLowCutParam->load(std::memory_order_relaxed),
                                 delayHighCutParam->load(std::memory_order_relaxed),
                                 delayMixParam->load(std::memory_order_relaxed));
        vocalDelay.process(buffer);
    }

    // 10. Reverb (Plate / Room / Hall / Space)
    if (reverbEnableParam)
    {
        vocalReverb.setParameters(reverbEnableParam->load(std::memory_order_relaxed) > 0.5f,
                                  static_cast<MazyesDSP::ReverbType>(static_cast<int>(reverbTypeParam->load(std::memory_order_relaxed))),
                                  reverbPredelayParam->load(std::memory_order_relaxed),
                                  reverbDecayParam->load(std::memory_order_relaxed),
                                  reverbDampingParam->load(std::memory_order_relaxed),
                                  reverbWidthParam->load(std::memory_order_relaxed),
                                  reverbMixParam->load(std::memory_order_relaxed));
        vocalReverb.process(buffer);
    }

    // 11. Global Dry / Wet Blend
    float dryWetPct = dryWetParam ? dryWetParam->load(std::memory_order_relaxed) : 100.0f;
    float wetAmount = dryWetPct * 0.01f;
    float dryAmount = 1.0f - wetAmount;

    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        float* wetData = buffer.getWritePointer(ch);
        const float* dryData = dryBuffer.getReadPointer(ch);
        for (int i = 0; i < numSamples; ++i)
        {
            wetData[i] = (dryData[i] * dryAmount) + (wetData[i] * wetAmount);
        }
    }

    // 12. Limiter (Peak Catching)
    float limiterGrDb = 0.0f;
    if (limiterEnableParam)
    {
        vocalLimiter.setParameters(limiterEnableParam->load(std::memory_order_relaxed) > 0.5f,
                                   limiterCeilingParam->load(std::memory_order_relaxed),
                                   limiterReleaseParam->load(std::memory_order_relaxed));
        vocalLimiter.process(buffer, limiterGrDb);
    }

    // 13. Output Gain
    float outGainDb = outGainParam ? outGainParam->load(std::memory_order_relaxed) : 0.0f;
    float outGainLinear = std::pow(10.0f, outGainDb / 20.0f);
    buffer.applyGain(outGainLinear);

    // Measure Output Peaks
    float outPeakL = buffer.getMagnitude(0, 0, numSamples);
    float outPeakR = (totalNumInputChannels > 1) ? buffer.getMagnitude(1, 0, numSamples) : outPeakL;

    // Push Meter Telemetry to UI Queue
    MazyesDSP::MeterTelemetry m;
    m.inputPeakL = inPeakL;
    m.inputPeakR = inPeakR;
    m.outputPeakL = outPeakL;
    m.outputPeakR = outPeakR;
    m.gateGainReduction = gateGrDb;
    m.compGainReduction = compGrDb;
    m.deessGainReduction = deessGrDb;
    m.limiterGainReduction = limiterGrDb;
    meterFIFO.push(m);
}

bool MazyesVocalsAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* MazyesVocalsAudioProcessor::createEditor()
{
    return new MazyesVocalsAudioProcessorEditor (*this);
}

void MazyesVocalsAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MazyesVocalsAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MazyesVocalsAudioProcessor();
}
