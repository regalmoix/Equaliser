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

class LookNFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(  juce::Graphics&,
                            int x, int y, int width, int height,
                            float sliderPosProportional,
                            float rotaryStartAngle,
                            float rotaryEndAngle,
                            juce::Slider&
                        ) override;
};

class RotarySliderWithLabels : public juce::Slider
{
public:
    // Delegating superclass constructor to initialise the super class members according to the below parameters
    // If we did not do this, juce::Slider would have been default constructed
    RotarySliderWithLabels(juce::RangedAudioParameter& param, const juce::String& unitSuffix) 
        : juce::Slider  (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox),
          parameter     (&param),
          unit          (unitSuffix)
    {
        setLookAndFeel(&lnf);
    }

    ~RotarySliderWithLabels()
    {
        setLookAndFeel(nullptr);
    }

    struct LabelWithPosition
    {
        juce::String            labelText;
        float                   sliderPos;

        float getAngularPosition(const float startAngle, const float endAngle) const
        {
            return jmap (sliderPos, 0.0f, 1.0f, startAngle, endAngle);
        }
    };

    juce::Array<LabelWithPosition>  labels;

    void drawLabelAtPosition(juce::Graphics& g, const LabelWithPosition& label, const Point<float>& labelCenter);
    void paint(Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14;}
    juce::String getDisplayString() const;


private:
    LookNFeel lnf;
    juce::RangedAudioParameter* parameter;
    juce::String                unit;
};

class ResponseCurveComponent :  public juce::AudioProcessorParameter::Listener, 
                                public juce::Timer,
                                public juce::Component
{
public:
    ResponseCurveComponent(VstpluginAudioProcessor&);
    ~ResponseCurveComponent();

    void parameterValueChanged   (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override { /* No Op */ }

    void timerCallback() override;

    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    VstpluginAudioProcessor&    processor;
    juce::Atomic<bool>          paramsChanged { false };
    MonoChain                   monoChain;
    juce::Image                 backgroundGrid;

    void updateChain();
    juce::Rectangle<int> getRenderArea();
};

class VstpluginAudioProcessorEditor  :  public AudioProcessorEditor
{
public:
    VstpluginAudioProcessorEditor (VstpluginAudioProcessor&);
    ~VstpluginAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    VstpluginAudioProcessor& processor;

    RotarySliderWithLabels peakFrequencySlider;
    RotarySliderWithLabels peakGainSlider;
    RotarySliderWithLabels peakQualitySlider;

    RotarySliderWithLabels lowCutFrequencySlider;
    RotarySliderWithLabels highCutFrequencySlider;

    RotarySliderWithLabels lowCutSlopeSlider;
    RotarySliderWithLabels highCutSlopeSlider;

    ResponseCurveComponent  responseCurveComponent;


    using APVTS = juce::AudioProcessorValueTreeState;

    APVTS::SliderAttachment peakFrequencySliderAttachment;
    APVTS::SliderAttachment peakGainSliderAttachment;
    APVTS::SliderAttachment peakQualitySliderAttachment;

    APVTS::SliderAttachment lowCutFrequencySliderAttachment;
    APVTS::SliderAttachment highCutFrequencySliderAttachment;

    APVTS::SliderAttachment lowCutSlopeSliderAttachment;
    APVTS::SliderAttachment highCutSlopeSliderAttachment;


    std::vector<juce::Component*> getComponents();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VstpluginAudioProcessorEditor)
};
