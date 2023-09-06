#pragma once

#include <JuceHeader.h>
#include "MeterCalculation.h"

namespace Gui
{
	/*********ATTENTION***********/
	/*

	THERE ARE TWO PARTS TO THE METERS:

	MeterCalculation.h --> This is the calculation side. It goes in the processor area after "denormals". To include, simply add "meterValues.updateLevel(buffer);"
	and
	HorizontalMeter.h/other meters --> This is the visualisation side. Setup procedure is below.


	When using this meter class:

	--In PluginEditor.h:
	inherit from public juce::Timer

	public:
		void timerCallback() override;
	private:
		Gui::HorizontalMeter horizontalMeterL;
		Gui::HorizontalMeter horizontalMeterR;

	--In PluginEditor.cpp constructor:

	addAndMakeVisible(horizontalMeterL);
	addAndMakeVisible(horizontalMeterR);

	horizontalMeterL.meterInit(0);
	horizontalMeterR.meterInit(1);

	startTimerHz(24);

	--in plugineditor.cpp resized:

	horizontalMeterL.setBounds(100, 100, 200, 15);
	horizontalMeterL.setBounds(100, 120, 200, 15);

	--In pluginEditor.cpp, add the following to the timerCallback:

	void LevelMetersAudioProcessorEditor::timerCallback()
	{
		horizontalMeterL.refreshMeter(audioProcessor.meterValues);
		horizontalMeterR.refreshMeter(audioProcessor.meterValues);
	}

	*/
	/********************************/

	class VerticalMeterTypeOne : public juce::Component
	{
	public:

		void meterInit(int channelNum) {

			channelNumber = channelNum;
			grill = juce::ImageCache::getFromMemory(BinaryData::LevelMeterVerticalGrid_png, BinaryData::LevelMeterVerticalGrid_pngSize);
		}

		void paint(juce::Graphics& g) override
		{
			auto bounds = getLocalBounds().toFloat().reduced(2.0f);
			g.setColour(juce::Colours::black);
			g.fillRect(bounds);

			g.setGradientFill(gradient);
			const auto scaledY = juce::jmap(level, -60.0f, +6.0f, 0.0f, static_cast<float>(getHeight()));

			g.fillRect(bounds.removeFromBottom(scaledY));
		}

		void paintOverChildren(juce::Graphics& g) override
		{
			g.drawImage(grill, getLocalBounds().toFloat());
		}

		void resized() override
		{
			const auto bounds = getLocalBounds().toFloat();
			gradient = juce::ColourGradient{
				juce::Colours::green,
				bounds.getBottomLeft(),
				juce::Colours::red,
				bounds.getTopLeft(),
				false
			};
			gradient.addColour(0.5, juce::Colours::yellow);
		}


		/// <summary>
		/// Sets the direct level value
		/// </summary>
		/// <param name="value"></param>
		void setLevel(const float value) { level = value; }

		float getLevel() {
			return level;
		}

		void refreshMeter(MeterCalculation calculation)
		{
			float newLevel = calculation.getRmsValue(channelNumber);
			setLevel(newLevel);

			this->repaint();
		}

	private:
		float level = -60.0f;
		int channelNumber = 0;
		juce::ColourGradient gradient{};
		juce::Image grill;
	};
}
