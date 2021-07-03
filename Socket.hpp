
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
	Socket(t_config *config); //constructor for listen socket
	Socket(const Socket &copy);
	~Socket();
	Socket &operator=(const Socket &copy);

	int		bind();
	int		listen();
	Socket	*accept();
	void	setSockNonblock();
	void	setSockReuseaddr();
	void	setSocketFlags();

	int getFd() const;
	const sockaddr_in &getSockAddr() const;

private:

	int					_fd;
	int					_port;
	struct sockaddr_in	_sock_addr;

	/*private methods*/

	void	_setSockaddr(std::string listen);
	void	_copySockaddr(struct sockaddr_in addr);
};

#endif
