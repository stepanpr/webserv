

#include "Cgi.hpp"


// Cgi::Cgi() {}

// Cgi::~Cgi()
// {         
// 	for (int i = 0; _varsArray[i]; ++i)
//         free(_varsArray[i]);
// }

// Cgi &Cgi::operator=(const Cgi &copy)
// {
// 	return *this;
// }

// Cgi::Cgi(const Cgi &cgi) { *this = cgi; }

Cgi::Cgi(std::string body, struct s_config *config, std::string pathToCgi, std::map<std::string, std::string> requestHeaders, std::string requestMethod) 
:  _config(config), _body(body), _pathToCgi(pathToCgi)
{
	std::cout << CYAN << "!CGI is working" << RESET << "\n";
	_varsArray = getVarsArray(setVariables(requestHeaders, requestMethod));
}


std::vector<std::string> Cgi::setVariables(std::map<std::string, std::string> requestHeaders, std::string requestMethod)
{
	// std::vector<std::string> env;
    // _vars.push_back("AUTH_TYPE=" + locations.auth_data.AuthType);
    _vars.push_back("CONTENT_LENGTH=" + toString(_body.size()));
	if (requestHeaders.find("Content-Type") != requestHeaders.end())
    	_vars.push_back("CONTENT_TYPE=" + requestHeaders.find("Content-Type")->second);
	else
		_vars.push_back("CONTENT_TYPE=");
    _vars.push_back("GATEWAY_INTERFACE=CGI/1.1");
    // _vars.push_back(getPathInfo("PATH_INFO=" + );
    // _vars.push_back("PATH_TRANSLATED=" + );
	// if (.find("query_string") != )
   	// 	_vars.push_back("QUERY_STRING=" + .find("query_string")->second);
	// else
    //     _vars.push_back("QUERY_STRING=");
    _vars.push_back("REMOTE_ADDR=" + _config->server_name);
    // _vars.push_back("REMOTE_IDENT=" + );
    // _vars.push_back("REMOTE_USER=" + );
	if (requestMethod.size() > 1)
    	_vars.push_back("REQUEST_METHOD=" + requestMethod);
	else
        _vars.push_back("REQUEST_METHOD=");
    // _vars.push_back("RЕQUEST_URI=" + );
    _vars.push_back("SERVER_NAME=" + _config->server_name);
    _vars.push_back("SERVER_PORT=" + _config->listen);
    _vars.push_back("SERVER_PROTOCOL=HTTP/1.1");
    _vars.push_back("SERVER_SOFTWARE=web_server");
	
	for (int i = 0; i < (int)_vars.size(); i++)
		std::cout << _vars[i] << std::endl;
	return _vars;
}

char **Cgi::getVarsArray(std::vector<std::string> vars) 
{
    char **pString = (char **) malloc(sizeof(char **) * (vars.size() + 1));
    std::vector<std::string>::const_iterator j = vars.begin();
    std::string::size_type i = 0;
    for (; i < vars.size(); ++i) {
        pString[i] = strdup(j->c_str());
        j++;
    }
    pString[i] = NULL;
    return pString;
}




void Cgi::launchCGI()
{
	//Формируем в глобальных переменных тело запроса и его длинну
	static const std::string strRequestBody = _body.c_str();

	static const std::string strRequestHeader = "Content-Length=" + std::to_string((long long)strRequestBody.length());
	//Формируем переменные окружения которые будут отосланы дочернему процессу
//	static const char *variables[4] = {strRequestHeader.c_str(), "VARIABLE2=erererer", "VARIABLE3=3", 0};

	//Формируем переменные командной строки для дочернего процесса. Первая переменная - путь к дочернему процессу.
	// static const char *pszChildProcessArgs[4] = {"./cgi_bin/mkcgi", "first argument", "second argument", 0};
	// static const char *pszChildProcessArgs[3] = {"/usr/bin/perl", "./cgi_bin/perl1.cgi", 0};
	static const char *pszChildProcessArgs[3] = {"/usr/bin/python", "./cgi_bin/python1.py", 0};

	//При желании можно запустить интерпретатор какого-нибудь скрипта. 
	//Тогда первый аргумент - путь к интерпретатору, второй - к скрипту
	//static const char *pszChildProcessArgs[3] = {"python", "./test.py", 0};

	int fdStdInPipe[2], fdStdOutPipe[2];
	
	fdStdInPipe[0] = fdStdInPipe[1] = fdStdOutPipe[0] = fdStdOutPipe[1] = -1;
	if (pipe(fdStdInPipe) != 0 || pipe(fdStdOutPipe) != 0)
	{
		std::cout << "Cannot create CGI pipe";
		// return 0;
	}

	// Duplicate stdin and stdout file descriptors
	int fdOldStdIn = dup(fileno(stdin));
	int fdOldStdOut = dup(fileno(stdout));

	// Duplicate end of pipe to stdout and stdin file descriptors
	if ((dup2(fdStdOutPipe[1], fileno(stdout)) == -1) || (dup2(fdStdInPipe[0], fileno(stdin)) == -1))
		std::cout << "Cannot create CGI pipe2";

	// Close original end of pipe
	close(fdStdInPipe[0]);
	close(fdStdOutPipe[1]);

	//Запускаем дочерний процесс, отдаем ему переменные командной строки и окружения
	// const int nChildProcessID = spawn_process(pszChildProcessArgs, variables);


    /* Create copy of current process */
    // const int pID = spawn_process(pszChildProcessArgs, variables);
    pid_t pID = fork();
    
    /* The parent`s new pid will be 0 */
    if(pID == 0)
    {
		/* We are now in a child progress 
		Execute different process */
		execve(pszChildProcessArgs[0], (char* const*)pszChildProcessArgs, (char* const*)_varsArray);
		/* This code will never be executed */
		exit(EXIT_SUCCESS);

	} else if (pID < 0)
        write(2, "Error Fork", 10);

    /* We are still in the original process */

	// Duplicate copy of original stdin an stdout back into stdout
	dup2(fdOldStdIn, fileno(stdin));
	dup2(fdOldStdOut, fileno(stdout));

	// Close duplicate copy of original stdin and stdout
	close(fdOldStdIn);
	close(fdOldStdOut);

	//Отдаем тело запроса дочернему процессу
	write(fdStdInPipe[1], strRequestBody.c_str(), strRequestBody.length());

	while (1)
	{
		//Читаем ответ от дочернего процесса
		char bufferOut[100000];
		int n = read(fdStdOutPipe[0], bufferOut, 100000);
		if (n > 0)
		{
			//Выводим ответ на экран
			fwrite(bufferOut, 1, n, stdout);
			fflush(stdout);
		}

		//Если дочерний процесс завершился, то завершаем и родительский процесс

		int status;
		if (waitpid(pID, &status, 0) > 0)
			std::cout << "OKKK";

	}



}








