/*
  ==============================================================================

    TestDelay.h
    Created: 4 May 2026 2:18:49pm
    Author:  Jocelyn

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Delay.h"

class DelayTests : public juce::UnitTest
{
    public:
    DelayTests() : juce::UnitTest("Delay Tests") {}
    
    void runTest() override
    {
        testImpulseResponse();
        testWetZero();
        testDryZero();
        testZeroDelay();
        testSilence();
        testMultiChannel();
        testFeedback();
        testFractionalDelay();
    }
    
    private:
    
    void testImpulseResponse()
    {
        beginTest("Impulse appears at correct delay time");
        
        Delay delay;
        
        double sampleRate = 48000;
        int maxDelaySamples = 48000; // 1 second
        int numChannels = 1;
        
        delay.prepare(sampleRate, maxDelaySamples, numChannels);
        delay.setDelayTime(0.1f); // 100 ms
        delay.setWetMix(1.0f);
        delay.setFeedbackAmt(0.0f);
        
        int delaySamples = int(0.1f * sampleRate);
        
        float output = 0.0f;
        float input = 1.0f;
        
        for (int i = 0; i < delaySamples + 10; ++i)
        {
            if (i > 0) {
                input = 0;
            }
            output = delay.processSample(input, 0);
            
            if (i < delaySamples)
            {
                expectWithinAbsoluteError(output, 0.0f, 1e-6f);
            }

            if (i == delaySamples)
            {
                expectWithinAbsoluteError(output, 1.0f, 1e-3f);
            }
        }
        
//        // At delay time, we expect the impulse
//        expectWithinAbsoluteError(output, 1.0f, 1e-3f);
    }
    
    void testWetZero()
    {
        beginTest("All dry signal has no effect");
        
        Delay delay;
        
        delay.prepare(48000, 48000, 1);
        delay.setDelayTime(0.01f);
        delay.setFeedbackAmt(0.0f);
        
        // Dry only
        delay.setWetMix(0.0f);
        float outDry = delay.processSample(1.0f, 0);
        expectWithinAbsoluteError(outDry, 1.0f, 1e-6f);
    }
    
    void testDryZero()
    {
        beginTest("All wet signal has output of 0 at start");
        
        Delay delay;
        
        delay.prepare(48000, 48000, 1);
        delay.setDelayTime(0.01f);
        delay.setFeedbackAmt(0.0f);
        
        // Wet only (first sample should be 0)
        delay.setWetMix(1.0f);
        float outWet = delay.processSample(1.0f, 0);
        expectWithinAbsoluteError(outWet, 0.0f, 1e-6f);
    }
    
    void testZeroDelay()
    {
        beginTest("Delay of time 0 has no effect");
        
        Delay delay;
        
        delay.prepare(48000, 48000, 1);
        delay.setDelayTime(0.0f);
        delay.setWetMix(0.5f);
        delay.setFeedbackAmt(0.0f);
        
        float out = delay.processSample(1.0f, 0);
        
        expectWithinAbsoluteError(out, 0.5f, 1e-3f);
    }
    
    
    void testSilence()
    {
        beginTest("Silence remains silence");

        Delay delay;
        delay.prepare(48000, 48000, 1);

        delay.setDelayTime(0.1f);
        delay.setWetMix(1.0f);
        delay.setFeedbackAmt(0.9f);

        for (int i = 0; i < 1000; ++i)
        {
            float out = delay.processSample(0.0f, 0);
            expectWithinAbsoluteError(out, 0.0f, 1e-6f);
        }
    }
    
    void testMultiChannel()
    {
        beginTest("Channels are independent");

        Delay delay;
        delay.prepare(48000, 48000, 2);

        delay.setDelayTime(0.01f);
        delay.setWetMix(1.0f);

        float outL = 0.0f;
        float outR = 0.0f;

        for (int i = 0; i < 1000; ++i)
        {
            float inputL = (i == 0) ? 1.0f : 0.0f;
            float inputR = 0.0f;

            outL = delay.processSample(inputL, 0);
            outR = delay.processSample(inputR, 1);
        }

        expect(outR == 0.0f); // right channel should stay silent
    }
    
    void testFeedback()
    {
        beginTest("Feedback produces correct exponential decay");

        Delay delay;
        double sampleRate = 48000;
        delay.prepare(sampleRate, 48000, 1);

        float delayTime = 0.01f;
        float feedback = 0.5f;

        delay.setDelayTime(delayTime);
        delay.setWetMix(1.0f);
        delay.setFeedbackAmt(feedback);

        int delaySamples = (int)(delayTime * (float)sampleRate);
        float expectedAmplitude = 1.0f;

        for (int i = 0; i < delaySamples * 5; ++i)
        {
            float input = (i == 0) ? 1.0f : 0.0f;
            float out = delay.processSample(input, 0);

            for (int k = 0; k < 4; ++k)
            {
                if (i == delaySamples * (k + 1))
                {
                    expectWithinAbsoluteError(out, expectedAmplitude, 1e-2f);
                    expectedAmplitude *= feedback;
                }
            }
        }
    }
    
    void testFractionalDelay()
    {
        beginTest("Fractional delay spreads impulse across adjacent samples");

        Delay delay;
        double sampleRate = 4800;
        delay.prepare(sampleRate, 4800, 1);

        // 100.5ms — intentionally fractional
        float delayTime = 0.1005f;
        delay.setDelayTime(delayTime);
        delay.setWetMix(1.0f);
        delay.setFeedbackAmt(0.0f);

        float delaySamplesF = delayTime * (float)sampleRate; // 482.4
        int floor_sample = (int)delaySamplesF;               // 482
        int ceil_sample  = floor_sample + 1;                 // 483
        float frac = delaySamplesF - floor_sample;           // .6

        float peakFloor = 0.0f;
        float peakCeil  = 0.0f;

        for (int i = 0; i < ceil_sample + 2; ++i)
        {
            float input = (i == 0) ? 1.0f : 0.0f;
            float out = delay.processSample(input, 0);

            if (i == floor_sample)
            {
                peakFloor = out;
            }
            if (i == ceil_sample)
            {
                peakCeil  = out;
            }
        }

        // energy should be split between the two adjacent samples
        expectWithinAbsoluteError(peakFloor, 1.0f - frac, 1e-2f);
        expectWithinAbsoluteError(peakCeil, frac, 1e-2f);
        
        // total energy should sum to 1
        expectWithinAbsoluteError(peakFloor + peakCeil, 1.0f, 1e-3f);
    }

};

static DelayTests delayTests;
