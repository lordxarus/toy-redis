#pragma once
#include <stdint.h>
#include <stdio.h>

size_t MAX_MSG = 4096;

#define __FILENAME__ \
	(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define MSG(fmt, ...) msg(__FILENAME__, __LINE__, fmt, __VA_ARGS__)
#define DIE(fmt, ...) die(__FILENAME__, __LINE__, fmt, __VA_ARGS__)
#define S_MSG(...) msg(__FILENAME__, __LINE__, "%s\n", __VA_ARGS__)
#define S_DIE(...) die(__FILENAME__, __LINE__, "%s\n", __VA_ARGS__)

int32_t read_full(int fd, char *buf, size_t n);
int32_t write_all(int fd, const char *buf, size_t n);
void msg(char *filename, int line_number, const char *fmt, ...);
void die(char *filename, int line_number, const char *fmt, ...);
