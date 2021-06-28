#include "RequestParser.hpp"

int main()
{
	std::stringstream zapros, zapros_chunked, zapros_length, zapros_length2;

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
			<< "9\r\n"
			<< "Developer\r\n"
			<< "7\r\n"
			<< "Network\r\n"
			<< "0\r\n"
			<< "\r\n"
			<< std::endl;
	// std::cout << zapros.str() << std::endl;


	zapros_length	<< "POST /favicon.ico HTTP/1.1\r\n"
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
			<< "Content-Length: 700\r\n\r\n"
			<< "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
			<< "2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222"
			<< "3333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333"
			<< "4444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444";

		zapros_length2	<< "5555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555"
			<< "6666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666"
			<< "7777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777\r\n"
			<< "\r\n"
			<< std::endl;

	char str[] = {"G"};
	char str2[] = {"ET / HT"};
	char str3[] = {"TP/1.1\r\nHost: localho"};
	char str4[] = {"st:5006\r\nConnection: keep-alive\r\n"};
	char str5[] = {"Cache-Control: max-age=0\r\n"};
	char str6[] = {"Accept-Language: en-US,en;q=0.9\r\n\r\n"};

	int digit;
	int len = strlen(zapros_length.str().c_str());
	int len2 = strlen(zapros_length2.str().c_str());
	RequestParser a;
	digit = a.RequestWaiter(zapros_length.str().c_str(), len);
	digit = a.RequestWaiter(zapros_length2.str().c_str(), len2);
	std::cout << "result: " << digit;

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