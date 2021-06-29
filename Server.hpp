#ifndef SERVER_HPP
# define SERVER_HPP
# include "main.hpp"
# include "RequestParser.hpp"
#include "Connection.hpp"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/types.h>
#include <netdb.h>
#include <cstdio>
#include "structure.h"
#include <string>
#include <map>
#define MAX_CLIENTS 20
#include <fcntl.h>
#include "Socket.hpp"
// #define BACKLOG 10

class Config;

class Connection;

class Server
{
	private:

		Socket						_listenSock;
//		int							_listen_sock_fd;
//		struct sockaddr_in			_servaddr;
//		struct sockaddr_in			_cliaddr;	//структура clnt_addr, в которую мы будем записывать адрес и порт подсоединившегося клиента
//		int							opt;
		t_config					*_config;
//		std::map<int, Connection>	_mapConnection; //key = fd, value = Connection этого fd


		// int i; //итератор (количество клиентов)
		/********* Private methods *******/
		void	_createListenSocket();



	public:
		Server();
		Server(const Server &copy);
		~Server();
		Server &operator=(const Server &copy);

		int startServer(struct s_config *config);
		int pollLoop();
		int request(struct pollfd *pfd_array, int &clients_count, int &i, struct s_config &config); //&
		int response(struct pollfd *pfd_array, int &i, RequestParser &HTTPrequest, struct s_config &config);

		int responseSend(std::string response, struct pollfd *pfd_array, int &i);





};

#endif
