#pragma once
#include <JuceHeader.h>

namespace Gui 
{
	class MeterCalculation
	{
	public:
		/// <summary>
		/// Initialises meter. Place under PrepareToPlay block in PluginProcessor.cpp
		/// </summary>
		/// <param name="sampleRate"></param>
		void meterInit(double sampleRate) 
		{
			rmsLevelLeft.reset(sampleRate, 0.5);
			rmsLevelRight.reset(sampleRate, 0.5);

			rmsLevelLeft.setCurrentAndTargetValue(-100.0f);
			rmsLevelRight.setCurrentAndTargetValue(-100.0f);
		}

		/// <summary>
		/// Updates the RMS levels. Call inside audio process block
		/// </summary>
		/// <param name="buffer"></param>
		void updateLevel(juce::AudioBuffer<float>& buffer)
        {
            rmsLevelLeft.skip(buffer.getNumSamples());
            rmsLevelRight.skip(buffer.getNumSamples());
            {
                const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
                if (value < rmsLevelLeft.getCurrentValue())
                    rmsLevelLeft.setTargetValue(value);
                else
                    rmsLevelLeft.setCurrentAndTargetValue(value);
            }

            {
                const auto value = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
                if (value < rmsLevelRight.getCurrentValue())
                    rmsLevelRight.setTargetValue(value);
                else
                    rmsLevelRight.setCurrentAndTargetValue(value);
            }
		}

		/// <summary>
		/// Returns direct level value from smoothed value
		/// </summary>
		/// <param name="channel"></param>
		/// <returns></returns>
		float getRmsValue(const int channel) const 
        {
			jassert(channel == 0 || channel == 1);
			if (channel == 0)
			{
				return rmsLevelLeft.getCurrentValue();
			}
			else if (channel == 1)
			{
				return rmsLevelRight.getCurrentValue();
			}
			else
			{
				return 0;
			}
		}
	private:
		juce::LinearSmoothedValue<float> rmsLevelLeft, rmsLevelRight;
	};
}