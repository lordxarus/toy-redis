#pragma once

#define __FILENAME__ \
	(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define MSG(fmt, ...) msg(__FILENAME__, __LINE__, fmt, __VA_ARGS__)
#define DIE(fmt, ...) die(__FILENAME__, __LINE__, fmt, __VA_ARGS__)
#define S_MSG(...) msg(__FILENAME__, __LINE__, "%s", __VA_ARGS__)
#define S_DIE(...) die(__FILENAME__, __LINE__, "%s", __VA_ARGS__)

void msg(char *filename, int line_number, const char *fmt, ...);
void die(char *filename, int line_number, const char *fmt, ...);
