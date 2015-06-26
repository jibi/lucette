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
#include <complex.h>
#include <fftw3.h>
#include <math.h>

#include <limits.h>

#include <lucette/alsa.h>
#include <lucette/fft.h>

#include <lucette/unicornd_helpers.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

#define HIST_SIZE 43 /* 44100 / CAPTURE_SIZE, about a second */
#define BANDS_COUNT 32
#define BAND_SAMPLES (CAPTURE_SIZE / BANDS_COUNT)

#define C 2.4

#define complex_mod(x) (pow(__real__ x, 2) + pow(__imag__ x, 2))

fftw_plan plan;
fftw_complex *buffer_in;
fftw_complex *buffer_out;

float Es[BAND_SAMPLES];
float Ei[HIST_SIZE][BAND_SAMPLES];

int hist_start = 0;
int hist_end   = 0;
int hist_size  = 0;

void do_stuff();

void
init_fft()
{
	buffer_in  = fftw_malloc(CAPTURE_SIZE * sizeof(fftw_complex));
	buffer_out = fftw_malloc(CAPTURE_SIZE * sizeof(fftw_complex));

	plan       = fftw_plan_dft_1d(CAPTURE_SIZE, buffer_in, buffer_out, FFTW_REDFT00, FFTW_ESTIMATE);
}

static inline
void
fill_fft_buffer_in(float *buffer)
{
	int i;

	for (i = 0; i < CAPTURE_SIZE; i++) {
		__real__ buffer_in[i] = buffer[i];
		__imag__ buffer_in[i] = 0;
	}
}

static inline
void
get_fft_buffer_out(float *buffer)
{
	int i;

	for (i = 0; i < CAPTURE_SIZE; i++) {
		buffer[i] = complex_mod(buffer_out[i]);
	}
}

static inline
void
calc_subband_energy(float *buffer, int i)
{
	int j;

	Es[i] = 0;

	for (j = i * BAND_SAMPLES; j < (i + 1) * BAND_SAMPLES; j++) {
		Es[i] += buffer[j];
	}

	Es[i] *= 32;
	Es[i] /= 1024;

	//Es[i] /= 32;
}

static inline
void
calc_subbands_energy(float *buffer)
{
	int i;

	for (i = 0; i < BANDS_COUNT; i++) {
		calc_subband_energy(buffer, i);
	}
}

void
do_fft(float *buffer)
{
	fill_fft_buffer_in(buffer);
	fftw_execute(plan);
	get_fft_buffer_out(buffer);

	calc_subbands_energy(buffer);

	hist_end = (hist_end + 1) % HIST_SIZE;

	if (hist_size == HIST_SIZE) {
		do_stuff();

		memcpy(Ei[hist_start], Es, BAND_SAMPLES * sizeof(float));
		hist_start = (hist_start + 1) % HIST_SIZE;
	} else {

		memcpy(Ei[hist_start], Es, BAND_SAMPLES * sizeof(float));
		hist_size++;
	}
}

int
do_stuff_for_band(int i)
{
	float inst_e, avg_e;
	int j;

	inst_e = Es[i];
	avg_e  = 0;

	for (j = 0; j < HIST_SIZE; j++) {
		avg_e += Ei[j][i];
	}

	avg_e /= HIST_SIZE;

	if (inst_e > C * avg_e) {
		return 1;
	} else {
		return 0;
	}

	return 0;
}

void
do_stuff()
{
	int i;
	int boom_count = 0;
	static int last_beat = -4;

	for (i = 0; i < BANDS_COUNT / 8; i++) {
		boom_count += do_stuff_for_band(i);
	}

	if (boom_count) {
		accendi_lucette();
		last_beat=0;
	} else {
		if (last_beat > -2) {
			accendi_lucette();
		} else {
			spegni_lucette();
		}

		last_beat--;
	}
}

