/*
 * alsa_test.c
 *
 * Copyright (C) 2009-2013 by Digi International Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * Description: ALSA test application based on [1]
 *
 * [1] http://www.alsa-project.org/alsa-doc/alsa-lib/_2test_2pcm__min_8c-example.html
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <alsa/asoundlib.h>

#define	PROGRAM			"alsa_test"
#define VERSION			"2.0"

#define	PCM_CARD_NAME		"default"
#define PCM_TEST_FORMAT		SND_PCM_FORMAT_U8
#define PCM_TEST_BITRATE	(44100)
#define PCM_TEST_CHANNELS	(2)


/***********************************************************************
 * @Function: main
 * @Return: EXIT_SUCCESS or EXIT_FAILURE
 * @Descr: main applications function
 ***********************************************************************/
int main(void)
{
        int err;
        unsigned int i;
        snd_pcm_t *handle;
        snd_pcm_sframes_t frames, total_frames;
	char *device = PCM_CARD_NAME;
	unsigned char buffer[16 * 1024 * PCM_TEST_CHANNELS];

	/* Create the random buffer */
        for (i = 0; i < sizeof(buffer); i++)
                buffer[i] = random() & 0xff;

	/* Open the PCM-device in playback mode */
        if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
                printf("Could't open PCM '%s': %s\n", device, snd_strerror(err));
                return EXIT_FAILURE;
        }
	
        if ((err = snd_pcm_set_params(handle,
                                      PCM_TEST_FORMAT,
                                      SND_PCM_ACCESS_RW_INTERLEAVED,
                                      PCM_TEST_CHANNELS,
                                      PCM_TEST_BITRATE,
                                      1,
                                      500000)) < 0) {   /* 0.5sec */
                printf("Playback set params error: %s\n", snd_strerror(err));
                goto exit_close_pcm;
        }

	total_frames = sizeof(buffer) / PCM_TEST_CHANNELS;
        for (i = 0; i < 16; i++) {

                frames = snd_pcm_writei(handle, buffer, total_frames);
		if (frames < 0)
                        frames = snd_pcm_recover(handle, frames, 0);

		if (frames < 0) {
                        printf("snd_pcm_writei failed: %s\n", snd_strerror(err));
                        err = EXIT_FAILURE;
			goto exit_close_pcm;
                }
		
                if (frames > 0 && frames < total_frames)
                        printf("Short write (expected %li, wrote %li)\n",
			       total_frames, frames);
        }

	err = EXIT_SUCCESS;
	
 exit_close_pcm:
        snd_pcm_close(handle);
	
        return err;
}
