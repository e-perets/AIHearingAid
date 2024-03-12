#include "AudioFile/AudioFile.h"
#include <iostream>

int main(){
AudioFile<float> e;

e.load ("/Users/eperets/dst5/AIHearingAid/Bernd_Repo/deepNeuronalFilter/audio_filter/audio/exp1/signal_noise.wav");

e.save ("/Users/eperets/dst5/AIHearingAid/Bernd_Repo/deepNeuronalFilter/audio_filter/audio/exp1/kek.wav");
}

