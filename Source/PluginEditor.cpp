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
    : AudioProcessorEditor (&p), processor (p),
      peakFrequencySliderAttachment (processor.apvts, "Peak Freq", peakFrequencySlider),
      peakGainSliderAttachment      (processor.apvts, "Peak Gain", peakGainSlider),
      peakQualitySliderAttachment   (processor.apvts, "Peak Q",    peakQualitySlider),


      lowCutFrequencySliderAttachment (processor.apvts, "LowCut Freq",  lowCutFrequencySlider),
      highCutFrequencySliderAttachment(processor.apvts, "HighCut Freq", highCutFrequencySlider),

      lowCutSlopeSliderAttachment     (processor.apvts, "LowCut Slope",  lowCutSlopeSlider),
      highCutSlopeSliderAttachment    (processor.apvts, "HighCut Slope", highCutSlopeSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    for (auto* component : getComponents())
    {
        addAndMakeVisible(component);
    }

    // Register as a listener
    auto& parameters = processor.getParameters();
    for (auto& param : parameters)
    {
        param->addListener(this);
    }

    startTimerHz(60);
    setSize (600, 400);
}

VstpluginAudioProcessorEditor::~VstpluginAudioProcessorEditor()
{
    // De register as the listener
    auto& parameters = processor.getParameters();
    for (auto& param : parameters)
    {
        param->removeListener(this);
    }
    stopTimer();
}

//==============================================================================
void VstpluginAudioProcessorEditor::paint (Graphics& g)
{
    using namespace juce;

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::black);
    auto bounds     = getLocalBounds();
    auto frequencyResponseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);
    auto width      = frequencyResponseArea.getWidth();

    CutFilter& lowCut   = monoChain.get<ChainPostitions::LowCut>();
    Filter& peak        = monoChain.get<ChainPostitions::Peak>();
    CutFilter& highCut  = monoChain.get<ChainPostitions::HighCut>();

    auto sampleRate = processor.getSampleRate();

    // For each pixel in 0 to width - 1 index, we want the magnitude (the height of the graph) in dB
    std::vector<double> magnitudesDecibel(width);

    for (int i = 0; i < width; ++i)
    {
        // Initialise to 1 gain (ie no change of volume)
        double magnitude = 1.0f;
        // On log scale, get frequency corresponding to pixel #i given 20Hz is i = 0 and 20KHz is i = width - 1
        double frequency = mapToLog10((double) i / (double) width, 20.0, 20000.0);

        if (!monoChain.isBypassed<ChainPostitions::Peak>())
            magnitude *= peak.coefficients->getMagnitudeForFrequency(frequency, sampleRate);

        if (!lowCut.isBypassed<0>())
            magnitude *= lowCut.get<0>().coefficients->getMagnitudeForFrequency(frequency, sampleRate);
        if (!lowCut.isBypassed<1>())
            magnitude *= lowCut.get<1>().coefficients->getMagnitudeForFrequency(frequency, sampleRate);
        if (!lowCut.isBypassed<2>())
            magnitude *= lowCut.get<2>().coefficients->getMagnitudeForFrequency(frequency, sampleRate);
        if (!lowCut.isBypassed<3>())
            magnitude *= lowCut.get<3>().coefficients->getMagnitudeForFrequency(frequency, sampleRate);

        if (!highCut.isBypassed<0>())
            magnitude *= highCut.get<0>().coefficients->getMagnitudeForFrequency(frequency, sampleRate);
        if (!highCut.isBypassed<1>())
            magnitude *= highCut.get<1>().coefficients->getMagnitudeForFrequency(frequency, sampleRate);
        if (!highCut.isBypassed<2>())
            magnitude *= highCut.get<2>().coefficients->getMagnitudeForFrequency(frequency, sampleRate);
        if (!highCut.isBypassed<3>())
            magnitude *= highCut.get<3>().coefficients->getMagnitudeForFrequency(frequency, sampleRate);

        magnitudesDecibel[i] = Decibels::gainToDecibels(magnitude);;
    }

    Path responseCurve;
    const double minY = frequencyResponseArea.getBottom();
    const double maxY = frequencyResponseArea.getY();

    auto  mapMagnitudeToPixel = [minY, maxY] (double input)
    {
        double pxval = jmap(input, -24.0, +24.0, minY, maxY);
        return pxval;
    };

    responseCurve.startNewSubPath(frequencyResponseArea.getX(), mapMagnitudeToPixel(magnitudesDecibel.front()));

    for (size_t x = 1; x < magnitudesDecibel.size(); ++x)
    {
        responseCurve.lineTo(frequencyResponseArea.getX() + x, mapMagnitudeToPixel(magnitudesDecibel[x]));
    }

    g.setColour(Colours::orange);

    g.drawRoundedRectangle(frequencyResponseArea.toFloat(), 4.0f, 1.0f);

    g.setColour(Colours::white);
    g.strokePath(responseCurve, PathStrokeType(2.0f));
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

void VstpluginAudioProcessorEditor::parameterValueChanged(int parameterIndex, float newValue)
{
    paramsChanged.set(true);
}

void VstpluginAudioProcessorEditor::timerCallback()
{
    if (paramsChanged.compareAndSetBool(false, true))
    {
        // Update mono chain of editor
        ChainSettings  chainSettings    = getChainSettings(processor.apvts);
        CoefficientPtr peakCoefficients = makePeakFilter(chainSettings, processor.getSampleRate());
        updateCoefficients(monoChain.get<ChainPostitions::Peak>().coefficients, peakCoefficients); 

        // Do a repaint
        repaint();
    }
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