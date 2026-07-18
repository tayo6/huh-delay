#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HuhDelayAudioProcessor::HuhDelayAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                        .withOutput ("Output", juce::AudioChannelSet::mono(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

HuhDelayAudioProcessor::~HuhDelayAudioProcessor() = default;

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout HuhDelayAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Delay time as a note-division choice (matches the "1 / 16 D" readout
    // in the reference UI). Values map to musical divisions; "D" = dotted
    // could be layered on top later via a separate modifier if needed.
    juce::StringArray divisions { "1 / 32", "1 / 16 T", "1 / 16", "1 / 16 D",
                                   "1 / 8 T", "1 / 8", "1 / 8 D",
                                   "1 / 4 T", "1 / 4", "1 / 4 D",
                                   "1 / 2", "1 / 1" };

    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { ParamIDs::delayTime, 1 },
        "Delay Time",
        divisions,
        3)); // default "1 / 16 D", matching the reference screenshot

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { ParamIDs::syncEnabled, 1 },
        "Sync",
        true));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParamIDs::outputGain, 1 },
        "Output",
        juce::NormalisableRange<float> (-24.0f, 6.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { ParamIDs::bypass, 1 },
        "Bypass",
        false));

    return { params.begin(), params.end() };
}

//==============================================================================
void HuhDelayAudioProcessor::prepareToPlay (double /*sampleRate*/, int /*samplesPerBlock*/)
{
    // TODO: prepare delay line / DSP state once audio processing is implemented.
}

void HuhDelayAudioProcessor::releaseResources()
{
    // TODO: release delay line / DSP state once audio processing is implemented.
}

bool HuhDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Mono in, mono out only.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono())
        return false;

    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::mono())
        return false;

    return true;
}

void HuhDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    midiMessages.clear();

    // Passthrough only - intentionally no delay DSP yet (see class comment
    // in PluginProcessor.h). Downstream audio channels beyond the active
    // input count are cleared per standard JUCE plugin convention.
    for (auto ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear (ch, 0, buffer.getNumSamples());
}

//==============================================================================
juce::AudioProcessorEditor* HuhDelayAudioProcessor::createEditor()
{
    return new HuhDelayAudioProcessorEditor (*this);
}

//==============================================================================
void HuhDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState(); state.isValid())
    {
        if (auto xml = state.createXml())
            copyXmlToBinary (*xml, destData);
    }
}

void HuhDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HuhDelayAudioProcessor();
}
