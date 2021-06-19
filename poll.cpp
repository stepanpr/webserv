#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>

#define MAX_CLIENTS 8
#define SERVER_PORT 10000

int main()
{
	int listen_sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_sock_fd < 0)
	{
		printf("server: Cannot create TCP socket: %s\n", strerror(errno));
		return -1;
	}

	int opt = 1;
	if (0 != setsockopt(listen_sock_fd, SOL_SOCKET, SO_REUSEADDR, (const void*) &opt, sizeof(opt)))
	{
		printf("server: Error: could not allow reuse address on socket: %s\n", strerror(errno));
		close(listen_sock_fd);
		return 0;
	}

	struct sockaddr_in servaddr = {0};
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVER_PORT);

	if (-1 == bind(listen_sock_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)))
	{
		printf("server: Error on call 'bind': %s\n", strerror(errno));
		return -1;
	}

	if (-1 == listen(listen_sock_fd, 1))
	{
		printf("server: Error on call 'listen': %s\n", strerror(errno));
		return -1;
	}

	struct pollfd pfd_array[1 /* listen */ + MAX_CLIENTS] =
			{
					{
							.fd = listen_sock_fd,
							.events = POLLIN
					}
			};
	int i;
	for (i = 0; i < MAX_CLIENTS; i++)
	{
		pfd_array[1 + i].fd = -1;
	}
	struct pollfd *listen_sock = pfd_array;

	int clients_count = 0;

	while (1)
	{
		int ret = poll(pfd_array, 1 + MAX_CLIENTS, -1 /* infinity */);
		if (ret == -1)
		{
			printf("Error on call 'poll': %s", strerror(errno));
			return -1;
		}
		else
		{
			// обработка попытки подключения клиента
			if ((listen_sock->revents & POLLIN) != 0)
			{
				listen_sock->revents &= ~POLLIN;

				struct sockaddr_in cliaddr;
				socklen_t clilen = sizeof(struct sockaddr_in);
				int sock_fd = accept(listen_sock_fd, (struct sockaddr*)&cliaddr, &clilen);
				if (sock_fd > 0)
				{
					// ограничение числа подключений
					if (clients_count == MAX_CLIENTS)
					{
						close(sock_fd);
						printf("%d clients already connected, unexpected new connection have discarded\n", MAX_CLIENTS);
					} else
					{
						clients_count++;
						opt = 1;
						setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&opt, sizeof(opt));

						for (i = 0; i < MAX_CLIENTS; i++)
						{
							if (pfd_array[1 + i].fd == -1)
							{
								pfd_array[1 + i].fd = sock_fd;
								pfd_array[1 + i].events = POLLIN;

								printf("Client %d has been connected\nClients count = %d\n", i, clients_count);
								break;
							}
						}
					}
				}
			}

			for (i = 0; i < MAX_CLIENTS; i++)
			{
				if (pfd_array[1 + i].fd != -1 && (pfd_array[1 + i].revents & POLLIN) != 0)
				{
					pfd_array[1 + i].revents &= ~POLLIN;

					uint8_t buf[64];
					int ret = recv(pfd_array[1 + i].fd, buf, 64, 0);

					if (ret < 0)
					{
						printf("Error on call 'recv': %s\n", strerror(errno));
						return -1;
					} else if (ret == 0)
					{
						close(pfd_array[1 + i].fd);
						pfd_array[1 + i].fd = -1;
						pfd_array[1 + i].events = 0;
						clients_count--;
						printf("Client %d has been disconnected\nClients count = %d\n\n", i, clients_count);
					} else
					{
						printf("%d bytes received from client %d\n", ret, i);
						printf("Message from client %d: %.*s\n", i, ret, (char *)buf);
						if(-1 == send(pfd_array[1 + i].fd, (void *)"Response", 8, 0))
						{
							printf("Error on call 'send': %s\n", strerror(errno));
							return -1;
						}
					}
				}
			}
		}
	}
}