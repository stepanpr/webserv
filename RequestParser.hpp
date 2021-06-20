#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP
# include <iostream>
#include <sstream>
#include <map>

class RequestParser
{

public:
	RequestParser();
	RequestParser(std::string buf);
	// RequestParser(const RequestParser &copy);
	~RequestParser();
	// RequestParser &operator=(const RequestParser &copy);

private:
	std::string _metod;
	std::string _path;
	std::string _protokol;

};

#endif
