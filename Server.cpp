#include "Server.hpp"
#include "Config.hpp"


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

Server::Server(): _mapConnections(), _config(NULL), _clientsCount(0), _listenSock(NULL)
{
	_initPollfdStruct();
}

Server::~Server()
{
	delete _listenSock;
}

Server::Server(const Server &copy)
{
	*this = copy;
}

Server	&Server::operator=(const Server &copy)
{
	this->_listenSock = copy._listenSock;
	this->_config = copy._config;
	this->_mapConnections = copy._mapConnections;
	_copyPollfdStruct(const_cast<pollfd *>(copy._fd_array));
	return (*this);
}


/*-------------------------------------------------------------------------------*/





int Server::startServer(t_config *config)
{
	this->_config = config;

	_createListenSocket();

	_pollLoop();

	std::cout << "EXIT SERV\n";
	return 0;
}

int Server::_pollLoop()
{
	_fd_array[0].fd = _listenSock->getFd();

	while (1)
	{
		/*
		** int poll(struct pollfd fds[], nfds_t nfds, int timeout)
		** pollfd fds[] - используется для хранения файлового дескриптора сокета, статус которого необходимо проверить;
		** nfds - параметр типа nfds_t, используемый для обозначения бщее количества элементов (клиентов);
		** timeout - время блокировки (в миллисекундах)
		*/
		int ret = poll(_fd_array, 1 + MAX_CLIENTS, -1); //-1 (infinity) или значние TIMEOUT
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
			_checkNewConnection(ret);
		}
		// поиск всех событий
		for (int i = 0; i < MAX_CLIENTS && ret > 0; i++)
		{
			struct pollfd pfd = _fd_array[i + 1];
			if (pfd.fd != -1 && pfd.revents)
			{
				ret--;
				Connection *tempConnect = _mapConnections.find(pfd.fd)->second;
				if ((pfd.revents & POLLERR) || (pfd.revents & POLLHUP) || (pfd.revents & POLLNVAL))
				{
					_removeConnection(tempConnect, i + 1);
					continue ;
				}
				if (tempConnect->getState() == READING)
				{
					int request_status = this->request(*tempConnect, i + 1);
					if (request_status == ERROR)
						_removeConnection(tempConnect, i + 1);
					else if (request_status == FULL)
					{
						tempConnect->responsePrepare();
						tempConnect->setState(WRITING);
						_fd_array[i + 1].events = POLLOUT;
					}
				}
				else if (tempConnect->getState() == WRITING)
				{
					int response_status = this->responseSend(*tempConnect);
					if (response_status == FULL)
						_removeConnection(tempConnect, i + 1);
				}
			}
		}
	}
}

int Server::request(Connection &conn, int i)
{
	uint8_t buf[1024];
	memset(buf, 0, 1024);
	int ret = recv(conn.getSockFd(), buf, 1024, 0);/* Возврат из функции recv происходит, когда модуль TCP решает передать процессу полученные от клиента данные. Данные возвращается в буфере buf, размер которого передается в третьем аргументе. В четвертом аргументе могут передаваться дополнительные опциипараметры. Функция возвращает число байтов, которые модуль TCP записал в буфер buf; если функция возвращает ноль, то клиент данных для передачи больше не имеет.*/

	if (ret < 0)
		throw Exceptions();

	else if (ret == 0)
		return ERROR;
	else if (ret > 0)
	{
		std::cout << WHITE_B << ret << WHITE << " bytes received from client " << WHITE_B << i << RESET << std::endl;
		std::cout << GREEN << buf << RESET << std::endl;
		conn.bufHandler((char*)buf, ret);
		if (conn.get_isOk())
			return FULL;
	}
	return WAIT;
}

//int Server::request(struct pollfd *pfd_array, int &clients_count, int &i, struct s_config &config)
//{
//	for (i = 0; i < MAX_CLIENTS; i++)
//	{
//		if (pfd_array[1 + i].fd != -1 && (pfd_array[1 + i].revents & POLLIN) != 0)
//		{
//			pfd_array[1 + i].revents &= ~POLLIN;
//
//
//
//
//			/* !!!NEW_VERSION
//			** Connection temp = _mapConnection.find(pfd_array[1 + i].fd)->second;
//			*/
//			Connection *tempConnect = _mapConnections.find(pfd_array[1 + i].fd)->second;
//			tempConnect->setConfig(config);
//
//
//
//			uint8_t buf[1024];
//			int ret = recv(pfd_array[1 + i].fd, buf, 1024, 0); //read (pfd_array[1 + i].fd , buf, 1024); /* Возврат из функции recv происходит, когда модуль TCP решает передать процессу полученные от клиента данные. Данные возвращается в буфере buf, размер которого передается в третьем аргументе. В четвертом аргументе могут передаваться дополнительные опциипараметры. Функция возвращает число байтов, которые модуль TCP записал в буфер buf; если функция возвращает ноль, то клиент данных для передачи больше не имеет.*/
//
//
//			if (ret < 0)
//			{
//				// printf("Error on call 'recv': %s\n", strerror(errno));
//        		// if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) //некритические
//            	// 	continue;
//				// if (errno == ECONNRESET) //фатальная
//				// 	std::cout << YELLOW_B << " ! " << YELLOW << "error on call \'recv\': " << WHITE << strerror(errno) << std::endl;
//				// // return -1;
//				// close(pfd_array[1 + i].fd);        //https://stackoverflow.com/questions/24916937/how-to-catch-a-connection-reset-by-peer-error-in-c-socket
//				// break ;
//				// continue;
//			} else if (ret == 0)
//			{
//				close(pfd_array[1 + i].fd);
//				pfd_array[1 + i].fd = -1;
//				pfd_array[1 + i].events = 0;
//				clients_count--;
//				std::cout << WHITE <<"Client " WHITE_B << i << WHITE << " has been disconnected | clients total: " << WHITE_B << clients_count << RESET << std::endl;
//			} else if (ret > 0)
//			{
//				buf[ret] = '\0';
//
//				std::cout << WHITE_B << ret << WHITE << " bytes received from client " << WHITE_B << i << RESET << std::endl;
//				std::cout << GREEN << buf << RESET << std::endl;
//
//
//				/* !!!NEW_VERSION
//				**
//				**
//				** arranara:
//				** temp.bufAnalize((char*)buf, ret);
//				**
//				**
//				** emabel:
//				** if _request.isOk => connection.state = WRITE
//				** connection.makeResponse
//				*/
//				tempConnect.bufHandler((char*)buf, ret);
//				if (tempConnect.get_isOk())
//					responseSend(tempConnect.responsePrepare(), &(*pfd_array), i);
//
//			}
//		}
//	}

//
//	return 0;
//}



//int Server::responseSend(std::string response, struct pollfd *pfd_array, int &i)
//{
//	// std::cout << response << '\n';
//	if(-1 == send(pfd_array[1 + i].fd, response.c_str(), response.length(), 0))
//		throw Exceptions();
//	return 0;
//}

int Server::responseSend(Connection &conn)
{
	int sended;

	std::string response = conn.getResponse();
//	std::cout << RED << response << RESET << std::endl;
	sended = send(conn.getSockFd(), response.c_str(), response.length(), 0);
//	std::cout << BLUE << response.length() << RESET << std::endl;
//	std::cout << GREEN << sended << RESET << std::endl;
	if (sended < 0)
		throw Exceptions();
	if (sended < response.length())
	{
		conn.setResponse(response.substr(sended, response.length()));
		return WAIT;
	}
	else
		return FULL;
}

/*-------------------------------------------------------------------------------*/

void Server::_createListenSocket()
{
	_listenSock = new Socket(_config);
	_listenSock->setSockNonblock();
	_listenSock->setSockReuseaddr();
	_listenSock->setSocketFlags();
	if (_listenSock->bind() < 0)
	{
		std::cout << RED_B << "Port for listen already in use" << RESET << std::endl;
		exit(1);
	}
	if (_listenSock->listen() < 0)
		throw Exceptions();
	pthread_mutex_lock(&lock);
	std::cout << std::endl << GREEN_B << "Server started! " << WHITE <<"(ID: "
			  << _config->serverID << "; port: " << _config->listen << ")" << RESET << std::endl;
	pthread_mutex_unlock(&lock);
}

void Server::_initPollfdStruct()
{
	/*
	**	struct pollfd {
	**   	int fd;			- описатель файла
	**  	short events;	- запрошенные события
	**		short revents;	- возвращенные события
	**	};
	**	https://www.opennet.ru/man.shtml?category=2&topic=poll
	*/

	for (int i = 0; i < MAX_CLIENTS + 1; i++)
	{
		_fd_array[i].fd = -1;
		_fd_array[i].events = POLLIN;
		_fd_array[i].revents = 0;
	}
}

void Server::_copyPollfdStruct(struct pollfd *array)
{
	for (int i = 0; i < MAX_CLIENTS + 1; i++)
	{
		_fd_array[i].fd = array[i].fd;
		_fd_array[i].events = array[i].events;
		_fd_array[i].revents = array[i].revents;
	}
}

void Server::_addSocketToConnections(Socket *newSocket)
{
	_addToPollfd(newSocket->getFd());
	_addToMap(newSocket);
}

void Server::_addToPollfd(int new_fd)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (_fd_array[i + 1].fd == -1)
		{
			_fd_array[i + 1].fd = new_fd;
			_fd_array[i + 1].events = POLLIN;
			break ;
		}
	}
}

void Server::_addToMap(Socket *newSocket)
{
	Connection *newConnect = new Connection(newSocket, _config);
	_mapConnections.insert(std::pair<int, Connection *>(newSocket->getFd(), newConnect));
	std::cout << WHITE <<"!client " << WHITE << " has been connected from " << WHITE_B
			  << inet_ntoa((in_addr)newSocket->getSockAddr().sin_addr) << ":" << _config->listen << WHITE
			  << " | clients total: " << WHITE_B << _clientsCount << RESET << std::endl;
}

void Server::_checkNewConnection(int &ret)
{
	if ((_fd_array[0].revents & POLLIN) != 0)
	{
		ret--;
		_fd_array[0].revents &= ~POLLIN;
		Socket *temp = _listenSock->accept();
		if (_clientsCount == MAX_CLIENTS)
		{
			delete temp;
			std::cout << WHITE_B << MAX_CLIENTS;
			std::cout << WHITE << " clients already connected, unexpected new connection have discarded" << RESET;
			std::cout << std::endl;
		}
		else
		{
			_clientsCount++;
			_addSocketToConnections(temp);
		}
	}
}

void Server::_removeConnection(Connection *conn, int i)
{
	_mapConnections.erase(_fd_array[i].fd);
	delete conn;
	std::cout << WHITE <<"Client " WHITE_B << i << WHITE << " has been disconnected | clients total: " << WHITE_B << _clientsCount << RESET << std::endl;
	_clientsCount--;
	_fd_array[i].fd = -1;
	_fd_array[i].events = POLLIN;
	_fd_array[i].revents = 0;
}
