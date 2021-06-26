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
			<< "Mozilla\r\n"
			<< "0\r\n"
			<< "\r\n" << std::endl;

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


	// char str[] = {"G"};
	// char str2[] = {"ET / HT"};
	// char str3[] = {"TP/1.1\r\nHost: localho"};
	// char str4[] = {"st:5006\r\nConnection: keep-alive\r\n"};
	// char str5[] = {"Cache-Control: max-age=0\r\n"};
	// char str6[] = {"Accept-Language: en-US,en;q=0.9\r\n\r\n"};

	int digit;
	int len = strlen(zapros_chunked.str().c_str());
	std::cout << len << '\n' ;

	// std::cout << zapros_chunked.str().c_str() << std::endl;
	RequestParser a;
	digit = a.RequestWaiter(zapros_chunked.str().c_str(), len);
	std::cout << digit << '\n';

	// a.RequestWaiter(str2);
	// a.RequestWaiter(str3);
	// a.RequestWaiter(str4);
	// a.RequestWaiter(str5);
	// a.RequestWaiter(str6);

	// RequestParser a(zapros_chunked.str());
	// a.addRequest(zapros_chunked2.str());
	// a.addRequest(zapros_chunked3.str());
	// a.addRequest(zapros_chunked4.str());
	// a.PrintMap();

}