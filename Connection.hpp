#ifndef CONNECTION_HPP
# define CONNECTION_HPP
# include <iostream>
# include <map>
# include "RequestParser.hpp"
# include "Socket.hpp"
# include "Response.hpp"


class Response;
class Connection
{
public:
	Connection();
	Connection(Socket *sock);

	Connection(const Connection &copy);

	~Connection();
	Connection &operator=(const Connection &copy);

	/*
	 * for Arannara
	 */
	void	bufHandler(char *buf, int	len); 	// эта функция принимает char *buf и длину buf, считанного в данной итерации
											// добавляет buf к _response и

	std::string	responsePrepare();


	int get_isOk();
	void setConfig(struct  s_config &s_config);

private:
	RequestParser		_request;
	// Response			*_response;
	int					_state;
	int					_sock_fd;
	int 	_isOK;
	Socket				*_sock;
	struct s_config			*_config;
};

#endif
