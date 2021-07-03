
#include "Socket.hpp"

Socket::Socket()
{
}

Socket::~Socket()
{
	close(_fd);
}

Socket::Socket(const Socket &copy)
{
	*this = copy;
}

Socket	&Socket::operator=(const Socket &copy)
{
	this->_fd = copy._fd;
	this->_port = copy._port;
	_copySockaddr(copy._sock_addr);
	return (*this);
}

Socket::Socket(t_config *config)
{
	_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->_fd == -1)
		throw Exceptions();
	_setSockaddr(config->listen);
}

int Socket::bind()
{
	return ::bind(_fd, (struct sockaddr *) &_sock_addr, sizeof(_sock_addr));
}

int Socket::listen()
{
	return ::listen(_fd, SOMAXCONN);
}

Socket *Socket::accept()
{
	Socket *connection = new Socket;
	socklen_t cliLen = sizeof(struct sockaddr_in);
	connection->_fd = ::accept(this->_fd, (struct sockaddr *) &connection->_sock_addr, &cliLen);
	if (connection->_fd == -1)
	{
		delete connection;
		throw Exceptions();
	}
	return connection;
}

void Socket::setSockNonblock()
{
	if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1)
		throw Exceptions();
}

void Socket::setSockReuseaddr()
{
	int opt = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
		throw Exceptions();
}

void Socket::setSocketFlags()
{
	int opt1 = 1;
	int opt2 = 65536;
	if (setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &opt1, sizeof(int)) < 0)
		throw Exceptions();
	if (setsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &opt2, sizeof(int)) < 0)
		throw Exceptions();

}

void Socket::_setSockaddr(std::string listen)
{
	_sock_addr.sin_family = AF_INET;
	_port = atoi(listen.c_str());
	_sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_sock_addr.sin_port = htons(_port);
}

void Socket::_copySockaddr(struct sockaddr_in addr)
{
	_sock_addr.sin_port = addr.sin_port;
	_sock_addr.sin_family = addr.sin_family;
	_sock_addr.sin_addr.s_addr = addr.sin_addr.s_addr;
}

int Socket::getFd() const
{
	return _fd;
}

const sockaddr_in &Socket::getSockAddr() const
{
	return _sock_addr;
}
