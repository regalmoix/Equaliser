/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class RotarySlider : public juce::Slider
{
public:
    // Delegating superclass constructor to initialise the super class members according to the below parameters
    // If we did not do this, juce::Slider would have been default constructed
    RotarySlider() 
        : juce::Slider (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox)
    {
        
    }
};

class VstpluginAudioProcessorEditor  :  public AudioProcessorEditor, 
                                        public juce::AudioProcessorParameter::Listener, 
                                        public juce::Timer
{
public:
    VstpluginAudioProcessorEditor (VstpluginAudioProcessor&);
    ~VstpluginAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    void parameterValueChanged   (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override { /* No Op */ }

    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    VstpluginAudioProcessor& processor;

    juce::Atomic<bool> paramsChanged { false };

    RotarySlider peakFrequencySlider;
    RotarySlider peakGainSlider;
    RotarySlider peakQualitySlider;

    RotarySlider lowCutFrequencySlider;
    RotarySlider highCutFrequencySlider;

    RotarySlider lowCutSlopeSlider;
    RotarySlider highCutSlopeSlider;

    using APVTS = juce::AudioProcessorValueTreeState;

    APVTS::SliderAttachment peakFrequencySliderAttachment;
    APVTS::SliderAttachment peakGainSliderAttachment;
    APVTS::SliderAttachment peakQualitySliderAttachment;

    APVTS::SliderAttachment lowCutFrequencySliderAttachment;
    APVTS::SliderAttachment highCutFrequencySliderAttachment;

    APVTS::SliderAttachment lowCutSlopeSliderAttachment;
    APVTS::SliderAttachment highCutSlopeSliderAttachment;

    MonoChain monoChain;

    std::vector<juce::Component*> getComponents();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VstpluginAudioProcessorEditor)
};
