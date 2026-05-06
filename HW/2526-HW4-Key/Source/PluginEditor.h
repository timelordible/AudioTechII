/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class _2526HW4KeyAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    _2526HW4KeyAudioProcessorEditor (_2526HW4KeyAudioProcessor&);
    ~_2526HW4KeyAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    _2526HW4KeyAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (_2526HW4KeyAudioProcessorEditor)
};
