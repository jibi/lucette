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

#include <stdint.h>

#define UNICORND_SOCK_PATH "/var/run/unicornd.socket"

typedef struct col_s {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} __attribute__ ((packed)) col_t;

extern col_t unicornd_col_black;
extern col_t unicornd_col_white;

void unicornd_connect();
void unicornd_set_brightness(uint8_t brightness);
void unicornd_set_all_pixels(col_t pixels[64]);
void unicornd_set_all_pixels_color(col_t col);
void unicornd_show();

#define UNICORND_CMD_SET_BRIGHTNESS 0
#define UNICORND_CMD_SET_ALL_PIXELS 2
#define UNICORND_CMD_SHOW           3
