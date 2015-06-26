/*
 * Copyright (C) 2015 jibi <jibi@paranoici.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

#include <alsa/asoundlib.h>

#include <lucette/log.h>
#include <lucette/alsa.h>

#include <byteswap.h>

snd_pcm_t *
setup_alsa_capture()
{
	snd_pcm_t *capture_handle;
	snd_pcm_hw_params_t *hw_params;
	int err;

	char *device            = DEFAULT_DEVICE;
	uint32_t rate           = RATE;
	uint32_t channels       = CHANNELS;
	snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

	if ((err = snd_pcm_open (&capture_handle, device, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
		fatal_tragedy(1, "cannot open audio device %s (%s)", device, snd_strerror (err));
	}
	log_info("audio interface opened");

	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		fatal_tragedy(1 , "cannot allocate hardware parameter structure (%s)", snd_strerror (err));
	}
	log_info("hw_params allocated");

	if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
		fatal_tragedy(1, "cannot initialize hardware parameter structure (%s)", snd_strerror (err));
	}
	log_info("hw_params initialized");

	if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fatal_tragedy(1, "cannot set access type (%s)", snd_strerror (err));
	}
	log_info("hw_params access setted");

	if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format)) < 0) {
		fatal_tragedy(1, "cannot set sample format (%s)", snd_strerror (err));
	}
	fprintf(stdout, "hw_params format setted");

	if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
		fatal_tragedy(1, "cannot set sample rate (%s)", snd_strerror (err));
	}
	log_info("hw_params rate setted");

	if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, channels)) < 0) {
		fatal_tragedy(1, "cannot set channel count (%s)", snd_strerror (err));
	}
	log_info("hw_params channels setted");

	if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
		fatal_tragedy(1, "cannot set parameters (%s)", snd_strerror (err));
	}
	fprintf(stdout, "hw_params setted");

	snd_pcm_hw_params_free (hw_params);

	if ((err = snd_pcm_prepare (capture_handle)) < 0) {
		fatal_tragedy(1, "cannot prepare audio interface for use (%s)", snd_strerror (err));
	}
	log_info("audio interface prepared");

	return capture_handle;
}

void
rec_loop(snd_pcm_t *capture_handle, void(*callback)(float*))
{
	int err;
	short buf[CAPTURE_SIZE];
	float f_buf[CAPTURE_SIZE];

	while (1) {

		if ((err = snd_pcm_readi(capture_handle, buf, CAPTURE_SIZE)) != CAPTURE_SIZE) {
			fatal_tragedy(1, "read from audio interface failed (%s)", snd_strerror (err));
		}

		for (int i = 0; i < CAPTURE_SIZE; i++) {
			//f_buf[i] = (float) ((short)__bswap_16(buf[i])) / (float) (USHRT_MAX / 2); /* 0x7fff */
			//f_buf[i] = (float) (buf[i]); // / (float) (USHRT_MAX / 2); /* 0x7fff */
			f_buf[i] = (float) buf[i];
		}

		callback(f_buf);
	}
}
