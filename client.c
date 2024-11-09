#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "common.h"

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
