#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include "common.h"

static void do_something(int connfd)
{
	char rbuf[64] = {};
	ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
	if (n < 0) {
		S_MSG("read()");
		return;
	}
	MSG("client says: %s\n", rbuf);

	char wbuf[] = "world";
	write(connfd, wbuf, strlen(wbuf));
}
/**
+-----+------+-----+------+--------
| len | msg1 | len | msg2 | more...
+-----+------+-----+------+--------
The protocol consists of 2 parts: a 4-byte little-endian integer (uint32) indicating the length of the following request, and a variable length request.
**/

static int32_t one_request(int connfd)
{
	// 4 bytes header

	char rbuf[4 + MAX_MSG + 1];

	errno = 0;
	int32_t err = read_full(connfd, rbuf, 4);
	if (err) {
		if (errno == 0) {
			S_MSG("EOF");
		} else {
			S_MSG("read() error");
		}
		return err;
	}

	uint32_t len = 0;
	memcpy(&len, rbuf, 4);
	if (len > MAX_MSG) {
		S_MSG("too long");
		return -1;
	}

	// request body
	err = read_full(connfd, &rbuf[4], len);
	if (err) {
		S_MSG("read() error");
		return err;
	}

	// do something
	rbuf[4 + len] = '\0';
	printf("client says: %s\n", &rbuf[4]);

	// reply using the same protocol
	const char reply[] = "world";
	char wbuf[4 + sizeof(reply)];
	len = (uint32_t)strlen(reply);
	memcpy(wbuf, &len, 4);
	memcpy(&wbuf[4], reply, len);

	return write_all(connfd, wbuf, 4 + len);
}

static int32_t many_requests(int connfd)
{
	// Read nBytes from the kernel
	// Try to parse out messages
	// Currently using read_full but what we should do is:
	// nRead = read(nbytes)
	// get the len (first 4 bytes)
	// if len == nRead - 4
	//     we have one message
	// elseif len > nRead - 4
	//     we haven't read enough
	//     read(nLeft)
	// elseif len < nRead - 4
	//

	int nMsg = 5;
	int nBytes = (4 + MAX_MSG + 1) * nMsg;

	// 4 bytes header
	char rbuf[nBytes];

	errno = 0;
	int32_t nRead = read(connfd, rbuf, nBytes);

	if (nRead == -1) {
		if (errno == 0) {
			S_MSG("EOF");
		} else {
			S_MSG("read() error");
		}
		return nRead;
	}

	uint32_t len = 0;
	memcpy(&len, rbuf, 4);

	if (len > MAX_MSG) {
		S_MSG("too long");
		return -1;
	}

	if (len == nRead - 4) {
		rbuf[4 + len] = '\0';
		printf("client says: %s\n", &rbuf[4]);
	}

	// do something

	// reply using the same protocol
	const char reply[] = "world";
	char wbuf[4 + sizeof(reply)];
	len = (uint32_t)strlen(reply);
	memcpy(wbuf, &len, 4);
	memcpy(&wbuf[4], reply, len);

	return write_all(connfd, wbuf, 4 + len);
}

int main(void)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	int val = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = ntohs(1234);
	addr.sin_addr.s_addr = ntohl(0); // wildcard address 0.0.0.0

	int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
	if (rv) {
		S_DIE("bind()");
	}
	rv = listen(fd, SOMAXCONN);
	if (rv) {
		S_DIE("listen()");
	}
	while (true) {
		struct sockaddr_in client_addr = {};
		socklen_t socklen = sizeof(client_addr);
		int connfd =
			accept(fd, (struct sockaddr *)&client_addr, &socklen);
		if (connfd < 0) {
			continue;
		}
		int32_t err = many_requests(connfd);
		close(connfd);
	}
}
