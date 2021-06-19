#ifndef SERVER_HPP
# define SERVER_HPP
# include <iostream>

class Server
{
public:
	Server();
	Server(const Server &copy);
	~Server();
	Server &operator=(const Server &copy);

private:
};

#endif
