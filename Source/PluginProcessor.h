/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/**  ENUMS **/
enum Slope
{
    Slope12, 
    Slope24, 
    Slope36, 
    Slope48
};

// Enum for easy access to Chain Elements of a Mono Chain
enum ChainPostitions
{
    LowCut,
    Peak,
    HighCut
};


/**  TYPE ALIASES **/
using Filter    = juce::dsp::IIR::Filter<float>;
using CutFilter = juce::dsp::ProcessorChain <Filter, Filter, Filter, Filter>;

using MonoChain = juce::dsp::ProcessorChain <CutFilter, Filter, CutFilter>;
using CoefficientPtr = Filter::CoefficientsPtr;
using CoefficientArr = juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<float>>;

struct ChainSettings
{
    float peakFreq      { 0.0f };
    float peakGain      { 0.0f };
    float peakQ         { 0.0f };
    
    float lowCutFreq    { 0.0f };
    float highCutFreq   { 0.0f };

    Slope lowCutSlope     { Slope::Slope12 };
    Slope highCutSlope    { Slope::Slope12 };
};

ChainSettings getChainSettings (const juce::AudioProcessorValueTreeState& apvts);

void updateCoefficients (CoefficientPtr& old, const CoefficientPtr& replacement);

CoefficientPtr makePeakFilter   (const ChainSettings& settings, const double sampleRate);
CoefficientArr makeLowCutFilter (const ChainSettings& settings, const double sampleRate);
CoefficientArr makeHighCutFilter(const ChainSettings& settings, const double sampleRate);

template<typename FilterChainType, typename CoefficientType>
void updateCutFilter (FilterChainType& lowCut, const CoefficientType& cutCoefficients, const Slope slope);
  

class VstpluginAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    VstpluginAudioProcessor();
    ~VstpluginAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // Since the function doesnt depend on any member variables.
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout() };

private:
    // To accomodate for stereo we need both left and right chains
    MonoChain leftChain;
    MonoChain rightChain;


    void updateFilters      ();
    void updateLowCutFilter (const ChainSettings& settings);
    void updateHighCutFilter(const ChainSettings& settings);
    void updatePeakFilter   (const ChainSettings& settings);

  

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VstpluginAudioProcessor)
};
