/**
 * @file    alsa_cpp_wrapper_test.cpp
 * @author  Adam Englebright
 * @date    01.03.2024
 * @brief   Small test program showing the funtionality of the alsa_cpp_wrapper
 * library
 */
#include <Fir1.h>
#include <Iir.h>
#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <thread>
#include "../alsa_cpp_wrapper.hpp"

const int nTapsDNF = 200;
double dnf_learning_rate = 0.05;





int main() {

  union{
  	int32_t filterOutput;
	unsigned char alsaOutput[sizeof(filterOutput)];
  }bytes;       	

  Fir1 lms_filter(nTapsDNF);

  lms_filter.setLearningRate(dnf_learning_rate);
  

  /* Create our object, with appropriate ALSA PCM identifiers,
   * sample rate, buffer data organization, and period size in frames.
   * The ALSA PCM identifiers will be "hw:0,0" on the pi, but will
   * likely be different on your own hardware. Use "aplay -l" and
   * "arecord -l" on the terminal to find the appropriate id
   * for playback and capture. The first number in the id is the
   * card number, and the second is the device number. */
  Alsa audio("hw:2,0", "hw:2,0", 44100, SND_PCM_ACCESS_RW_INTERLEAVED, 256);

  // We can get various bits of data from the "audio" object:
  unsigned int rate = audio.getRate();
  snd_pcm_uframes_t period_size = audio.getPeriodSize();

  //audio is 4 bytes interleaved, first 2 are for right channel and last 2 are left channel

  uint8_t right_buffer[2]; //array storing 2 bytes for right channel
  uint8_t left_buffer[2];  //array storing 2 bytes for left channel
  uint8_t output_buffer[4];//array for outputting

  // Most importantlly, we can get a pointer to the buffer storing each period of audio captured
  char *buffer = audio.getBufPtr();//buffer contains bytes
  size_t buffer_size = audio.getBufSize(); // Need to know the size of the buffer in bytes
  


  audio.start(); // Start the capture and playback devices
  //std::thread thread(&Alsa::captureLoop, &audio);
  while (true) {
  	//std::cout <<"before capturePeriod"<<std::endl;	

	audio.capturePeriod(); // Capture a period

	for(size_t i = 0; i < audio.getPeriodSize()/4; i++){
		double input_signal = (buffer[i*4]<<8|buffer[i*4+1])/pow(2,15);
		double ref_noise = (buffer[i*4+2]<<8|buffer[i*4+3])/pow(2,15);
		double canceller = lms_filter.filter(ref_noise);
		double output_signal = input_signal - canceller;
		lms_filter.lms_update(output_signal);
		int32_t audioOutput = output_signal*pow(2,15);
	//	bytes.filterOutput = audioOutput;
	//	for(int k = 0; k < sizeof(int32_t); k++){
	//		continue;
			//std::cout<<(int) bytes.alsaOutput[k];
			//std::cout<<", ";
			//buffer[i*4 + k] = bytes.alsaOutput[k];
	//	}
		//std::cout<<""<<std::endl;

		//	std::cout<<(int) buffer[i];
		//	std::cout<<", ";
	}


    
    audio.playbackPeriod(); // Playback the captured period
  }
}
