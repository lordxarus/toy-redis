#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

int32_t read_full(int fd, char *buf, size_t n)
{
	while (n > 0) {
		ssize_t rv = read(fd, buf, n);
		if (rv <= 0) {
			return -1;
		}
		assert((size_t)rv <= n);
		n -= (size_t)rv;
		buf += rv;
	}
	return 0;
}

int32_t write_all(int fd, const char *buf, size_t n)
{
	while (n > 0) {
		ssize_t rv = write(fd, buf, n);
		if (rv <= 0) {
			return -1;
		}
		assert((size_t)rv <= n);
		n -= (size_t)rv;
		buf += rv;
	}
	return 0;
}

void logger(FILE *fd, char *filename, int line_number, const char *fmt,
	    va_list vargs)
{
	fprintf(fd, "%s:%d: ", filename, line_number);
	vfprintf(fd, fmt, vargs);
}

void msg(char *filename, int line_number, const char *fmt, ...)
{
	va_list vargs;
	va_start(vargs, fmt);
	logger(stderr, filename, line_number, fmt, vargs);
	va_end(vargs);
}

void die(char *filename, int line_number, const char *fmt, ...)
{
	va_list vargs;
	va_start(vargs, fmt);
	size_t len = strlen(fmt) + sizeof("DIE: ");
	char *die_fmt = malloc(len);

	if (!die_fmt) {
		fprintf(stderr, "couldn't allocate memory for die. yikers.\n");
		logger(stderr, filename, line_number, fmt, vargs);
	} else {
		sprintf(die_fmt, "die: %s", fmt);
		logger(stderr, filename, line_number, die_fmt, vargs);
		free(die_fmt);
	}

	va_end(vargs);
	exit(EXIT_FAILURE);
}
