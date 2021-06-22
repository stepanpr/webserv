#include "Server.hpp"
#include "Config.hpp"


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

Server::Server()
{
}

Server::~Server()
{
}

Server::Server(const Server &copy)
{
}

Server	&Server::operator=(const Server &copy)
{
	return (*this);
}


/*-------------------------------------------------------------------------------*/




/* START SERVER WITH POLL */
int Server::startServer(struct s_config *config)
{


	pthread_mutex_lock(&lock);
	std::cout << std::endl << WHITE_B << "Server start! " << WHITE <<"(ID: " 
	<< config->serverID << "; port: " << config->listen << ")" << RESET << std::endl;
	pthread_mutex_unlock(&lock);



	//SERVER_CODE
	listen_sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//создание сокета от которого мы будем ожидать входящих данных
	if (listen_sock_fd < 0)
	{
		printf("server: Cannot create TCP socket: %s\n", strerror(errno));
		return -1;
	}

	opt = 1;
	if (0 != setsockopt(listen_sock_fd, SOL_SOCKET, SO_REUSEADDR, (const void*) &opt, sizeof(opt))) //опции сокета
	{
		printf("server: Error: could not allow reuse address on socket: %s\n", strerror(errno));
		close(listen_sock_fd);
		return 0;
	}

	servaddr = {0};											//заполняем структуру sockaddr_in 
	servaddr.sin_family = AF_INET;							// AF_INET определяет, что используется сеть для работы с сокетом
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 			//связывает сокет со всеми доступными интерфейсами
	servaddr.sin_port = htons(stoi(config->listen)); 		//port

	if (-1 == bind(listen_sock_fd, (struct sockaddr*)&servaddr, sizeof(servaddr))) //Bind: Привязка сокета к адресу
	{
		printf("server: Error on call 'bind': %s\n", strerror(errno));
		return -1;
	}

	if (-1 == listen(listen_sock_fd, SOMAXCONN)) //1		//Listen: Подготовка сокета к принятию входящих соединений, SOMAXCONN - максимально возможное число одновременных TCP-соединений, можно указать нужное число
	{
		printf("server: Error on call 'listen': %s\n", strerror(errno));
		return -1;
	}



	pollLoop(*config);

	std::cout << "EXIT SERV\n";
	return 0;
}





int Server::pollLoop(struct s_config &config)
{


	/*	
	**	struct pollfd {
	**   	int fd;			- описатель файла
	**  	short events;	- запрошенные события
	**		short revents;	- возвращенные события
	**	};
	**	https://www.opennet.ru/man.shtml?category=2&topic=poll
	*/
	struct pollfd pfd_array[1 /* listen */ + MAX_CLIENTS] =
		{
			{
				.fd = listen_sock_fd,
				.events = POLLIN		// запрошенные события (POLLIN: 0x0001 Можно считывать данные)
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

		/* 
		** int poll(struct pollfd fds[], nfds_t nfds, int timeout)
		** pollfd fds[] - используется для хранения файлового дескриптора сокета, статус которого необходимо проверить;
		** nfds - параметр типа nfds_t, используемый для обозначения бщее количества элементов (клиентов);
		** timeout - время блокировки (в миллисекундах)
		*/ 
		int ret = poll(pfd_array, 1 + MAX_CLIENTS, -1 /* infinity */); //-1 (infinity) или значние TIMEOUT
		if (ret == -1)
		{
			printf("Error on call 'poll': %s", strerror(errno));
			return -1;
		}
		else if ( ret == 0 )
		{
			// printf("TIMEOUT");
		}
		else
		{
			// обработка попытки подключения клиента
			if ((listen_sock->revents & POLLIN) != 0)
			{
				listen_sock->revents &= ~POLLIN;

				// struct sockaddr_in cliaddr;		
				socklen_t clilen = sizeof(struct sockaddr_in);
				int sock_fd = accept(listen_sock_fd, (struct sockaddr*)&cliaddr, &clilen);	//Accept: Ожидание входящего соединения 
				if (sock_fd > 0)
				{
					// ограничение числа подключений
					if (clients_count == MAX_CLIENTS)
					{
						close(sock_fd);
						std::cout << WHITE_B << MAX_CLIENTS << WHITE 
						<<" clients already connected, unexpected new connection have discarded" << RESET << std::endl;
						// printf("%d clients already connected, unexpected new connection have discarded\n", MAX_CLIENTS);
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

								// printf("Client %d has been connected from \nClients count = %d\n", i, clients_count);
								std::cout << WHITE <<"!client " << WHITE_B << i << WHITE << " has been connected from " << WHITE_B 
								<< inet_ntoa((in_addr)cliaddr.sin_addr) << ":" << config.listen << WHITE << " | clients total: " 
								<< WHITE_B << clients_count << RESET << std::endl;
								// std::cout << "!got connection from " << inet_ntoa((in_addr)cliaddr.sin_addr) << std::endl;
								break;
							}
						}
					}
				}
			}
			request(&(*pfd_array), clients_count, i);
		}
	}

}







int Server::request(struct pollfd *pfd_array, int &clients_count, int &i) //&
{

	// request(pfd_array);
	for (i = 0; i < MAX_CLIENTS; i++)
	{
		if (pfd_array[1 + i].fd != -1 && (pfd_array[1 + i].revents & POLLIN) != 0)
		{
			pfd_array[1 + i].revents &= ~POLLIN;

			uint8_t buf[1024];
			int ret = recv(pfd_array[1 + i].fd, buf, 1024, 0);	/* Возврат из функции recv происходит, когда модуль TCP решает передать процессу полученные от клиента данные. Данные возвращается в буфере buf, размер которого передается в третьем аргументе. В четвертом аргументе могут передаваться дополнительные опциипараметры. Функция возвращает число байтов, которые модуль TCP записал в буфер buf; если функция возвращает ноль, то клиент данных для передачи больше не имеет.*/

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
				std::cout << WHITE <<"Client " WHITE_B << i << WHITE << " has been disconnected | clients total: " << WHITE_B << clients_count << RESET << std::endl;
				// printf("Client %d has been disconnected\nClients count = %d\n\n", i, clients_count);
			} else if (ret > 0)
			{
				buf[ret] = '\0';
				std::cout << WHITE_B << ret << WHITE << " bytes received from client " << WHITE_B << i << RESET << std::endl;
				std::cout << GREEN << buf << RESET << std::endl;
				

				response(&(*pfd_array), i);
			}
		}
	}

	return 0;
}






int Server::response(struct pollfd *pfd_array, int &i)
{
	std::stringstream response_body;////////////////////////////////////
	std::ifstream file; // создаем объект класса ifstream
	file.open("www/site.com/index.html"); // открываем файл
	if (!file) 
	{
		std::cout << "Файл не открыт\n\n"; 
		return -1;
	}
	else
	{
		std::cout << "Все ОК! Файл открыт!\n\n";
		// std::string file_buffer;
		char *file_buffer = new char[1000 + 1]; file_buffer[1000] = 0;
		// response_body << file;
		file.read(file_buffer, 300);
		// for(file >> file_buffer; !file.eof(); file >> file_buffer)
		// 	std::cout << file_buffer;
		response_body << file_buffer;
	}


		std::stringstream response;
		response << "HTTP/1.1 200 OK\r\n"
			<< "Version: HTTP/1.1\r\n"
		<< "Content-Type: text/html; charset=utf-8\r\n"
		<< "Content-Length: " << response_body.str().length()
		<< "\r\n\r\n"
		<< response_body.str();

	if(-1 == send(pfd_array[1 + i].fd, response.str().c_str(), response.str().length(), 0))
	{
		printf("Error on call 'send': %s\n", strerror(errno));
		return -1;
	}

	return 0;
}






















/*-------------------------------------------------------------------------------*/











// /* START SERVER WITH POLL */
// int Server::startServer(struct s_config *config)
// {


// 	pthread_mutex_lock(&lock);
// 	std::cout << std::endl << "Server start! (ID: " << config->serverID << ")" << std::endl;
// 	pthread_mutex_unlock(&lock);

// 	// std::cout << "listen: " << config->listen << std::endl;
// 	// std::cout << "server_name: " << config->server_name << std::endl;
// 	// std::cout << "error_page: " << config.error_page << std::endl;
// 	// std::cout << "index: " << config.location[0].index << std::endl;





// 	//SERVER_CODE





// 	int listen_sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//создание сокета от которого мы будем ожидать входящих данных
// 	if (listen_sock_fd < 0)
// 	{
// 		printf("server: Cannot create TCP socket: %s\n", strerror(errno));
// 		return -1;
// 	}

// 	int opt = 1;
// 	if (0 != setsockopt(listen_sock_fd, SOL_SOCKET, SO_REUSEADDR, (const void*) &opt, sizeof(opt))) //опции сокета
// 	{
// 		printf("server: Error: could not allow reuse address on socket: %s\n", strerror(errno));
// 		close(listen_sock_fd);
// 		return 0;
// 	}

// 	struct sockaddr_in servaddr = {0};						//заполняем структуру sockaddr_in 
// 	servaddr.sin_family = AF_INET;							// AF_INET определяет, что используется сеть для работы с сокетом
// 	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 			//связывает сокет со всеми доступными интерфейсами
// 	servaddr.sin_port = htons(stoi(config->listen)); 		//port

// 	if (-1 == bind(listen_sock_fd, (struct sockaddr*)&servaddr, sizeof(servaddr))) //Bind: Привязка сокета к адресу
// 	{
// 		printf("server: Error on call 'bind': %s\n", strerror(errno));
// 		return -1;
// 	}

// 	if (-1 == listen(listen_sock_fd, SOMAXCONN)) //1		//Listen: Подготовка сокета к принятию входящих соединений, SOMAXCONN - максимально возможное число одновременных TCP-соединений, можно указать нужное число
// 	{
// 		printf("server: Error on call 'listen': %s\n", strerror(errno));
// 		return -1;
// 	}

// 	    /*	struct pollfd {
//         **   	int fd;			- описатель файла
//         **  	short events;	-запрошенные события
//         **		short revents;	- возвращенные события
//         **	};
// 		**	https://www.opennet.ru/man.shtml?category=2&topic=poll
// 		*/
// 	struct pollfd pfd_array[1 /* listen */ + MAX_CLIENTS] =
// 		{
// 			{
// 				.fd = listen_sock_fd,
// 				.events = POLLIN		// запрошенные события (POLLIN: 0x0001 Можно считывать данные)
// 			}
// 		};

// 	int i;
// 	for (i = 0; i < MAX_CLIENTS; i++)
// 	{
// 		pfd_array[1 + i].fd = -1;
// 	}
// 	struct pollfd *listen_sock = pfd_array;

// 	int clients_count = 0;

// 	while(1)
// 	{
// 		while (1)
// 		{

// 			/* int poll(struct pollfd fds[], nfds_t nfds, int timeout)
// 			** pollfd fds[] - используется для хранения файлового дескриптора сокета, статус которого необходимо проверить;
// 			** nfds - параметр типа nfds_t, используемый для обозначения бщее количества элементов (клиентов);
// 			** timeout - время блокировки (в миллисекундах)
// 			*/ 
// 			int ret = poll(pfd_array, 1 + MAX_CLIENTS, -1 /* infinity */); //-1 (infinity) или значние TIMEOUT
// 			if (ret == -1)
// 			{
// 				printf("Error on call 'poll': %s", strerror(errno));
// 				return -1;
// 			}
// 			else if ( ret == 0 )
// 			{
// 				// printf("TIMEOUT");
// 			}
// 			else
// 			{
// 				// обработка попытки подключения клиента
// 				if ((listen_sock->revents & POLLIN) != 0)
// 				{
// 					listen_sock->revents &= ~POLLIN;

// 					struct sockaddr_in cliaddr;		//структура clnt_addr, в которую мы будем записывать адрес и порт подсоединившегося клиента
// 					socklen_t clilen = sizeof(struct sockaddr_in);
// 					int sock_fd = accept(listen_sock_fd, (struct sockaddr*)&cliaddr, &clilen);	//Accept: Ожидание входящего соединения 
// 					if (sock_fd > 0)
// 					{
// 						// ограничение числа подключений
// 						if (clients_count == MAX_CLIENTS)
// 						{
// 							close(sock_fd);
// 							printf("%d clients already connected, unexpected new connection have discarded\n", MAX_CLIENTS);
// 						} else
// 						{
// 							clients_count++;
// 							opt = 1;
// 							setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&opt, sizeof(opt));

// 							for (i = 0; i < MAX_CLIENTS; i++)
// 							{
// 								if (pfd_array[1 + i].fd == -1)
// 								{
// 									pfd_array[1 + i].fd = sock_fd;
// 									pfd_array[1 + i].events = POLLIN;

// 									printf("Client %d has been connected\nClients count = %d\n", i, clients_count);
// 									break;
// 								}
// 							}
// 						}
// 					}
// 				}

// 				for (i = 0; i < MAX_CLIENTS; i++)
// 				{
// 					if (pfd_array[1 + i].fd != -1 && (pfd_array[1 + i].revents & POLLIN) != 0)
// 					{
// 						pfd_array[1 + i].revents &= ~POLLIN;

// 						uint8_t buf[1024];
// 						int ret = recv(pfd_array[1 + i].fd, buf, 1024, 0);	/* Возврат из функции recv происходит, когда модуль TCP решает передать процессу полученные от клиента данные. Данные возвращается в буфере buf, размер которого передается в третьем аргументе. В четвертом аргументе могут передаваться дополнительные опциипараметры. Функция возвращает число байтов, которые модуль TCP записал в буфер buf; если функция возвращает ноль, то клиент данных для передачи больше не имеет.*/

// 						if (ret < 0)
// 						{
// 							printf("Error on call 'recv': %s\n", strerror(errno));
// 							return -1;
// 						} else if (ret == 0)
// 						{
// 							close(pfd_array[1 + i].fd);
// 							pfd_array[1 + i].fd = -1;
// 							pfd_array[1 + i].events = 0;
// 							clients_count--;
// 							printf("Client %d has been disconnected\nClients count = %d\n\n", i, clients_count);
// 						} else
// 						{
// 							std::stringstream response_body;
// 							std::ifstream file; // создаем объект класса ifstream
// 							file.open("www/site.com/index.html"); // открываем файл
// 							if (!file) 
// 							{
// 								std::cout << "Файл не открыт\n\n"; 
// 								return -1;
// 							}
// 							else
// 							{
// 								std::cout << "Все ОК! Файл открыт!\n\n";
// 								// std::string file_buffer;
// 								char *file_buffer = new char[1000 + 1]; file_buffer[1000] = 0;
// 								// response_body << file;
// 								file.read(file_buffer, 300);
// 								// for(file >> file_buffer; !file.eof(); file >> file_buffer)
// 								// 	std::cout << file_buffer;
// 								response_body << file_buffer;
// 							}

// 								std::stringstream response;
// 								response << "HTTP/1.1 200 OK\r\n"
// 								<< "Version: HTTP/1.1\r\n"
// 								<< "Content-Type: text/html; charset=utf-8\r\n"
// 								<< "Content-Length: " << response_body.str().length()
// 								<< "\r\n\r\n"
// 								<< response_body.str();



// 							printf("%d bytes received from client %d\n", ret, i);
// 							printf("Message from client %d: %.*s\n", i, ret, (char *)buf);
// 							if(-1 == send(pfd_array[1 + i].fd, response.str().c_str(), response.str().length(), 0))
// 							{
// 								printf("Error on call 'send': %s\n", strerror(errno));
// 								return -1;
// 							}
// 						}
// 					}
// 				}
// 		}
// 		}
// 	}




// 	std::cout << "EXIT SERV\n";
// 	return 0;
// }

