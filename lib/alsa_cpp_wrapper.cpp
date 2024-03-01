/**
 * @file    alsa_cpp_wrapper.cpp
 * @author  Adam Englebright
 * @date    29.02.2024
 * @brief   This file contains implementations for a simple C++ wrapper library
 * around ALSA, for audio passthrough.
 */

#include "alsa_cpp_wrapper.hpp"
#include <alsa/error.h>
#include <alsa/pcm.h>
#include <asm-generic/errno-base.h>
#include <iostream>

Alsa::Alsa(std::string _pdevice, std::string _cdevice, unsigned int _rate,
           snd_pcm_access_t _buffer_mode, snd_pcm_uframes_t _period_size)
  :
  pdevice {_pdevice},
  cdevice {_cdevice},
  rate {_rate},
  buffer_mode {_buffer_mode},
  period_size {_period_size}
{
  int err;
  int dir;
  constexpr snd_pcm_uframes_t start_threshold = 0x1;
  constexpr snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

  // Setup ALSA standard output
  if ((err = snd_output_stdio_attach(&output, stdout, 0)) < 0) {
      std::cout << "Output failed: " << snd_strerror(err) << std::endl;
    }

  // Open PCM device for capture
  if ((err = snd_pcm_open(&chandle, cdevice.c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
      std::cout << "Unable to open pcm device: " << snd_strerror(err) << std::endl;
    }

  // Open PCM device for playback
  if ((err = snd_pcm_open(&phandle, pdevice.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
      std::cout << "Unable to open pcm device: " << snd_strerror(err) << std::endl;
    }

  // Allocate hardware parameter objects.
  snd_pcm_hw_params_alloca(&c_params);
  snd_pcm_hw_params_alloca(&p_params);

  // Fill them in with default values.
  snd_pcm_hw_params_any(chandle, c_params);
  snd_pcm_hw_params_any(phandle, p_params);

  // Now set the desired hardware parameters.
  // Buffer data mode
  snd_pcm_hw_params_set_access(chandle, c_params, buffer_mode);
  snd_pcm_hw_params_set_access(phandle, p_params, buffer_mode);

  // Set sample format
  snd_pcm_hw_params_set_format(chandle, c_params, format);
  snd_pcm_hw_params_set_format(phandle, p_params, format);

  // Two channels (stereo) for capture and playback
  snd_pcm_hw_params_set_channels(chandle, c_params, 2);
  snd_pcm_hw_params_set_channels(phandle, p_params, 2);

  // Set sample rate
  snd_pcm_hw_params_set_rate_near(chandle, c_params, &rate, &dir);
  snd_pcm_hw_params_set_rate_near(phandle, p_params, &rate, &dir);

  // Set period size in frames.
  snd_pcm_hw_params_set_period_size_near(chandle, c_params, &period_size, &dir);
  snd_pcm_hw_params_set_period_size_near(phandle, p_params, &period_size, &dir);

  // Set buffer size to twice the period size
  buffer_size = 2 * period_size;
  snd_pcm_hw_params_set_buffer_size_near(chandle, c_params, &buffer_size);
  snd_pcm_hw_params_set_buffer_size_near(phandle, p_params, &buffer_size);

  // Write the hardware parameters to the driver
  if ((err = snd_pcm_hw_params(chandle, c_params)) < 0) {
    std::cout << "Unable to set hw parameters for capture: " << snd_strerror(err) << std::endl;
  }
  if ((err = snd_pcm_hw_params(phandle, p_params)) < 0) {
    std::cout << "Unable to set hw parameters for playback: " << snd_strerror(err) << std::endl;
  }

  // Allocate software parameter objects
  snd_pcm_sw_params_alloca(&c_swparams);
  snd_pcm_sw_params_alloca(&p_swparams);

  // Get their current values
  if ((err = snd_pcm_sw_params_current(chandle, c_swparams)) < 0) {
    std::cout << "Unable to determine current capture swparams: " << snd_strerror(err) << std::endl;
  }
  if ((err = snd_pcm_sw_params_current(phandle, p_swparams)) < 0) {
    std::cout << "Unable to determine current playback swparams: " << snd_strerror(err) << std::endl;
  }

  // Set the start threshold
  if ((err = snd_pcm_sw_params_set_start_threshold(chandle, c_swparams, start_threshold)) < 0) {
    std::cout << "Unable to set capture start threshold mode: " << snd_strerror(err) << std::endl;
  }
  if ((err = snd_pcm_sw_params_set_start_threshold(phandle, p_swparams, start_threshold)) < 0) {
    std::cout << "Unable to set playback start threshold mode: " << snd_strerror(err) << std::endl;
  }

  // Set avail min (and check actual period size obtained)
  snd_pcm_hw_params_get_period_size(c_params, &period_size_actual, &dir);
  if (period_size_actual != period_size) {
    std::cout << "Warning: period size selected was not achieved for capture: ";
    std::cout << "selected " << period_size << ", but got " << period_size_actual << std::endl;
  }
  if ((err = snd_pcm_sw_params_set_avail_min(chandle, c_swparams, period_size_actual)) < 0) {
    std::cout << "Unable to set avail min for capture: " << snd_strerror(err) << std::endl;
  }

  snd_pcm_hw_params_get_period_size(p_params, &period_size_actual, &dir);
  if (period_size_actual != period_size) {
    std::cout << "Warning: period size selected was not achieved for playback: ";
    std::cout << "selected " << period_size << ", but got " << period_size_actual << std::endl;
  }
  if ((err = snd_pcm_sw_params_set_avail_min(phandle, p_swparams, period_size_actual)) < 0) {
    std::cout << "Unable to set avail min for playback: " << snd_strerror(err) << std::endl;
  }

  // Write the software parameters to the driver
  if ((err = snd_pcm_sw_params(chandle, c_swparams)) < 0) {
    std::cout << "Unable to set sw params for capture: " << snd_strerror(err) << std::endl;
  }
  if ((err = snd_pcm_sw_params(phandle, p_swparams)) < 0) {
    std::cout << "Unable to set sw params for playback: " << snd_strerror(err) << std::endl;
  }

  // Link PCM devices
  if ((err = snd_pcm_link(chandle, phandle)) < 0) {
    std::cout << "Streams link error: " << snd_strerror(err) << std::endl;
  }

  // Allocate a buffer large enough (in bytes) to hold one period
  // 2 channels * 2 bytes/sample (ASSUMING SND_PCM_FORMAT_S16_LE)
  period_size_actual_bytes = 4 * period_size_actual;
  buffer = (char*) malloc(period_size_actual_bytes);

  // Prefill playback buffer to avoid broken pipe when starting
  if (snd_pcm_format_set_silence(format, buffer, period_size_actual * 2) < 0) { // 2 channels, hence "* 2"
    std::cout << "Silence error" << std::endl;
  }
  if ((err = snd_pcm_writei(phandle, buffer, period_size_actual)) < 0) {
    std::cout << "Write error: " << snd_strerror(err) << std::endl;
  }
  if ((err = snd_pcm_writei(phandle, buffer, period_size_actual)) < 0) {
    std::cout << "Write error 2: " << snd_strerror(err) << std::endl;
  }

  // Dump PCM info
  snd_pcm_dump(chandle, output);
  snd_pcm_dump(phandle, output);
  fflush(stdout);
}

void Alsa::capturePeriod(void) {
  // Capture a period of frames
  int err;
  if ((err = snd_pcm_readi(chandle, buffer, period_size_actual)) == -EPIPE) {
    std::cout << "Overrun occurred" << std::endl;
    snd_pcm_prepare(chandle);
  } else if (err < 0) {
    std::cout << "Error from readi: " << snd_strerror(err) << std::endl;
  } else if (err != (int)period_size_actual) {
    std::cout << "Short read, read " << err << " frames" << std::endl;
  }
}

void Alsa::playbackPeriod(void) {
  // Playback a period of frames
  int err;
  if ((err = snd_pcm_writei(phandle, buffer, period_size_actual)) == -EPIPE) {
    std::cout << "Underrun occurred" << std::endl;
    snd_pcm_prepare(phandle);
  } else if (err < 0) {
    std::cout << "Error from writei: " << snd_strerror(err) << std::endl;
  } else if (err != (int)period_size_actual) {
    std::cout << "Short write, wrote " << err << " frames" << std::endl;
  }
}

char *Alsa::getBufPtr(void) { return buffer; }

size_t Alsa::getBufSize(void) { return period_size_actual_bytes; }

Alsa::~Alsa(void) {
  snd_pcm_unlink(chandle);
  snd_pcm_hw_free(chandle);
  snd_pcm_hw_free(phandle);
  snd_pcm_drop(chandle);
  snd_pcm_close(chandle);
  snd_pcm_drain(phandle);
  snd_pcm_close(phandle);
  snd_output_close(output);
  free(buffer);
}
