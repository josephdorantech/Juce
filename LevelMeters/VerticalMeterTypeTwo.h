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


	class Bulb : public juce::Component
	{
	public:
		Bulb(const juce::Colour& c) : colour(c) {}

		void paint(juce::Graphics& g) override
		{
			const auto delta = 4.0f;
			const auto bounds = getLocalBounds().toFloat().reduced(delta);
			const auto side = juce::jmin(bounds.getWidth(), bounds.getHeight());
			const auto bulbFillBounds = juce::Rectangle<float>{ bounds.getX(), bounds.getY(), side, side };
			if (isOn)
			{
				g.setColour(colour);
			}
			else
			{
				g.setColour(juce::Colours::darkgrey);
			}

			g.fillEllipse(bulbFillBounds);
			g.setColour(juce::Colours::darkgrey);
			g.drawEllipse(bulbFillBounds, 1.0f);

			//Bulb glow

			if (isOn)
			{
				g.setGradientFill(
					juce::ColourGradient(colour.withAlpha(0.3f),
						bulbFillBounds.getCentre(),
						colour.withLightness(1.5f).withAlpha(0.f),
						{},
						true));

				g.fillEllipse(bulbFillBounds.expanded(delta));
			}
		}

		void setState(const bool state) { isOn = state; }

	private:
		bool isOn = false;
		juce::Colour colour{};
	};

	class VerticalMeterTypeTwo : public juce::Component
	{
	public:

		void meterInit(int channelNum) {

			channelNumber = channelNum;
		}

		void paint(juce::Graphics& g) override
		{
			const auto gainLevel = juce::jmap(level, -60.0f, 6.0f, 0.0f, 1.0f);

			for (auto i = 0; i < totalNumberOfBulbs; i++) 
			{
				if (gainLevel >= static_cast<float>(i + 1) / totalNumberOfBulbs)
				{
					bulbs[i]->setState(true);
				}
				else 
				{
					bulbs[i]->setState(false);
				}
			}
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

			auto bulbBounds = getLocalBounds();
			const auto bulbHeight = bulbBounds.getHeight() / totalNumberOfBulbs;
			bulbs.clear();

			for (auto i = 0; i < totalNumberOfBulbs; i++) 
			{
				auto bulb = std::make_unique<Bulb>(gradient.getColourAtPosition(static_cast<double>(i) / totalNumberOfBulbs));
				addAndMakeVisible(bulb.get());
				bulb->setBounds(bulbBounds.removeFromBottom(bulbHeight));
				bulbs.push_back(std::move(bulb));
			}

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

		std::vector<std::unique_ptr<Bulb>> bulbs;
		const int totalNumberOfBulbs = 10;
	};
}
