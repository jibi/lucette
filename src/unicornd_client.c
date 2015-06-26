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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <lucette/unicornd_client.h>

int unicornd_socket, len;
struct sockaddr_un remote;

col_t unicornd_col_black = { .r = 0, .b = 0, .g = 0 };
col_t unicornd_col_white = { .r = 255, .b = 255, .g = 255 };

void
unicornd_connect()
{
	if ((unicornd_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, UNICORND_SOCK_PATH);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);

	if (connect(unicornd_socket, (struct sockaddr *)&remote, len) == -1) {
		perror("connect");
		exit(1);
	}

}

void
unicornd_set_brightness(uint8_t brightness)
{
	uint8_t cmd = UNICORND_CMD_SET_BRIGHTNESS;

	write(unicornd_socket, &cmd, sizeof(uint8_t));
	write(unicornd_socket, &brightness, sizeof(uint8_t));

}

void
unicornd_set_all_pixels(col_t pixels[64])
{
	uint8_t cmd = UNICORND_CMD_SET_ALL_PIXELS;

	write(unicornd_socket, &cmd, sizeof(uint8_t));
	write(unicornd_socket, pixels, 64 * sizeof(col_t));

}

void
unicornd_set_all_pixels_color(col_t col)
{
  int i;
  col_t cols[64];

  for (i = 0; i < 64; i++) {
    cols[i].r = col.r;
    cols[i].g = col.g;
    cols[i].b = col.b;
  }

  unicornd_set_all_pixels(cols);

}

void
unicornd_show()
{
	uint8_t cmd = UNICORND_CMD_SHOW;

	write(unicornd_socket, &cmd, sizeof(uint8_t));
}

