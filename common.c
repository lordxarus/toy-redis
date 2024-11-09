#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
