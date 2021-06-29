
#include "Socket.hpp"

Socket::Socket()
{
}

Socket::~Socket()
{
}

Socket::Socket(const Socket &copy)
{
}

Socket	&Socket::operator=(const Socket &copy)
{
	return (*this);
}

Socket::Socket(t_config *config)
{
	_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->_fd == -1)
		throw Exceptions();
	_setSocketFlags();
}

void Socket::_setSocketFlags(void)
{
	if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1)
		throw Exceptions();

	int opt = 1;
	int opt1 = 1;
	int opt2 = 65536;

	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
		throw Exceptions();
	if (setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &opt1, sizeof(int)) < 0)
		throw Exceptions();
	if (setsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &opt2, sizeof(int)) < 0)
		throw Exceptions();
}
