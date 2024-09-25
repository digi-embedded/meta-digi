/*
 * Copyright (C) 2017, Digi International Inc.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

int main(int argc, char *argv[])
{
	int len, ret = EXIT_SUCCESS;
	snd_pcm_t *handle;
	char *device;

	if (argc < 2) {
		printf("Usage: %s [CARD NUMBER]\n", argv[0]);
		return EXIT_FAILURE;
	}

	len = strlen("hw:") + strlen(argv[1]) + 1;
	device = calloc(1, len);
	snprintf(device, len, "hw:%s", argv[1]);

	/* Open the PCM-device in playback mode */
	if (snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0) < 0) {
		printf("Could't open PCM '%s'\n", device);
		ret = EXIT_FAILURE;
	} else {
		printf("Device %s opened successfully\n", device);
		snd_pcm_close(handle);
	}

	free(device);
	return ret;
}
