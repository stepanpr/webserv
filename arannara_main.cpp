#include "RequestParser.hpp"

int main()
{
	std::stringstream zapros, zapros_chunked, zapros_chunked2, zapros_chunked3, zapros_chunked4;

	zapros	<< "GET / HTTP/1.1\r\n"
			<< "Host: localhost:5006\r\n"
			<< "Connection: keep-alive\r\n"
			<< "Cache-Control: max-age=0\r\n"
			<< "sec-ch-ua: \" Not;A Brand\";v=\"99\", \"Google Chrome\";v=\"91\", \"Chromium\";v=\"91\"\r\n"
			<< "sec-ch-ua-mobile: ?0\r\n"
			<< "Upgrade-Insecure-Requests: 1\r\n"
			<< "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.101 Safari/537.36\r\n"
			<< "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
			<< "Sec-Fetch-Site: none\r\n"
			<< "Sec-Fetch-Mode: navigate\r\n"
			<< "Sec-Fetch-User: ?1\r\n"
			<< "Sec-Fetch-Dest: document\r\n"
			<< "Accept-Encoding: gzip, deflate, br\r\n"
			<< "Accept-Language: en-US,en;q=0.9\r\n\r\n"
			<< std::endl;

	zapros_chunked	<< "GET /favicon.ico HTTP/1.1\r\n"
			<< "Host: localhost:5006\r\n"
			<< "Connection: keep-alive\r\n"
			<< "Cache-Control: max-age=0\r\n"
			<< "sec-ch-ua: \" Not;A Brand\";v=\"99\", \"Google Chrome\";v=\"91\", \"Chromium\";v=\"91\"\r\n"
			<< "sec-ch-ua-mobile: ?0\r\n"
			<< "Upgrade-Insecure-Requests: 1\r\n"
			<< "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.101 Safari/537.36\r\n"
			<< "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
			<< "Sec-Fetch-Site: none\r\n"
			<< "Sec-Fetch-Mode: navigate\r\n"
			<< "Sec-Fetch-User: ?1\r\n"
			<< "Sec-Fetch-Dest: document\r\n"
			<< "Accept-Encoding: gzip, deflate, br\r\n"
			<< "Accept-Language: en-US,en;q=0.9\r\n"
			<< "Transfer-Encoding: chunked\r\n\r\n"
			<< "7\r\n"
			<< "Mozilla\r\n" << std::endl;

	zapros_chunked2	<< "9\r\n"
			<< "Developer\r\n"
			<< std::endl;

	zapros_chunked3	<< "7\r\n"
			<< "Network\r\n"
			<< std::endl;

	zapros_chunked4	<< "0\r\n"
			<< "\r\n"
			<< std::endl;
	// std::cout << zapros.str() << std::endl;



	RequestParser a(zapros.str());
	a.PrintMap();

}