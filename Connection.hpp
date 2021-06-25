
#ifndef CONNECTION_HPP
# define CONNECTION_HPP
# include <iostream>
# include <map>
# include "RequestParser.hpp"
# include "Response.hpp"

class Connection
{
public:
	Connection();
	Connection(const Connection &copy);
	~Connection();
	Connection &operator=(const Connection &copy);

	/*
	 * for Arannara
	 */
	void	bufAnalize(char *buf, int	len); 	// эта функция принимает char *buf и длину buf, считанного в данной итерации
											// добавляет buf к _response и

private:
	RequestParser		_request;
	Response			_response;
	int					_state;
	int					_sock_fd;
};

#endif
