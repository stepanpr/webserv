#ifndef SERVER_HPP
# define SERVER_HPP
# include "Global.hpp"
# include "RequestParser.hpp"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/types.h>
#include <netdb.h>
#include <cstdio>
#include <string.h>
#define MAX_CLIENTS 20
// #define BACKLOG 10

class Config;

class Server
{
	private:
		int listen_sock_fd;
		struct sockaddr_in servaddr;
		struct sockaddr_in cliaddr;	//структура clnt_addr, в которую мы будем записывать адрес и порт подсоединившегося клиента

		// int clients_count;
		int opt;

		// int i; //итератор (количество клиентов)

		


	public:
		Server();
		Server(const Server &copy);
		~Server();
		Server &operator=(const Server &copy);

		int startServer(struct s_config *config);
		int pollLoop(struct s_config &config);
		int request(struct pollfd *pfd_array, int &clients_count, int &i, struct s_config &config); //&
		int response(struct pollfd *pfd_array, int &i, RequestParser &HTTPrequest, struct s_config &config);




};

#endif
