#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP
# include <iostream>
#include <sstream>

class RequestParser
{

public:
	RequestParser();
	RequestParser(std::string buf);
	// RequestParser(const RequestParser &copy);
	~RequestParser();
	// RequestParser &operator=(const RequestParser &copy);

private:

};

#endif
