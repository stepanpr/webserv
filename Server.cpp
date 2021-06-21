#include "Server.hpp"
#include "Config.hpp"

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








/* START SERVER WITH POLL */
void Server::startServer(struct s_config &config, int id)
{
	std::cout << std::endl << "Server start! (ID: " << id << ")" << std::endl;


	std::cout << "listen: " << config.listen << std::endl;
	std::cout << "listen: " << config.server_name << std::endl;
	std::cout << "listen: " << config.error_page << std::endl;
	std::cout << "listen: " << config.location[0].index << std::endl;




	//SERVER_CODE





}



