/*

Program combining record and playback,
for directly reading from the mic and writing straight to the speakers.

*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include <stdio.h>
#include <string.h>

int main() {
  long loops;
  int rc;
  int size;
  snd_pcm_t *handle_r;
  snd_pcm_t *handle_w;
  snd_pcm_hw_params_t *params_r;
  snd_pcm_hw_params_t *params_w;
  snd_pcm_sw_params_t *swparams_r;
  snd_pcm_sw_params_t *swparams_w;
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;
  snd_pcm_uframes_t buffer_frames;
  char *buffer;
  snd_output_t *output = NULL;
  snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

  rc = snd_output_stdio_attach(&output, stdout, 0);
  if (rc < 0) {
    fprintf(stderr, "Output failed: %s\n", snd_strerror(rc));
    return 0;
  }

  /* Open PCM device for recording (capture). */
  rc = snd_pcm_open(&handle_r, "hw:0,0",
                    SND_PCM_STREAM_CAPTURE, 0);
  if (rc < 0) {
    fprintf(stderr,
            "unable to open pcm device: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  /* Open PCM device for playback. */
  rc = snd_pcm_open(&handle_w, "hw:0,0",
                    SND_PCM_STREAM_PLAYBACK, 0);
  if (rc < 0) {
    fprintf(stderr,
            "unable to open pcm device: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  /* Allocate hardware parameter objects. */
  snd_pcm_hw_params_alloca(&params_r);
  snd_pcm_hw_params_alloca(&params_w);

  /* Fill them in with default values. */
  snd_pcm_hw_params_any(handle_r, params_r);
  snd_pcm_hw_params_any(handle_w, params_w);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(handle_r, params_r,
                      SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_access(handle_w, params_w,
                      SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(handle_r, params_r,
                              SND_PCM_FORMAT_S16_LE);
  snd_pcm_hw_params_set_format(handle_w, params_w,
                              SND_PCM_FORMAT_S16_LE);

  /* Two channels (stereo) */
  snd_pcm_hw_params_set_channels(handle_r, params_r, 2);
  snd_pcm_hw_params_set_channels(handle_w, params_w, 2);

  /* 44100 bits/second sampling rate (CD quality) */
  val = 44100;
  snd_pcm_hw_params_set_rate_near(handle_r, params_r,
                                  &val, &dir);
  snd_pcm_hw_params_set_rate_near(handle_w, params_w,
                                  &val, &dir);

  /* Set period size in frames. */
  frames = 32;
  snd_pcm_hw_params_set_period_size_near(handle_r,
                              params_r, &frames, &dir);
  snd_pcm_hw_params_set_period_size_near(handle_w,
                              params_w, &frames, &dir);

  /* Set buffer size */
  buffer_frames = 2 * frames;
  snd_pcm_hw_params_set_buffer_size_near(handle_r, params_r, &buffer_frames);
  snd_pcm_hw_params_set_buffer_size_near(handle_w, params_w, &buffer_frames);
  
  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle_r, params_r);
  if (rc < 0) {
    fprintf(stderr,
            "unable to set hw parameters for capture: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  rc = snd_pcm_hw_params(handle_w, params_w);
  if (rc < 0) {
    fprintf(stderr,
            "unable to set hw parameters for playback: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  /* Set the software parameters */
  snd_pcm_sw_params_alloca(&swparams_r);
  snd_pcm_sw_params_alloca(&swparams_w);
  
  rc = snd_pcm_sw_params_current(handle_r, swparams_r);
  if (rc < 0) {
    printf("Unable to determine current swparams: %s\n", snd_strerror(rc));
    return rc;
  }
  rc = snd_pcm_sw_params_current(handle_w, swparams_w);
  if (rc < 0) {
    printf("Unable to determine current swparams: %s\n", snd_strerror(rc));
    return rc;
  }
  
  rc = snd_pcm_sw_params_set_start_threshold(handle_r, swparams_r, 0x7fffffff);
  //rc = snd_pcm_sw_params_set_start_threshold(handle_r, swparams_r, 0x1);
  if (rc < 0) {
    printf("Unable to set start threshold mode: %s\n", snd_strerror(rc));
    return rc;
  }
  rc = snd_pcm_sw_params_set_start_threshold(handle_w, swparams_w, 0x7fffffff);
  //rc = snd_pcm_sw_params_set_start_threshold(handle_w, swparams_w, 0x1);
  if (rc < 0) {
    printf("Unable to set start threshold mode: %s\n", snd_strerror(rc));
    return rc;
  }

  snd_pcm_uframes_t period_size;
  snd_pcm_hw_params_get_period_size(params_r, &period_size, NULL);
  rc = snd_pcm_sw_params_set_avail_min(handle_r, swparams_r, period_size);
  if (rc < 0) {
    printf("Unable to set avail min: %s\n", snd_strerror(rc));
    return rc;
  }
  snd_pcm_hw_params_get_period_size(params_w, &period_size, NULL);
  rc = snd_pcm_sw_params_set_avail_min(handle_w, swparams_w, period_size);
  if (rc < 0) {
    printf("Unable to set avail min: %s\n", snd_strerror(rc));
    return rc;
  }

  rc = snd_pcm_sw_params(handle_r, swparams_r);
  if (rc < 0) {
    printf("Unable to set sw params: %s\n", snd_strerror(rc));
    return rc;
  }
  rc = snd_pcm_sw_params(handle_w, swparams_w);
  if (rc < 0) {
    printf("Unable to set sw params: %s\n", snd_strerror(rc));
    return rc;
  }

  /* Link PCM devices */
  rc = snd_pcm_link(handle_r, handle_w);
  if (rc < 0) {
    fprintf(stderr, "Streams link error: %s\n", snd_strerror(rc));
    exit(0);
  }

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params_r, &frames,
                                    &dir);
  size = frames * 4; /* 2 bytes/sample, 2 channels */
  buffer = (char *) malloc(size);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(params_r,
                                    &val, &dir);

  /* 5 seconds in microseconds divided by
   * period time */
  loops = 5000000 / val;

  /* Dump PCM info */
  snd_pcm_dump(handle_r, output);
  snd_pcm_dump(handle_w, output);
  fflush(stdout);

  /* Prepare capture and playback devices */
  //snd_pcm_prepare(handle_r);
  //snd_pcm_prepare(handle_w);

  /* Prefill playback buffer to avoid broken pipe when sarting */
  if (snd_pcm_format_set_silence(format, buffer, frames * 2) < 0) { // 2 channels, hence "* 2"
    fprintf(stderr, "silence error\n");
  }
  if ((rc = snd_pcm_writei(handle_w, buffer, frames)) < 0) {
    fprintf(stderr, "write error: %s\n", snd_strerror(rc));
  }
  if ((rc = snd_pcm_writei(handle_w, buffer, frames)) < 0) {
    fprintf(stderr, "write error 2: %s\n", snd_strerror(rc));
  }

  /* Start devices */
  if ((rc = snd_pcm_start(handle_r)) < 0) {
    printf("Go error: %s\n", snd_strerror(rc));
    exit(0);
  }

  while (loops > 0) {
    loops--;
    //fprintf(stderr, "Loop = %ld\n", loops);

    rc = snd_pcm_readi(handle_r, buffer, frames);
    if (rc == -EPIPE) {
      /* EPIPE means overrun */
      fprintf(stderr, "overrun occurred\n");
      snd_pcm_prepare(handle_r);
    } else if (rc < 0) {
      fprintf(stderr,
              "error from readi: %s\n",
              snd_strerror(rc));
    } else if (rc != (int)frames) {
      fprintf(stderr, "short read, read %d frames\n", rc);
    }

    rc = snd_pcm_writei(handle_w, buffer, frames);
    if (rc == -EPIPE) {
      /* EPIPE means underrun */
      fprintf(stderr, "underrun occurred\n");
      //snd_pcm_recover(handle_w, -EPIPE, 0);
      snd_pcm_prepare(handle_w);
    } else if (rc < 0) {
      fprintf(stderr,
              "error from writei: %s\n",
              snd_strerror(rc));
    } else if (rc != (int)frames) {
      fprintf(stderr,
              "short write, write %d frames\n", rc);
    }
  }

  /* Clean up */
  snd_pcm_unlink(handle_r);
  snd_pcm_hw_free(handle_r);
  snd_pcm_hw_free(handle_w);
  snd_pcm_drop(handle_r);
  snd_pcm_close(handle_r);
  snd_pcm_drain(handle_w);
  snd_pcm_close(handle_w);
  snd_output_close(output);
  free(buffer);

  return 0;
}
