
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

	Response response(_request, _config);
	// std::string responseString = response.responseInit(_request, _config);
	std::string responseString = response.responseInit();

	return responseString;
}




int Connection::get_isOk()
{
	return _isOK;
}

void Connection::setConfig(struct  s_config &config)
{
	this->_config = &config;
	// std::cout << config.listen << '\n';

}