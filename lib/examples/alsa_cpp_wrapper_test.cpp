/**
 * @file    alsa_cpp_wrapper_test.cpp
 * @author  Adam Englebright
 * @date    01.03.2024
 * @brief   Small test program showing the funtionality of the alsa_cpp_wrapper
 * library
 */

#include "../alsa_cpp_wrapper.hpp"

int main() {
  /* Create our object, with appropriate ALSA PCM identifiers,
   * sample rate, buffer data organization, and period size in frames.
   * The ALSA PCM identifiers will be "hw:0,0" on the pi, but will
   * likely be different on your own hardware. Use "aplay -l" and
   * "arecord -l" on the terminal to find the appropriate id
   * for playback and capture. The first number in the id is the
   * card number, and the second is the device number. */
  Alsa audio("hw:0,0", "hw:0,0", 44100, SND_PCM_ACCESS_RW_INTERLEAVED, 32);

  // We can get various bits of data from the "audio" object:
  unsigned int rate = audio.getRate();
  snd_pcm_uframes_t period_size = audio.getPeriodSize();

  // Most importantlly, we can get a pointer to the buffer storing each period of audio captured
  char *buffer = audio.getBufPtr();
  size_t buffer_size = audio.getBufSize(); // Need to know the size of the buffer in bytes

  audio.start(); // Start the capture and playback devices
  while (true) {
    audio.capturePeriod(); // Capture a period
    
    /*
     * DO FILTERING HERE!!!!
     */
    
    audio.playbackPeriod(); // Playback the captured period
  }
}
