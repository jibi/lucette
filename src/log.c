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
#include <stdarg.h>

#include <lucette/log.h>

#define PRINT_RED     "\033[22;31m"
#define PRINT_GREEN   "\033[22;32m"
#define PRINT_YELLOW  "\033[01;33m"
#define PRINT_RST     "\033[0m"

static void do_log(const char *fmt, va_list args, const char *type);

void
log_info(const char *msg, ...)
{
	va_list args;

	va_start(args, msg);
	do_log(msg, args, PRINT_GREEN "info " PRINT_RST);
	va_end(args);
}

void
log_error(const char *msg, ...)
{
	va_list args;

	va_start(args, msg);
	do_log(msg, args, PRINT_YELLOW "error" PRINT_RST);
	va_end(args);
}

void
fatal_tragedy(int code, const char *msg, ...)
{
	va_list args;

	va_start(args, msg);
	do_log(msg, args, PRINT_RED "fatal" PRINT_RST);
	va_end(args);

	exit(code);
}

static void
do_log(const char *fmt, va_list args, const char *type)
{
	char msgbuf[1024];
	char fmtbuf[1024];

	snprintf(fmtbuf, sizeof(fmtbuf), "[%s] %s\n", type, fmt);
	vsnprintf(msgbuf, sizeof(msgbuf), fmtbuf, args);
	fprintf(stderr, "%s", msgbuf);
}
