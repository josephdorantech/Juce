#pragma once

#include <JuceHeader.h>
#include "WaveformCalculation.h"

/*****ATTENTION*****/
/*
THIS PLUGIN WORKS ALONGSIDE WaveformCalculation.h

--In PluginProcessor.h:
public:
	Gui::WaveformCalculation waveformCalculation;


--In PluginProcessor.cpp:

UNDER THE PROCESS BLOCK:

waveformCalculation.updateWaveform(buffer, totalNumInputChannels);

--In pluginEditor.h:

#include "WaveformScreen/WaveformScreen.h"

inherit:
public juce::Timer,
public juce::Slider::Listener

public:
	void timerCallback() override;
	void sliderValueChanged(juce::Slider* slider) override;

private:
	Gui::WaveformScreen waveformScreen;

--In PluginEditor.cpp:
UNDER THE CONSTRUCTOR:

waveformScreen.WaveformScreenInit(audioProcessor.waveformCalculation);
addAndMakeVisible(waveformScreen);
waveformScreen.windowSlider.addListener(this);
startTimerHz(24);

UNDER RESIZED:
waveformScreen.setBounds(getLocalBounds().withSizeKeepingCentre(512, 430));

ADD:
void WaveformScreenAudioProcessorEditor::timerCallback()
{
	waveformScreen.refreshWindow(audioProcessor.waveformCalculation);
}

void WaveformScreenAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
	if (slider == &waveformScreen.windowSlider) {

		float windowSliderValue = waveformScreen.windowSlider.getValue();

		waveformScreen.windowValue = windowSliderValue;
		audioProcessor.waveformCalculation.setBufferStack(windowSliderValue);
	}
}
*/

namespace Gui
{
	class WaveformScreen : public juce::Component
	{
	public:
		void WaveformScreenInit(WaveformCalculation calculation)
		{
			calculation.setBufferStack(windowValue);

			auto bounds = getLocalBounds().toFloat();
			auto bottomLeft = bounds.getBottomLeft();

			windowSlider.setBounds(
				bottomLeft.x,
				bottomLeft.y - 30,
				bounds.getWidth(),
				30);

			windowSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
			windowSlider.setRange(1, 11, 1);
			windowSlider.setSliderStyle(juce::Slider::LinearHorizontal);
			windowSlider.setValue(windowValue);
			addAndMakeVisible(windowSlider);
		}

		void paint(juce::Graphics& g) override
		{
			auto bounds = getLocalBounds().toFloat();
			createBackgrounds(g, bounds);
			showSampleSize(g, bounds);

			createWaveform(g, bounds);
		}

		void createBackgrounds(juce::Graphics& g, juce::Rectangle<float> bounds)
		{
			/*Window background*/			
			g.setColour(juce::Colours::black);
			g.fillRoundedRectangle(bounds, 5.0f);

			//Slider background
			auto boundsWithRoomForSlider = bounds.removeFromBottom(30);
			g.setColour(juce::Colours::slategrey);
			g.fillRect(boundsWithRoomForSlider);
		}

		void createWaveform(juce::Graphics& g, juce::Rectangle<float> bounds)
		{			
			g.setColour(juce::Colours::white);

			int gainValuesSize = gainValues.size();
			float windowBottom = bounds.getBottom();
			float windowWidth = bounds.getWidth();
			float windowHeight = bounds.getHeight();
			auto windowLeft = bounds.getTopLeft();
			float xCoorWindow = windowLeft.x;

			for (int i = 0; i < gainValuesSize; i++)
			{
				float lineLength = decibelLineHeight(gainValues[i], windowHeight - 30);

				float modifier = gainValuesSize / windowWidth;
				g.drawVerticalLine(					
					(xCoorWindow + i) / modifier,
					windowBottom - 30 - lineLength,
					windowBottom - 30);
			}
		}

		float decibelLineHeight(float dbVal, float windowHeight)
		{
			float dynamicPerc = (std::abs(dbVal) / 100) - 1;
			dynamicPerc = std::abs(dynamicPerc);

			float lineLen = dynamicPerc * windowHeight;

			return lineLen;
		}

		void showSampleSize(juce::Graphics& g, juce::Rectangle<float> bounds)
		{
			int gainValuesSize = gainValues.size();
			auto windowLeft = bounds.getTopRight();

			auto topEdge = bounds.removeFromTop(40.0);
			auto textLevel = topEdge.removeFromBottom(30.0);
			auto textSpace = textLevel.removeFromRight(bounds.getWidth() * 0.2);
			g.setColour(juce::Colours::slategrey);
			g.fillRoundedRectangle(textSpace, 2.0f);
			g.setColour(juce::Colours::white);
			g.setFont(12.0f);
			g.drawFittedText(std::to_string(gainValuesSize) + " samples", textSpace.toNearestInt(), juce::Justification::centred, 1);
		}

		void refreshWindow(WaveformCalculation calcs)
		{
			gainValues.clear();
			gainValues = calcs.getGainValues();
			this->repaint();
		}

		void setWindowValue(int value) 
		{ 
			windowValue = value;
		}

		//Window
		int windowValue = 6;
		juce::Slider windowSlider;

	private:
		std::vector<float> gainValues;
	};
}