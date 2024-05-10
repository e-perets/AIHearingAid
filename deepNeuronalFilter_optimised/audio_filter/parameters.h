#ifndef AUDIO_FILTER_PARAMETERS_H
#define AUDIO_FILTER_PARAMETERS_H

#include "dnf.h"

const double fs = 44100; // 48000

const float start_time_ignore = 0; // sec

const char* audioPath = "audio/exp%d/S1m_N90deg1m_44100_32bit_drill.wav";

Neuron::actMethod ACTIVATION = Neuron::Act_NONE;

int RingBufferPointer = 0;

// number of experiments
const int nExp = 5; // 2

// pre-filtering
const int filterorder = 2;
const double signalWithNoiseHighpassCutOff = 20; // Hz
const double noiserefHighpassCutOff = 20; // Hz

//creat circular buffers for plotting
const int bufferLength = 1000; // 1000

// Twice the number of taps.
int nTapsDNF = 300; //int(fs / noiserefHighpassCutOff) * 3; // 300

// dnf learning rate
double dnf_learning_rate = 0.05; // 1

// dnf number of layers
int NLAYERS = 3; // 5

// LMS learning rates
double lms_learning_rate = 0.05; // 0.05

//how frequently error Backward propagation works
int propErrorBackwardPeriod = 1;

double lms_learning_rate_array[] = {0.01, 0.025, 0.05, 0.075, 0.1};
double dnf_learning_rate_array[] = {0.01, 0.025, 0.05, 0.075, 0.1};
int nTapsDNF_array[] = {100, 200, 300, 400, 500};
int NLAYERS_array[] = {1, 2, 3, 4, 5};
Neuron::actMethod ACTIVATION_array[] = {Neuron::Act_NONE, Neuron::Act_Sigmoid, Neuron::Act_Tanh, Neuron::Act_ReLU};
unsigned int propErrorBackwardPeriod_array[] = {1, 2, 5, 10, 20};
const char* audioPath_array[] = {"audio/exp%d/S1m_N90deg1m_44100_32bit_drill.wav", "audio/exp%d/S1m_N60deg1m_44100_32bit_drill.wav", "audio/exp%d/S1m_N30deg1m_44100_32bit_drill.wav", "audio/exp%d/S1m_Behind1m_44100_32bit_drill.wav"};


const double signalWithNoise_gain = 0.5; // 1
const double noiseref_gain = 0.5; // 1
const double remover_gain = 1; // 1

// PLOTTING
#define WINDOW "Deep Neuronal Filter"
const int plotW = 1200; // 1200
const int plotH = 720; // 720


#endif //AUDIO_FILTER_PARAMETERS_H
