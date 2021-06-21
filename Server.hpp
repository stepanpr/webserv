#ifndef SERVER_HPP
# define SERVER_HPP
# include "Global.hpp"

class Config;

class Server
{
public:
	Server();
	Server(const Server &copy);
	~Server();
	Server &operator=(const Server &copy);

	void startServer(struct s_config &config, int id);

private:
};

#endif
