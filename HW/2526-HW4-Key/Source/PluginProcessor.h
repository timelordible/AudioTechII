/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Delay.h"
#include "TestDelay.h"

//==============================================================================
/**
*/
class _2526HW4KeyAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    _2526HW4KeyAudioProcessor();
    ~_2526HW4KeyAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    
    juce::AudioProcessorValueTreeState apvts;
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (_2526HW4KeyAudioProcessor)
    
    Delay delay;

    double samplingRate;
    int bufferSize;

    const int maxDelaySec = 3;

    juce::AudioProcessorValueTreeState::ParameterLayout createParams();
    
    juce::SmoothedValue<float> smoothedDelay;
    juce::SmoothedValue<float> smoothedMix;
    juce::SmoothedValue<float> smoothedFeedback;
    
    float currDelay;
    float currMix;
    float currFeedback;
};
