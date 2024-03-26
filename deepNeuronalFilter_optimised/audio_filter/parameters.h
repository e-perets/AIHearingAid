#ifndef AUDIO_FILTER_PARAMETERS_H
#define AUDIO_FILTER_PARAMETERS_H

#include "dnf.h"

const double fs = 44100; // 48000

const float start_time_ignore = 0; // sec

const char* const audioPath = "audio/exp%d/long_drill_16bit_int.wav";

const Neuron::actMethod ACTIVATION = Neuron::Act_NONE;

int RingBufferPointer = 0;

// number of experiments
const int nExp = 1; // 2

// pre-filtering
const int filterorder = 2;
const double signalWithNoiseHighpassCutOff = 20; // Hz
const double noiserefHighpassCutOff = 20; // Hz

//creat circular buffers for plotting
const int bufferLength = 1000; // 1000

// Twice the number of taps.
const int nTapsDNF = 50; //int(fs / noiserefHighpassCutOff) * 3; // 300

// dnf learning rate
const double dnf_learning_rate = 0.05; // 1

// dnf number of layers
const int NLAYERS = 5; // 5

// LMS learning rates
const double lms_learning_rate = 0.05; // 0.05

const double signalWithNoise_gain = 0.5; // 1
const double noiseref_gain = 0.5; // 1
const double remover_gain = 1; // 1

// PLOTTING
#define WINDOW "Deep Neuronal Filter"
const int plotW = 1200; // 1200
const int plotH = 720; // 720


#endif //AUDIO_FILTER_PARAMETERS_H
