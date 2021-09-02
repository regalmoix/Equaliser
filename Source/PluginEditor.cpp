/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VstpluginAudioProcessorEditor::VstpluginAudioProcessorEditor (VstpluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    for (auto* component : getComponents())
    {
        addAndMakeVisible(component);
    }
    setSize (600, 400);
}

VstpluginAudioProcessorEditor::~VstpluginAudioProcessorEditor()
{
}

//==============================================================================
void VstpluginAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void VstpluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();

    // Reserves space of 33% for the frequency spectrum on top
    auto frequencyResponseArea  = bounds.removeFromTop(bounds.getHeight() * 0.33);
    // Reserves space of 33% for the low cut controls, 66% remains
    auto lowCutArea             = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    // Reserves space of 33% for the frequency spectrum (0.5 * 66% = 33%)
    auto highCutArea            = bounds.removeFromRight(bounds.getWidth() * 0.50);

    auto lowCutFreqSliderArea   = lowCutArea.removeFromTop(bounds.getHeight() * 0.50);
    auto lowCutSlopeSliderArea  = lowCutArea;
    
    auto highCutFreqSliderArea  = highCutArea.removeFromTop(bounds.getHeight() * 0.50);
    auto highCutSlopeSliderArea = highCutArea;

    // Top 33% of the middle 33% block
    auto peakFrequencySliderArea= bounds.removeFromTop(bounds.getHeight() * 0.33);
    // Mid 33% of the middle 33% block
    auto peakGainSliderArea     = bounds.removeFromTop(bounds.getHeight() * 0.50);
    // Low 33% of the middle 33% block
    auto peakQualitySliderArea  = bounds;

    lowCutFrequencySlider .setBounds(lowCutFreqSliderArea) ;
    highCutFrequencySlider.setBounds(highCutFreqSliderArea);
    peakFrequencySlider   .setBounds(peakFrequencySliderArea);

    peakGainSlider        .setBounds(peakGainSliderArea);
    peakQualitySlider     .setBounds(peakQualitySliderArea);

    lowCutSlopeSlider     .setBounds(lowCutSlopeSliderArea);
    highCutSlopeSlider    .setBounds(highCutSlopeSliderArea);

}

std::vector<juce::Component*> VstpluginAudioProcessorEditor::getComponents()
{
    return {
        &peakFrequencySlider,
        &peakGainSlider,
        &peakQualitySlider,
        &lowCutFrequencySlider,
        &highCutFrequencySlider,
        &lowCutSlopeSlider,
        &highCutSlopeSlider
    };
}