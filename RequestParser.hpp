#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP
# include <iostream>
#include <sstream>
#include <map>

class RequestParser
{

public:
	RequestParser();
	RequestParser(std::string);
	void addRequest(std::string);
	void RequestWaiter(char *);
	void PrintMap();

	std::string getMetod();
	std::string getPath();
	std::string getProtokol();
	std::map<std::string,std::string> getHeaders();

	// RequestParser(const RequestParser &copy);
	~RequestParser();
	// RequestParser &operator=(const RequestParser &copy);

private:
	bool _is_ok;
	std::string _metod;
	std::string _path;
	std::string _protokol;
	std::stringstream _bodybuffer;
	std::map<std::string,std::string> _headers;
};

#endif
