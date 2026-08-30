#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "Parameters.h"
#include <map>
#include <string>
#include <vector>

namespace MazyesPresets
{
    struct Preset
    {
        juce::String name;
        std::map<juce::String, float> values;
    };

    inline const std::vector<Preset>& getFactoryPresets()
    {
        static const std::vector<Preset> presets =
        {
            {
                "RAGE VOCAL",
                {
                    { MazyesParams::pitchEnable.getParamID(), 1.0f },
                    { MazyesParams::pitchSpeed.getParamID(), 0.0f }, // Hard tune 0ms
                    { MazyesParams::pitchAmount.getParamID(), 100.0f },
                    { MazyesParams::pitchHumanize.getParamID(), 0.0f },
                    { MazyesParams::gateEnable.getParamID(), 1.0f },
                    { MazyesParams::gateThresh.getParamID(), -45.0f },
                    { MazyesParams::compEnable.getParamID(), 1.0f },
                    { MazyesParams::compThresh.getParamID(), -22.0f },
                    { MazyesParams::compRatio.getParamID(), 6.0f },
                    { MazyesParams::compAttack.getParamID(), 5.0f },
                    { MazyesParams::compMakeup.getParamID(), 6.0f },
                    { MazyesParams::satEnable.getParamID(), 1.0f },
                    { MazyesParams::satDrive.getParamID(), 12.0f },
                    { MazyesParams::satType.getParamID(), 1.0f }, // Tube
                    { MazyesParams::satTone.getParamID(), 25.0f },
                    { MazyesParams::exciterEnable.getParamID(), 1.0f },
                    { MazyesParams::exciterDrive.getParamID(), 60.0f },
                    { MazyesParams::eqAirGain.getParamID(), 4.5f },
                    { MazyesParams::delayEnable.getParamID(), 1.0f },
                    { MazyesParams::delaySync.getParamID(), 3.0f }, // 1/4
                    { MazyesParams::delayMix.getParamID(), 20.0f },
                    { MazyesParams::reverbEnable.getParamID(), 1.0f },
                    { MazyesParams::reverbDecay.getParamID(), 1.8f },
                    { MazyesParams::reverbMix.getParamID(), 22.0f }
                }
            },
            {
                "OPIUM",
                {
                    { MazyesParams::pitchEnable.getParamID(), 1.0f },
                    { MazyesParams::pitchSpeed.getParamID(), 0.0f },
                    { MazyesParams::pitchAmount.getParamID(), 100.0f },
                    { MazyesParams::pitchFormant.getParamID(), 1.0f }, // Slight high formant
                    { MazyesParams::satEnable.getParamID(), 1.0f },
                    { MazyesParams::satDrive.getParamID(), 14.0f },
                    { MazyesParams::satType.getParamID(), 0.0f }, // Tape
                    { MazyesParams::satTone.getParamID(), 40.0f },
                    { MazyesParams::compThresh.getParamID(), -26.0f },
                    { MazyesParams::compRatio.getParamID(), 8.0f },
                    { MazyesParams::deessAmount.getParamID(), 80.0f },
                    { MazyesParams::delayEnable.getParamID(), 1.0f },
                    { MazyesParams::delaySync.getParamID(), 5.0f }, // 1/8D
                    { MazyesParams::delayFeedback.getParamID(), 55.0f },
                    { MazyesParams::delayMix.getParamID(), 30.0f },
                    { MazyesParams::reverbType.getParamID(), 3.0f }, // Space
                    { MazyesParams::reverbDecay.getParamID(), 4.5f },
                    { MazyesParams::reverbMix.getParamID(), 32.0f }
                }
            },
            {
                "DARK TRAP",
                {
                    { MazyesParams::pitchEnable.getParamID(), 1.0f },
                    { MazyesParams::pitchSpeed.getParamID(), 0.0f },
                    { MazyesParams::pitchFormant.getParamID(), -1.5f }, // Dark down formant
                    { MazyesParams::gateEnable.getParamID(), 1.0f },
                    { MazyesParams::gateThresh.getParamID(), -42.0f },
                    { MazyesParams::eqLowGain.getParamID(), 2.0f },
                    { MazyesParams::satDrive.getParamID(), 8.0f },
                    { MazyesParams::satType.getParamID(), 2.0f }, // Warm
                    { MazyesParams::compThresh.getParamID(), -20.0f },
                    { MazyesParams::compRatio.getParamID(), 5.0f },
                    { MazyesParams::delayEnable.getParamID(), 1.0f },
                    { MazyesParams::delaySync.getParamID(), 3.0f },
                    { MazyesParams::delayDucking.getParamID(), 60.0f },
                    { MazyesParams::reverbDecay.getParamID(), 2.5f },
                    { MazyesParams::reverbMix.getParamID(), 18.0f }
                }
            },
            {
                "CLEAN VOCAL",
                {
                    { MazyesParams::pitchEnable.getParamID(), 1.0f },
                    { MazyesParams::pitchSpeed.getParamID(), 35.0f }, // Natural retune
                    { MazyesParams::pitchAmount.getParamID(), 80.0f },
                    { MazyesParams::pitchHumanize.getParamID(), 40.0f },
                    { MazyesParams::gateEnable.getParamID(), 0.0f },
                    { MazyesParams::eqHpfFreq.getParamID(), 100.0f },
                    { MazyesParams::eqAirGain.getParamID(), 3.0f },
                    { MazyesParams::compThresh.getParamID(), -16.0f },
                    { MazyesParams::compRatio.getParamID(), 3.5f },
                    { MazyesParams::compAttack.getParamID(), 20.0f },
                    { MazyesParams::deessAmount.getParamID(), 50.0f },
                    { MazyesParams::satDrive.getParamID(), 2.0f },
                    { MazyesParams::satMix.getParamID(), 40.0f },
                    { MazyesParams::delayEnable.getParamID(), 0.0f },
                    { MazyesParams::reverbType.getParamID(), 2.0f }, // Hall
                    { MazyesParams::reverbDecay.getParamID(), 2.8f },
                    { MazyesParams::reverbMix.getParamID(), 15.0f }
                }
            },
            {
                "HARD TUNE",
                {
                    { MazyesParams::pitchEnable.getParamID(), 1.0f },
                    { MazyesParams::pitchSpeed.getParamID(), 0.0f }, // Instant snap
                    { MazyesParams::pitchAmount.getParamID(), 100.0f },
                    { MazyesParams::pitchHumanize.getParamID(), 0.0f },
                    { MazyesParams::pitchTransition.getParamID(), 0.0f },
                    { MazyesParams::exciterEnable.getParamID(), 1.0f },
                    { MazyesParams::exciterDrive.getParamID(), 50.0f },
                    { MazyesParams::exciterMix.getParamID(), 45.0f },
                    { MazyesParams::eqAirGain.getParamID(), 3.5f },
                    { MazyesParams::compThresh.getParamID(), -20.0f },
                    { MazyesParams::compRatio.getParamID(), 4.0f },
                    { MazyesParams::satDrive.getParamID(), 5.0f }
                }
            },
            {
                "MELODIC",
                {
                    { MazyesParams::pitchEnable.getParamID(), 1.0f },
                    { MazyesParams::pitchSpeed.getParamID(), 12.0f },
                    { MazyesParams::pitchAmount.getParamID(), 95.0f },
                    { MazyesParams::pitchHumanize.getParamID(), 20.0f },
                    { MazyesParams::delayEnable.getParamID(), 1.0f },
                    { MazyesParams::delaySync.getParamID(), 2.0f }, // 1/8
                    { MazyesParams::delayFeedback.getParamID(), 40.0f },
                    { MazyesParams::delayMix.getParamID(), 25.0f },
                    { MazyesParams::reverbType.getParamID(), 1.0f }, // Plate
                    { MazyesParams::reverbDecay.getParamID(), 3.2f },
                    { MazyesParams::reverbMix.getParamID(), 28.0f }
                }
            },
            {
                "UNDERGROUND",
                {
                    { MazyesParams::pitchEnable.getParamID(), 1.0f },
                    { MazyesParams::pitchSpeed.getParamID(), 0.0f },
                    { MazyesParams::eqHpfFreq.getParamID(), 180.0f },
                    { MazyesParams::eqLpfFreq.getParamID(), 14000.0f },
                    { MazyesParams::satEnable.getParamID(), 1.0f },
                    { MazyesParams::satDrive.getParamID(), 18.0f },
                    { MazyesParams::satType.getParamID(), 3.0f }, // Foldback
                    { MazyesParams::satMix.getParamID(), 90.0f },
                    { MazyesParams::compThresh.getParamID(), -24.0f },
                    { MazyesParams::compRatio.getParamID(), 8.0f },
                    { MazyesParams::reverbType.getParamID(), 0.0f }, // Room
                    { MazyesParams::reverbDecay.getParamID(), 1.2f },
                    { MazyesParams::reverbMix.getParamID(), 18.0f }
                }
            },
            {
                "AIRY",
                {
                    { MazyesParams::pitchEnable.getParamID(), 1.0f },
                    { MazyesParams::pitchSpeed.getParamID(), 20.0f },
                    { MazyesParams::eqAirGain.getParamID(), 6.0f },
                    { MazyesParams::exciterEnable.getParamID(), 1.0f },
                    { MazyesParams::exciterFreq.getParamID(), 10000.0f },
                    { MazyesParams::exciterDrive.getParamID(), 70.0f },
                    { MazyesParams::exciterMix.getParamID(), 55.0f },
                    { MazyesParams::compThresh.getParamID(), -18.0f },
                    { MazyesParams::compRatio.getParamID(), 3.0f },
                    { MazyesParams::reverbType.getParamID(), 3.0f }, // Space
                    { MazyesParams::reverbDecay.getParamID(), 5.0f },
                    { MazyesParams::reverbMix.getParamID(), 35.0f }
                }
            },
            {
                "RADIO",
                {
                    { MazyesParams::eqEnable.getParamID(), 1.0f },
                    { MazyesParams::eqHpfFreq.getParamID(), 450.0f },
                    { MazyesParams::eqLpfFreq.getParamID(), 4000.0f },
                    { MazyesParams::eqMidFreq.getParamID(), 1800.0f },
                    { MazyesParams::eqMidGain.getParamID(), 6.0f },
                    { MazyesParams::satEnable.getParamID(), 1.0f },
                    { MazyesParams::satDrive.getParamID(), 16.0f },
                    { MazyesParams::satType.getParamID(), 4.0f }, // Clip
                    { MazyesParams::compThresh.getParamID(), -30.0f },
                    { MazyesParams::compRatio.getParamID(), 12.0f },
                    { MazyesParams::delayEnable.getParamID(), 1.0f },
                    { MazyesParams::delayTime.getParamID(), 60.0f }, // Slap
                    { MazyesParams::delayMix.getParamID(), 15.0f },
                    { MazyesParams::reverbEnable.getParamID(), 0.0f }
                }
            },
            {
                "AGGRESSIVE",
                {
                    { MazyesParams::pitchEnable.getParamID(), 1.0f },
                    { MazyesParams::pitchSpeed.getParamID(), 0.0f },
                    { MazyesParams::gateEnable.getParamID(), 1.0f },
                    { MazyesParams::gateThresh.getParamID(), -38.0f },
                    { MazyesParams::satEnable.getParamID(), 1.0f },
                    { MazyesParams::satDrive.getParamID(), 20.0f },
                    { MazyesParams::satType.getParamID(), 1.0f }, // Tube
                    { MazyesParams::compThresh.getParamID(), -28.0f },
                    { MazyesParams::compRatio.getParamID(), 10.0f },
                    { MazyesParams::compAttack.getParamID(), 1.0f },
                    { MazyesParams::compMakeup.getParamID(), 10.0f },
                    { MazyesParams::exciterEnable.getParamID(), 1.0f },
                    { MazyesParams::exciterDrive.getParamID(), 65.0f },
                    { MazyesParams::limiterCeiling.getParamID(), -0.1f }
                }
            }
        };

        return presets;
    }

    inline void applyPreset(int presetIndex, juce::AudioProcessorValueTreeState& apvts)
    {
        const auto& presets = getFactoryPresets();
        if (presetIndex < 0 || presetIndex >= static_cast<int>(presets.size()))
            return;

        const auto& p = presets[presetIndex];
        for (const auto& [paramId, val] : p.values)
        {
            if (auto* param = apvts.getParameter(paramId))
            {
                param->setValueNotifyingHost(param->convertTo0to1(val));
            }
        }
    }
}
