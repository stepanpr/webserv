#include "Connection.hpp"

Connection::Connection():
		_sock(NULL), _config(NULL), _state(READING), _sock_fd(-1),
		_response(), _isOK(0), _request()
{
}

Connection::Connection(Socket *sock, t_config *config):
	_sock(sock), _config(config), _state(READING), _sock_fd(sock->getFd()),
	_response(), _isOK(0), _request()
{
}

Connection::~Connection()
{
	delete _sock;
}

Connection::Connection(const Connection &copy)
{
	*this = copy;
}

Connection	&Connection::operator=(const Connection &copy)
{
	_request = copy._request;
	_response = copy._response;
	_state = copy._state;
	_sock_fd  = copy._sock_fd;
	_isOK = copy._isOK;
	_sock = copy._sock;
	_config = copy._config;
	return (*this);
}

void Connection::bufHandler(char *buf, int	len)
{
	_isOK = _request.RequestWaiter(buf, len);
}

std::string Connection::responsePrepare()
{

	Response response(_request, _config);
	// std::string responseString = response.responseInit(_request, _config);
	_response = response.responseInit();

	return _response;
}




int Connection::get_isOk()
{
	return _isOK;
}

//void Connection::setConfig(struct  s_config &config)
//{
//	this->_config = &config;
//	// std::cout << config.listen << '\n';
//
//}

int Connection::getState() const
{
	return _state;
}

void Connection::setState(int state)
{
	_state = state;
}

int Connection::getSockFd() const
{
	return _sock_fd;
}

const std::string &Connection::getResponse() const
{
	return _response;
}

void Connection::setResponse(const std::string &response)
{
	_response = response;
}
