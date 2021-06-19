
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
#include <thread>
#include <pthread.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <functional>


class Server
{
	private:
		bool running;		//запущен ли сервер
		int result;
		int serverFD; 		//сокет сервера
		int clientFD; 		//сокет который возвращает accept

		std::stringstream response; 		// ответ клиенту
		// std::stringstream response_body; 	// тело ответа

		struct sockaddr_in srv_addr;
		struct sockaddr_in cli_addr;
		// socklen_t sin_len;

	public:
		Server();
		int startServer();
		void stopServer();
		void requestHandler();
		int responseHandler(std::string str);

		std::thread first;

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