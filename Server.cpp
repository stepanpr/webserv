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
	this->_listen_sock_fd = copy._listen_sock_fd;
	this->_servaddr.sin_addr = copy._servaddr.sin_addr;
	this->_servaddr.sin_family = copy._servaddr.sin_family;
	this->_servaddr.sin_port = copy._servaddr.sin_port;
	return (*this);
}


/*-------------------------------------------------------------------------------*/





int Server::startServer(struct s_config *config)
{


	pthread_mutex_lock(&lock);
	std::cout << std::endl << GREEN_B << "Server started! " << WHITE <<"(ID: "
	<< config->serverID << "; port: " << config->listen << ")" << RESET << std::endl;
	pthread_mutex_unlock(&lock);



	_listen_sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//создание сокета от которого мы будем ожидать входящих данных
	if (this->_listen_sock_fd == -1)
		throw Exceptions();
	if (fcntl(_listen_sock_fd, F_SETFL, O_NONBLOCK) == -1)
		throw Exceptions();


	opt = 1;
	int opt1 = 1;
	int opt2 = 65536;

	if (setsockopt(_listen_sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
		throw Exceptions();
	if (setsockopt(_listen_sock_fd, SOL_SOCKET, SO_KEEPALIVE, &opt1, sizeof(int)) < 0)
		throw Exceptions();
	if (setsockopt(_listen_sock_fd, SOL_SOCKET, SO_RCVBUF, &opt2, sizeof(int)) < 0)
		throw Exceptions();

	// _servaddr = {0};											//заполняем структуру sockaddr_in
	_servaddr.sin_family = AF_INET;							// AF_INET определяет, что используется сеть для работы с сокетом
	_servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 			//связывает сокет со всеми доступными интерфейсами
	const char *port = config->listen.c_str();
	_servaddr.sin_port = htons(atoi(port)); 					//port

	if (bind(_listen_sock_fd, (struct sockaddr*) &_servaddr, sizeof(_servaddr)) < 0) //Bind: Привязка сокета к адресу
		throw Exceptions();

	if (listen(_listen_sock_fd, SOMAXCONN) == -1) //1		//Listen: Подготовка сокета к принятию входящих соединений, SOMAXCONN - максимально возможное число одновременных TCP-соединений, можно указать нужное число
	{
		throw Exceptions();
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
				.fd = _listen_sock_fd,
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
			std::cout << YELLOW_B << " ! " << YELLOW << "error on call \'poll\': " << WHITE << strerror(errno) << std::endl;
			// printf("Error on call 'poll': %s", strerror(errno));
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

				// struct sockaddr_in _cliaddr;
				socklen_t clilen = sizeof(struct sockaddr_in);
				int sock_fd = accept(_listen_sock_fd, (struct sockaddr*)&_cliaddr, &clilen);	//Accept: Ожидание входящего соединения
				if (sock_fd == -1)
					throw Exceptions();
				else if (sock_fd > 0)
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
						if (fcntl(sock_fd, F_SETFL, O_NONBLOCK) == -1)
							throw Exceptions();
						for (i = 0; i < MAX_CLIENTS; i++)
						{
							if (pfd_array[1 + i].fd == -1)
							{
								pfd_array[1 + i].fd = sock_fd;
								pfd_array[1 + i].events = POLLIN;

								std::cout << WHITE <<"!client " << WHITE_B << i << WHITE << " has been connected from " << WHITE_B
								<< inet_ntoa((in_addr)_cliaddr.sin_addr) << ":" << config.listen << WHITE << " | clients total: "
								<< WHITE_B << clients_count << RESET << std::endl;
								// std::cout << "!got connection from " << inet_ntoa((in_addr)_cliaddr.sin_addr) << std::endl;
								break;
							}
						}
					}
				}

			}
			request(&(*pfd_array), clients_count, i, config);

		}
	}

}






int Server::request(struct pollfd *pfd_array, int &clients_count, int &i, struct s_config &config)
{
Connection tempConnect = _mapConnection.find(pfd_array[1 + i].fd)->second;

	for (i = 0; i < MAX_CLIENTS; i++)
	{
		if (pfd_array[1 + i].fd != -1 && (pfd_array[1 + i].revents & POLLIN) != 0)
		{
			pfd_array[1 + i].revents &= ~POLLIN;




			/* !!!NEW_VERSION
			** Connection temp = _mapConnection.find(pfd_array[1 + i].fd)->second;
			*/
			
			tempConnect.setConfig(config);
			


			uint8_t buf[1024];
			int ret = recv(pfd_array[1 + i].fd, buf, 1024, 0); //read (pfd_array[1 + i].fd , buf, 1024); /* Возврат из функции recv происходит, когда модуль TCP решает передать процессу полученные от клиента данные. Данные возвращается в буфере buf, размер которого передается в третьем аргументе. В четвертом аргументе могут передаваться дополнительные опциипараметры. Функция возвращает число байтов, которые модуль TCP записал в буфер buf; если функция возвращает ноль, то клиент данных для передачи больше не имеет.*/

			if (ret < 0)
			{
				// printf("Error on call 'recv': %s\n", strerror(errno));
        		// if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) //некритические
            	// 	continue;
				// if (errno == ECONNRESET) //фатальная
				// 	std::cout << YELLOW_B << " ! " << YELLOW << "error on call \'recv\': " << WHITE << strerror(errno) << std::endl;
				// // return -1;
				// close(pfd_array[1 + i].fd);        //https://stackoverflow.com/questions/24916937/how-to-catch-a-connection-reset-by-peer-error-in-c-socket
				// break ;
				// continue;
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


				/* !!!NEW_VERSION
				**
				**
				** arranara:
				** temp.bufAnalize((char*)buf, ret);
				**
				**
				** emabel:
				** if _request.isOk => connection.state = WRITE
				** connection.makeResponse
				*/
				tempConnect.bufHandler((char*)buf, ret);
				if (tempConnect.get_isOk())
					responseSend(tempConnect.responsePrepare(), &(*pfd_array), i);

			}
		}
	}


	return 0;
}



int Server::responseSend(std::string response, struct pollfd *pfd_array, int &i)
{
	// std::cout << response << '\n';
	if(-1 == send(pfd_array[1 + i].fd, response.c_str(), response.length(), 0))
		throw Exceptions();
	return 0;
}



/*-------------------------------------------------------------------------------*/







