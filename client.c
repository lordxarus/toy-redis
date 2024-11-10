#include <stdint.h>
#include <errno.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "common.h"

static int32_t query(int fd, const char *text)
{
	uint32_t len = (uint32_t)strlen(text);
	if (len > MAX_MSG) {
		S_MSG("too long");
		return -1;
	}

	// write
	char wbuf[4 + MAX_MSG];
	memcpy(wbuf, &len, 4); // assume little endian
	memcpy(&wbuf[4], text, len);
	int32_t err = write_all(fd, wbuf, 4 + len);
	if (err) {
		return err;
	}

	// read
	char rbuf[4 + MAX_MSG + 1];
	errno = 0;
	err = read_full(fd, rbuf, 4);
	if (err) {
		if (errno == 0) {
			S_MSG("EOF");
		} else {
			S_MSG("read() error");
		}
		return err;
	}

	memcpy(&len, rbuf, 4);
	if (len > MAX_MSG) {
		S_MSG("too long");
		return -1;
	}

	// reply body
	err = read_full(fd, &rbuf[4], len);
	if (err) {
		S_MSG("read() error");
		return err;
	}

	// do something
	rbuf[4 + len] = '\0';
	printf("server says: %s\n", &rbuf[4]);
	return 0;
}

int main(void)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		S_DIE("socket()");
	}

	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = ntohs(1234);
	addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1

	int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
	if (rv) {
		S_DIE("connect()");
	}

	int32_t err = query(fd, "hello1");
	if (err) {
		goto L_DONE;
	}
	err = query(fd, "hello2");
	if (err) {
		goto L_DONE;
	}
	err = query(fd, "hello3");
	if (err) {
		goto L_DONE;
	}
L_DONE:
	close(fd);
	return 0;
	char payload[] = "hello";
	write(fd, payload, strlen(payload));

	char rbuf[64] = {};
	ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);

	if (n != 0) {
		S_DIE("read()");
	}
	MSG("server says: %s\n", rbuf);
	close(fd);
}
