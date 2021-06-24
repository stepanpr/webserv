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
	*this = copy;
}

Server	&Server::operator=(const Server &copy)
{
	this->listen_sock_fd = copy.listen_sock_fd;
	this->servaddr.sin_addr = copy.servaddr.sin_addr;
	this->servaddr.sin_family = copy.servaddr.sin_family;
	this->servaddr.sin_port = copy.servaddr.sin_port;
	return (*this);
}


/*-------------------------------------------------------------------------------*/




/* START SERVER WITH POLL */
int Server::startServer(struct s_config *config)
{


	pthread_mutex_lock(&lock);
	std::cout << std::endl << GREEN_B << "Server started! " << WHITE <<"(ID: "
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

	// servaddr = {0};											//заполняем структуру sockaddr_in
	servaddr.sin_family = AF_INET;							// AF_INET определяет, что используется сеть для работы с сокетом
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 			//связывает сокет со всеми доступными интерфейсами
	const char *port = config->listen.c_str();
	servaddr.sin_port = htons(atoi(port)); 					//port

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
		int ret = poll(pfd_array, 1 + MAX_CLIENTS, -1); //-1 (infinity) или значние TIMEOUT
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







int Server::request(struct pollfd *pfd_array, int &clients_count, int &i)
{

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
			} else if (ret > 0)
			{
				buf[ret] = '\0';
				std::cout << WHITE_B << ret << WHITE << " bytes received from client " << WHITE_B << i << RESET << std::endl;
				std::cout << GREEN << buf << RESET << std::endl;


				/*
				** ЗАПУСК ОБРАБОТЧИКА ЗАПРОСА!
				*/

				/* std::map requestHeaders= */ RequestParser parseHTTP((char*)buf);

				// std::string get("GET");
				// std::string post("POST");

				// if (get.compare(parseHTTP.getMetod()) == 0)
				// 	response(&(*pfd_array), i, 1);

				// if (post.compare(parseHTTP.getMetod()) == 0)
				// 	response(&(*pfd_array), i, 2);



				// std::map<std::string, std::string> m = parseHTTP.getHeaders();
				// std::cout << parseHTTP.getMetod() << " " << parseHTTP.getPath() << '\n';
				// for (std::map<std::string, std::string>::iterator it = m.begin(); it != m.end() ; it++)
				// {
				// 	std::cout << it->first << " : " << it->second << '\n';
				// }


				/*
				** ОТПРАВКА ОБРАБОТАННОГО ЗАПРОСА В RESPONSE - response(&(*pfd_array), i, MAP_with_values);
				*/




				std::stringstream response_body;////////////////////////////////////
				std::ifstream file; // создаем объект класса ifstream
				char *file_buffer = new char[1000 + 1]; file_buffer[1000] = 0;    //поменять!


				file.open("www/site.com/index.html"); 	//пытаемся открыть файл по запросу

				if (!file) 								//нужного контента нету
				{
					std::ifstream error_404;
					file.open("www/default/404.html");
					if (!file)
					{
						std::cout << YELLOW << "error: content not found!" << RESET << std::endl;
					}
					else
					{
						error_404.read(file_buffer, 300);
						response_body << file_buffer;
					}
					return -1;
				}
				else									//контейнт найден
				{
					std::cout << std::endl << GREEN_B << "OK: " << WHITE <<"response will be send to client" << RESET << std::endl << std::endl;
					// std::string file_buffer;
					// char *file_buffer = new char[1000 + 1]; file_buffer[1000] = 0;
					// response_body << file;
					file.read(file_buffer, 300);
					// for(file >> file_buffer; !file.eof(); file >> file_buffer)
					// 	std::cout << file_buffer;
					response_body << file_buffer;
				// }


					std::stringstream response;
					// if (flag == 1)
						response << "HTTP/1.1 200 OK\r\n";
					// if (flag == 2)
					// 	response << "HTTP/1.1 201 OK\r\n";

					response << "Version: HTTP/1.1\r\n"
					<< "Content-Type: text/html; charset=utf-8\r\n"
					<< "Content-Length: " << response_body.str().length()
					<< "\r\n\r\n"
					<< response_body.str();

					if(-1 == send(pfd_array[1 + i].fd, response.str().c_str(), response.str().length(), 0))
					{
						printf("Error on call 'send': %s\n", strerror(errno));
						return -1;
					}
					}
					return 0;
			}
		}
	}

	return 0;
}






















/*-------------------------------------------------------------------------------*/







