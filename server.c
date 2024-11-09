#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
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
		S_DIE(__LINE__, "bind()");
	}
	rv = listen(fd, SOMAXCONN);
	if (rv) {
		S_DIE(__LINE__, "listen()");
	}
	while (true) {
		struct sockaddr_in client_addr = {};
		socklen_t socklen = sizeof(client_addr);
		int connfd =
			accept(fd, (struct sockaddr *)&client_addr, &socklen);
		if (connfd < 0) {
			continue;
		}
		do_something(connfd);
		close(connfd);
	}
}
