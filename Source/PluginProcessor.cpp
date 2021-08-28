/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VstpluginAudioProcessor::VstpluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

VstpluginAudioProcessor::~VstpluginAudioProcessor()
{
}

//==============================================================================
const String VstpluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VstpluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VstpluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VstpluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VstpluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VstpluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VstpluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VstpluginAudioProcessor::setCurrentProgram (int index)
{
}

const String VstpluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void VstpluginAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void VstpluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void VstpluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VstpluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void VstpluginAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool VstpluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* VstpluginAudioProcessor::createEditor()
{
    return new GenericAudioProcessorEditor (*this);
}

//==============================================================================
void VstpluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void VstpluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout VstpluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Adding LowCut Frequency Slider with appropriate range and defaults
    layout.add(std::make_unique<juce::AudioParameterFloat>
        (
            "LowCut Freq",
            "LowCut Freq",
            juce::NormalisableRange(20.0f, 20000.0f, 1.0f, 1.0f),
            20.0f
        )
    );

    // Adding HighCut Frequency Slider with appropriate range and defaults
    layout.add(std::make_unique<juce::AudioParameterFloat>
        (
            "HighCut Freq",
            "HighCut Freq",
            juce::NormalisableRange(20.0f, 20000.0f, 1.0f, 1.0f),
            20000.0f
        )
    );

    // Adding Peak Frequency Slider with appropriate range and defaults
    layout.add(std::make_unique<juce::AudioParameterFloat>
        (
            "Peak Freq",
            "Peak Freq",
            juce::NormalisableRange(20.0f, 20000.0f, 1.0f, 1.0f),
            1000.0f
        )
    );

    // Adding Peak Gain Slider with appropriate range and defaults
    layout.add(std::make_unique<juce::AudioParameterFloat>
        (
            "Peak Gain",
            "Peak Gain",
            juce::NormalisableRange(-24.0f, +24.0f, 0.5f, 1.0f),
            0.0f
        )
    );

    // Adding Peak Quality Slider with appropriate range and defaults
    layout.add(std::make_unique<juce::AudioParameterFloat>
        (
            "Peak Q",
            "Peak Q",
            juce::NormalisableRange(0.1f, 10.0f, 0.05f, 1.0f),
            1.0f
        )
    );

    juce::StringArray slopeChoices {"12 db/oct", "24 db/oct", "36 db/oct", "48 db/oct"};

    // Adding Low Cut Slope Gain dropdown with appropriate range and defaults
    layout.add(std::make_unique<juce::AudioParameterChoice>
        (
            "LowCut Slope",
            "LowCut Slope",
            slopeChoices,
            0
        )
    );    
    
    // Adding High Cut Slope dropdown with appropriate range and defaults
    layout.add(std::make_unique<juce::AudioParameterChoice>
        (
            "HighCut Slope",
            "HighCut Slope",
            slopeChoices,
            0
        )
    );

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VstpluginAudioProcessor();
}
