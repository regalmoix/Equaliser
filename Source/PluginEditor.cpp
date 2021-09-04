/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

void LookNFeel::drawRotarySlider(   
                                juce::Graphics& g,
                                int x, int y, int width, int height,
                                float sliderPosProportional,
                                float rotaryStartAngle,
                                float rotaryEndAngle,
                                juce::Slider& slider
                                )
{
    using namespace juce;

    auto bounds = Rectangle<float>(x, y, width, height);
    g.setColour(Colour(97u, 18u, 167u));
    g.fillEllipse(bounds);
    g.setColour(Colour(255u, 154u, 1u));    
    g.drawEllipse(bounds, 1.0f);


    // If slider is castable to RotarySliderWithLabels
    if (auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider)) 
    {
        auto boundCenter = bounds.getCentre();

        Path p;
        Rectangle<float> rect;

        // 4 pixel thick clock hand, centered vertically with bottom being just above the center and top touching the circle
        rect.setLeft    (boundCenter.getX() - 2);
        rect.setRight   (boundCenter.getX() + 2);
        rect.setTop     (bounds.getY());
        rect.setBottom  (boundCenter.getY() - rswl->getTextHeight() * 1.5);

        // Draw and rotate the slider's clock hand.
        p.addRoundedRectangle(rect, 3.0f);
        auto sliderAng  = jmap(sliderPosProportional, 0.0f, 1.0f, rotaryStartAngle, rotaryEndAngle);
        p.applyTransform(AffineTransform().rotated(sliderAng, boundCenter.getX(), boundCenter.getY()));
        g.fillPath(p);

        g.setFont(rswl->getTextHeight());
        auto stringWidth = g.getCurrentFont().getStringWidth(rswl->getDisplayString());
        rect.setSize(stringWidth + 5, rswl->getTextHeight() + 3);

        // Center this rectangle to be at center of the bounds
        rect.setCentre(boundCenter);

        // White Text on Black Backgrounf
        g.setColour(Colours::black);
        g.fillRect(rect);
        g.setColour(Colours::white);
        g.drawFittedText(rswl->getDisplayString(), rect.toNearestInt(), juce::Justification::centred, 1);
    }     
}

juce::String RotarySliderWithLabels::getDisplayString() const
{
    juce::String labelText;

    // For checking if parameter is a choice type for slope 
    if (auto* choiceTypeParam = dynamic_cast<AudioParameterChoice*>(parameter))
        labelText = choiceTypeParam->getCurrentChoiceName();
    
    // For checking if parameter is a float type like gain or freq or quality 
    else if (auto* floatTypeParam = dynamic_cast<AudioParameterFloat*>(parameter))
    {
        // Only frequency param can exceed 1000 so direcly can use KHz as Unit
        if (getValue() >= 1000.0)
            labelText = juce::String(getValue() / 1000, 2) + " KHz";

        // General Case, append Unit to Value [adding space if unit is not empty string]
        else
            labelText = juce::String(getValue(), 0) + ((unit != "") ? " " : "") + unit;
        
    }
    // Should not reach here.
    else
        jassertfalse;

    return labelText;
}

void RotarySliderWithLabels::paint(juce::Graphics& g)
{
    using namespace juce;

    float startAngle= degreesToRadians(180.0f + 60.0f);
    float endAngle  = degreesToRadians(180.0f - 60.0f) + MathConstants<float>::twoPi;

    auto  range         = getRange();
    auto  sliderBounds  = getSliderBounds();

    g.setColour(Colours::yellowgreen);
    g.drawRect(sliderBounds);

    g.setColour(Colours::red);
    g.drawRect(getLocalBounds());

    getLookAndFeel().drawRotarySlider ( g, 
                                        sliderBounds.getX(), sliderBounds.getY(), 
                                        sliderBounds.getWidth(), sliderBounds.getHeight(), 
                                        jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                        startAngle,
                                        endAngle,
                                        *this
                                    );

}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    auto bounds =  getLocalBounds();

    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());

    size -= getTextHeight() * 2;

    juce::Rectangle<int> square;
    square.setSize(size, size);
    square.setCentre(bounds.getCentreX(), 0);

    // y = 0 => top of the component
    square.setY(getTextHeight() / 2);

    return square;
}


//==============================================================================

ResponseCurveComponent::ResponseCurveComponent(VstpluginAudioProcessor& p) 
    : processor(p)
{
    // Register as a listener
    auto& parameters = processor.getParameters();
    for (auto& param : parameters)
    {
        param->addListener(this);
    }

    // Initial call to display response curve when plugin started
    updateFilters();

    startTimerHz(60);
}

ResponseCurveComponent::~ResponseCurveComponent()
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

void ResponseCurveComponent::paint (Graphics& g)
{
    using namespace juce;

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (Colours::black);

    auto frequencyResponseArea = getLocalBounds();
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
        // On log scale, get frequency corresponding to pixel #i given 15Hz is i = 0 and 22KHz is i = width - 1
        double frequency = mapToLog10((double) i / (double) width, 15.0, 22000.0);

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
        // 4dB headroom => 24 + 4 = 28dB
        double pxval = jmap(input, -28.0, +28.0, minY, maxY);
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

void ResponseCurveComponent::parameterValueChanged(int parameterIndex, float newValue)
{
    paramsChanged.set(true);
}

void ResponseCurveComponent::timerCallback()
{
    if (paramsChanged.compareAndSetBool(false, true))
    {
        // Update mono chain of editor
        updateFilters();
        // Do a repaint
        repaint();
    }
}

void ResponseCurveComponent::updateFilters()
{
    const double sampleRate         = processor.getSampleRate();

    ChainSettings  chainSettings        = getChainSettings(processor.apvts);
    CoefficientPtr peakCoefficients     = makePeakFilter(chainSettings, sampleRate);
    CoefficientArr lowCutCoefficients   = makeLowCutFilter(chainSettings, sampleRate);
    CoefficientArr highCutCoefficients  = makeHighCutFilter(chainSettings, sampleRate);

    updateCoefficients  (monoChain.get<ChainPostitions::Peak>().coefficients, peakCoefficients); 
    updateCutFilter     (monoChain.get<ChainPostitions::LowCut>(),  lowCutCoefficients,  chainSettings.lowCutSlope);
    updateCutFilter     (monoChain.get<ChainPostitions::HighCut>(), highCutCoefficients, chainSettings.highCutSlope);
}

//==============================================================================

VstpluginAudioProcessorEditor::VstpluginAudioProcessorEditor (VstpluginAudioProcessor& p)
    : AudioProcessorEditor (&p), 
      processor (p),

      peakFrequencySlider(*processor.apvts.getParameter("Peak Freq"), "Hz"),
      peakGainSlider(*processor.apvts.getParameter("Peak Gain"), "dB"),
      peakQualitySlider(*processor.apvts.getParameter("Peak Q"), ""),
      lowCutFrequencySlider(*processor.apvts.getParameter("LowCut Freq"), "Hz"),
      highCutFrequencySlider(*processor.apvts.getParameter("HighCut Freq"), "Hz"),
      lowCutSlopeSlider(*processor.apvts.getParameter("LowCut Slope"), "dB/Oct"),
      highCutSlopeSlider(*processor.apvts.getParameter("HighCut Slope"), "dB/Oct"),

      responseCurveComponent          (processor),
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

    setSize (600, 400);
}

VstpluginAudioProcessorEditor::~VstpluginAudioProcessorEditor()
{

}

//==============================================================================

void VstpluginAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);

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

    responseCurveComponent.setBounds(frequencyResponseArea);
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
        &highCutSlopeSlider,
        &responseCurveComponent
    };
}