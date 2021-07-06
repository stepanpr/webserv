#ifndef CONNECTION_HPP
# define CONNECTION_HPP
# include <iostream>
# include <map>
# include "RequestParser.hpp"
# include "Socket.hpp"
# include "Response.hpp"
# include "structure.h"

class Socket;
class Response;
class Connection
{
public:
	Connection();
	Connection(Socket *sock, t_config *_config);

	Connection(const Connection &copy);

	~Connection();
	Connection &operator=(const Connection &copy);

	int getState() const;

	int getSockFd() const;

	void setState(int state);

	const std::string &getResponse() const;

	void setResponse(const std::string &response);

	/*
	 * for Arannara
	 */
	void	bufHandler(char *buf, int	len); 	// эта функция принимает char *buf и длину buf, считанного в данной итерации
											// добавляет buf к _response и

	std::string	responsePrepare();


	int get_isOk();

private:
	RequestParser		_request;
	// Response			*_response;
	std::string			_response;
	int					_state;
	int					_sock_fd;
	int 				_isOK;
	Socket				*_sock;
	struct s_config		*_config;
};

#endif
