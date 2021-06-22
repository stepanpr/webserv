
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
	void PrintMap();

	std::string getMetod();
	std::string getPath();
	std::string getProtokol();
	std::map<std::string,std::string> getHeaders();


	// RequestParser(const RequestParser &copy);
	~RequestParser();
	// RequestParser &operator=(const RequestParser &copy);

private:
	std::string _metod;
	std::string _path;
	std::string _protokol;
	std::map<std::string,std::string> _headers;
};

#endif