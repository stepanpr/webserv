
#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include <iostream>
# include "main.hpp"

class Response
{
public:
	Response();
	Response(const Response &copy);
	~Response();
	Response &operator=(const Response &copy);

	std::string responseInit(RequestParser &HTTPrequest, struct s_config *config);

private:
	// _finalResponse;

	

};

#endif
