#pragma once

#include <JuceHeader.h>

namespace Gui
{
	class WaveformCalculation
	{
	public:
		std::vector<float> getGainValues()
		{
			std::vector<float> stackedGains;
			juce::LinearSmoothedValue<float> smoother;

			for (int i = 0; i < gainStack.size(); i++) {
				if (gainStack[i] < smoother.getCurrentValue())
				{
					smoother.setTargetValue(gainStack[i]);
				}
				else
				{
					smoother.setCurrentAndTargetValue(gainStack[i]);
				}
				stackedGains.push_back(smoother.getCurrentValue());
			}
 
			return stackedGains;
		}

		void setBufferStack(int value)
		{
			bufferStack = value * 50;
		}

		void updateWaveform(juce::AudioBuffer<float>& buffer, int inputChannels)
		{			
			samples = buffer.getNumSamples();

			float tempGainVal = 0;

			int stepInterval = 2;
			int sampleStep = samples / stepInterval;

			for (int interval = 0; interval < stepInterval; interval++)
			{
				tempGainVal = 0;

				for (int channel = 0; channel < inputChannels; ++channel)
				{
					float val = juce::Decibels::decibelsToGain(buffer.getRMSLevel(channel, interval * sampleStep, sampleStep));
					tempGainVal += std::abs((val - 1) / inputChannels);
				}
				gainStack = processGainStack(juce::Decibels::gainToDecibels(tempGainVal), gainStack);
			}
		}

	private:
		int samples;
		int bufferStack;
		std::vector<float> gainStack;

		std::vector<float> processGainStack(float newStack, std::vector<float> stack) {

			if (stack.size() == 0)
			{
				stack.push_back(newStack);
				return stack;
			}
			else
			{
				if (stack.size() < bufferStack)
				{
					stack.push_back(newStack);
				}
				else if (stack.size() > bufferStack)
				{
					stack.erase(stack.begin());
				}
				else 
				{
					stack.erase(stack.begin());
					stack.push_back(newStack);
				}

				return stack;
			}
		}

	};
}