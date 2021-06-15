
#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <err.h>
#include <sstream>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>




class Server
{
	// class Sockets
	// {
	// 	public:
	// 	int listen_fd;
	// 	int client_fd;
	// };




	public:
		bool running;		//запущен ли сервер
		int result;
		int serverFD; 		//сокет сервера
		// int clientFD; 		//сокет который возвращает accept

		struct sockaddr_in srv_addr;
		struct sockaddr_in cli_addr;
		// socklen_t sin_len;


		Server();
		int startServer();
		int requestHandler(int &clientFD);
		int responseHandler(int &clientFD);
		// Sockets sock;
		
};





# define RED  "\e[0;31m"
# define GREEN "\033[0;32m"
# define YELLOW "\e[0;33m"
# define BLUE "\033[0;34m"
# define PURPLE "\033[0;35m"
# define CYAN "\e[0;36m"
# define WHITE "\e[0;37m"
# define RED_B "\033[1;31m"
# define GREEN_B "\033[1;32m"
# define YELLOW_B "\e[1;33m"
# define BLUE_B "\033[1;34m"
# define PURPLE_B "\033[1;35m"
# define CYAN_B "\e[1;36m"
# define WHITE_B "\e[1;37m"
# define RESET "\033[0m"
#endif