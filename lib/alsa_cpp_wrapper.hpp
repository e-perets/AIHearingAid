/**
 * @file    alsa_cpp_wrapper.hpp
 * @author  Adam Englebright
 * @date    29.02.2024
 * @brief   This file contains declarations for a simple C++ wrapper library
 * around ALSA, for audio passthrough.
 */

#ifndef ALSA_CPP_WRAPPER_H
#define ALSA_CPP_WRAPPER_H

#include <string>
extern "C"{
  #include <alsa/asoundlib.h>
}

/**
 * @brief  Class to wrap up ALSA initialisation, providing methods for audio
 * capture and playback
 */
class Alsa {
public:
  /**
   * @brief  Constructor initialising attributes
   * @param  _pdevice     ALSA device name for audio playback
   * @param  _cdevice     ALSA device name for audio capture
   * @param  _rate        Audio sample rate for playback and capture
   * @param  _buffer_mode Organization of data in buffers (e.g. interleaved)
   * @param  _period_size Number of frames per period read or written
   */
  Alsa(std::string _pdevice, std::string _cdevice, unsigned int _rate, snd_pcm_access_t _buffer_mode, snd_pcm_uframes_t _period_size);

  /**
   * @brief  Method writing out one period of frames for playback
   * @param  None
   * @retval None
   */
  void playbackPeriod(void);

  /**
   * @brief  Method reading in one period of frames for capture
   * @param  None
   * @retval None
   */
  void capturePeriod(void);

  /**
   * @brief  Method to start the linked capture and playback PCM devices
   * @param  None
   * @retval None
   */
  void start(void);

  /**
   * @brief  Method returning pointer to internal application buffer containing a period of frames
   * @param  None
   * @retval char*
   */
  char *getBufPtr(void);

  /**
   * @brief  Method returning the length of the internal application buffer in bytes
   * @param  None
   * @retval size_t
   */
  size_t getBufSize(void);

  /**
   * @brief  Method returning the sample rate actually obtained
   * @param  None
   * @retval unsigned int
   */
  unsigned int getRate(void);

  /**
   * @brief  Method returning the period size in frames actually obtained
   * @param  None
   * @retval snd_pcm_uframes_t
   */
  snd_pcm_uframes_t getPeriodSize(void);

  /**
   * @brief Method for starting a capture loop (intended for threads) 
   * @param None
   * @retval None
   */
  void captureLoop();

  /**
   * @brief  Destructor to clean up
   */
  ~Alsa(void);

private:

  /** Boolean for starting and stopping capture loop */
  bool running;

  /** Application buffer containing one period of frames */
  char *buffer = NULL;

  /** ALSA playback device name */
  std::string pdevice;

  /** ALSA capture device name */
  std::string cdevice;

  /** Audio sample rate for playback and capture */
  const unsigned int rate;

  /** Audio sample rate for playback and capture actually obtained */
  unsigned int rate_actual;

  /** Buffer data mode (e.g. interlieved or non-interlieved) */
  snd_pcm_access_t buffer_mode;

  /** Number of frames per period */
  const snd_pcm_uframes_t period_size;

  /** Number of frames per period actually obtained */
  snd_pcm_uframes_t period_size_actual;

  /** Number of bytes per period actually obtained */
  size_t period_size_actual_bytes;

  /** Number of frames per hardware buffer */
  snd_pcm_uframes_t buffer_size;

  /** snd_output_t pointer for ALSA standard output functions */
  snd_output_t *output = NULL;

  /** ALSA PCM device handle for playback */
  snd_pcm_t *phandle = NULL;

  /** ALSA PCM device handle for capture */
  snd_pcm_t *chandle = NULL;

  /** ALSA PCM hardware parameters for playback */
  snd_pcm_hw_params_t *p_params = NULL;

  /** ALSA PCM hardware parameters for capture */
  snd_pcm_hw_params_t *c_params = NULL;

  /** ALSA PCM software parameters for playback */
  snd_pcm_sw_params_t *p_swparams = NULL;

  /** ALSA PCM software parameters for capture */
  snd_pcm_sw_params_t *c_swparams = NULL;
};

#endif
