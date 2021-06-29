
#ifndef SOCKET_HPP
# define SOCKET_HPP
# include <iostream>
# include <netinet/in.h>
# include "structure.h"
# include "main.hpp"
class Socket
{
public:
	Socket();
	Socket(t_config *config);
	Socket(const Socket &copy);
	~Socket();
	Socket &operator=(const Socket &copy);

private:

	int					_fd;
	int					_port;
	struct sockaddr_in	_sock_addr;

	/*private methods*/
	void	_setSocketFlags(void);
};

#endif
