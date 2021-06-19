#include "RequestParser.hpp"

RequestParser::RequestParser(void)
{ }


RequestParser::RequestParser(std::string buf)
{
	// std::string buf2 = static_cast<std::string>(buf);

	size_t pos = 0;
	std::string token;
	std::string separator = "\n\r";

	while ( (pos = buf.find(separator)) != std::string::npos)
	{
		token = buf.substr(0, pos);

		std::cout << token << std::endl;
		buf.erase(0, pos + separator.length());
	}
}

// RequestParser::RequestParser(const RequestParser &copy)
// {
// }

// RequestParser	&RequestParser::operator=(const RequestParser &copy)
// {
// 	return (*this);
// }


RequestParser::~RequestParser()
{ }