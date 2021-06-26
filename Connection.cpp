
#include "Connection.hpp"

Connection::Connection()
{
}

Connection::~Connection()
{
}

Connection::Connection(const Connection &copy)
{
}

Connection	&Connection::operator=(const Connection &copy)
{
	return (*this);
}




void Connection::bufHandler(char *buf, int	len)
{

	_isOK = _request.RequestWaiter(buf, len);

	

}





std::string Connection::responsePrepare()
{

	std::string response = _response.init(_request)


	return request;
}




int Connection::get_isOk()
{
	return _isOK;
}
