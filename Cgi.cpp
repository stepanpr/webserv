

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

Cgi::Cgi(std::string body, struct s_config *config, std::string pathToScript, 
std::map<std::string, std::string> requestHeaders, std::string requestMethod, std::string &date) 
:  _config(config), _body(body), _pathToScript(pathToScript), _date(date)
{
	std::cout << CYAN << "!CGI is working" << RESET << "\n";

	_varsArray = NULL;
	// _fullPathToScript = ;
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
	if (requestHeaders.count("query_string"))
   		_vars.push_back("QUERY_STRING=" + requestHeaders.find("query_string")->second);
	else
        _vars.push_back("QUERY_STRING=");
    _vars.push_back("REMOTE_ADDR=" + _config->server_name);
    // _vars.push_back("REMOTE_IDENT=" + );
    // _vars.push_back("REMOTE_USER=" + );
	if (requestMethod.size() > 1)
    	_vars.push_back("REQUEST_METHOD=" + requestMethod);
	else
        _vars.push_back("REQUEST_METHOD=");
    // _vars.push_back("RЕQUEST_URI=" + );
    // _vars.push_back("SERVER_NAME=" + _config->server_name);
    _vars.push_back("SERVER_PORT=" + _config->listen);
    _vars.push_back("SERVER_PROTOCOL=HTTP/1.1");
    _vars.push_back("SERVER_SOFTWARE=web_server");
	_vars.push_back("DATE_LOCAL=" + _date);
	_vars.push_back("SERVER_NAME=" + toString( _config->serverID));

	
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




// void strToFd(std::string const &str, int fd)
// {
// 	std::streambuf *backup = std::cerr.rdbuf();
// 	dup2(fd, 2);
// 	std::cerr << str;
// 	std::cerr.rdbuf(backup);
// }







/*
** считывает созданный скриптом db.py файл db_[ID сервера].tmp
** и формирует на отсное его дынных html-страницу db_[ID сервера].html
*/
void Cgi::addToDataBase()
{
	std::string nameOfTMP = "./db/db_" + toString(_config->serverID) + ".tmp";
	std::string nameOfHTML = "./db/db_" + toString(_config->serverID) + ".html";

	std::ifstream db(nameOfTMP.c_str(), std::ios::in);
	std::string line;
	std::string dataBaseBuffer;
	while(getline(db, line)) 
			dataBaseBuffer += line;

	// std::cout << dataBaseBuffer;

	std::ofstream html(nameOfHTML.c_str(), std::ios::out | std::ios::trunc);
	html << "<html>\n<head>\n<title>webserv - DataBase</title>\n</head>\n<body>";

	html << "<head>\n<body>";
	html << dataBaseBuffer;
	html << "</body></html>";

	db.close();

	






	
	// db << newBody;
	// fstream fo("...", ios::in|ios::out);  
}






bool Cgi::launchCGI()
{
	//также можно указывать в html путь с /cgi_bin/script.cgi и в этом случае заменять cgi_bin на alias
	//

	// this->_fullPathToScript = "./cgi_bin" + this->_pathToScript;					//версия номер 1 без alias 
	this->_fullPathToScript = _config->cgi_alias[1]  + this->_pathToScript;		//версия номер 2 с alias (/cgi_bin/ == /usr/bin/[name]/webserv/cgi_bin/) 




// std::cout << CYAN << _pathToScript << RESET << std::endl;
// std::cout << CYAN << _fullPathToScript << RESET << std::endl;
// std::cout << CYAN << relativePathToScript << RESET << std::endl;

	/* проверяем существует ли такой файл */
	std::string relativePathToScript = "./cgi_bin" + this->_pathToScript;
	std::cout << CYAN << relativePathToScript << RESET << std::endl;

	if (stat(relativePathToScript.c_str(), &_stat) != 0)
	{
		std::cout << RED << "webserv: " << RESET << "please, check existence if script" << std::endl;
		return false;
	}


	/* проверяем расширение файла */
	int extentionOfSctipt = EXTENTION_WITH_INTERPRETER;
	if (_pathToScript.rfind(".py") != std::string::npos)
		this->_pathToHandler = "/usr/bin/python";
	else if (_pathToScript.rfind(".php") != std::string::npos)
		this->_pathToHandler = "/usr/bin/php";
	else if (_pathToScript.rfind(".perl") != std::string::npos || _pathToScript.rfind(".pl") != std::string::npos)
		this->_pathToHandler = "/usr/bin/perl";
	else if (_pathToScript.rfind(".cgi") != std::string::npos || _pathToScript.rfind(".exe") != std::string::npos)
		extentionOfSctipt = EXTENTION_WITHOUT_INTERPRETER;
	else if (_pathToScript.rfind(".py") == std::string::npos && _pathToScript.rfind(".php") == std::string::npos && 
	_pathToScript.rfind(".perl") == std::string::npos && _pathToScript.rfind(".pl") == std::string::npos && 
	_pathToScript.rfind(".cgi") == std::string::npos && _pathToScript.rfind(".exe") == std::string::npos)
	{
		std::cout << RED << "webserv: " << RESET << "extention of the script is false" << std::endl;
		return false;
	}

	const char *arguments_cgi[4] = {_fullPathToScript.c_str(), 0, 0, 0}; //если скрипту не требуется интерпретатор (C++) //path, arg, arg

	const char *arguments[3] = {_pathToHandler.c_str(), _fullPathToScript.c_str(), 0}; //если скрипт с интепретатором

	std::cout << CYAN << _pathToHandler << RESET << std::endl;
	std::cout << CYAN << _pathToScript << RESET << std::endl;
	std::cout << CYAN << _fullPathToScript << RESET << std::endl;
	std::cout << CYAN << relativePathToScript << RESET << std::endl;
	// std::cout << CYAN << _pathToHandler << RESET << std::endl;

	std::cout <<GREEN << extentionOfSctipt <<RESET << "\n";









	pid_t		pid;
	int			saveStdin;
	int			saveStdout;
	char		**env;
	std::string	newBody;

	// try {
	// 	env = this->_getEnvAsCstrArray();
	// }
	// catch (std::bad_alloc &e) {
	// 	std::cerr << RED << e.what() << RESET << std::endl;
	// }

	// SAVING STDIN AND STDOUT IN ORDER TO TURN THEM BACK TO NORMAL LATER
	saveStdin = dup(STDIN_FILENO);
	saveStdout = dup(STDOUT_FILENO);

	FILE	*fIn = tmpfile();
	FILE	*fOut = tmpfile();
	long	fdIn = fileno(fIn);
	long	fdOut = fileno(fOut);
	int		ret = 1;

	write(fdIn, _body.c_str(), _body.size());
	lseek(fdIn, 0, SEEK_SET);

	pid = fork();

	if (pid == -1)
	{
		throw Exceptions();
		// std::cerr << RED << "Fork crashed" << RESET << std::endl;
		// return ("Status: 500\r\n\r\n");
		return false;
	}
	else if (!pid)
	{
		char * const * nll = NULL;

		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);


		std::cout <<GREEN << extentionOfSctipt <<RESET << "\n";
		if (extentionOfSctipt == EXTENTION_WITHOUT_INTERPRETER)
			execve(arguments_cgi[0], (char* const*)arguments_cgi, (char* const*)_varsArray); //OK c++
		else if (extentionOfSctipt == EXTENTION_WITH_INTERPRETER)
			execve(arguments[0], (char* const*)arguments, (char* const*)_varsArray);     //OK py

		// throw Exceptions();
		// execve(arguments[0], nll, (char* const*)_varsArray);

		// execve(scriptName.c_str(), nll, env);
		std::cerr << RED << "Execve crashed" << RESET << std::endl;
		// write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
		exit(EXIT_SUCCESS);
	}
	else
	{
		char	buffer[6024] = {0};

		waitpid(-1, NULL, 0);
		lseek(fdOut, 0, SEEK_SET);

		ret = 1;
		while (ret > 0)
		{
			memset(buffer, 0, 6024);
			ret = read(fdOut, buffer, 6024 - 1);
			newBody += buffer;
		}
	}

	// std::cout << newBody << std::endl;\
	///////create DB
	addToDataBase();
	// DB_buffer += newBody;


	dup2(saveStdin, STDIN_FILENO);
	dup2(saveStdout, STDOUT_FILENO);
	fclose(fIn);
	fclose(fOut);
	close(fdIn);
	close(fdOut);
	close(saveStdin);
	close(saveStdout);




	return true;
}










































































































































































// bool Cgi::launchCGI()
// {
// 	// this->_fullPathToScript = "./cgi_bin" + this->_pathToScript;					//версия номер 1 без alias 
// 	this->_fullPathToScript = _config->cgi_alias[1]  + this->_pathToScript;		//версия номер 2 с alias (/cgi_bin/ == /usr/bin/[name]/webserv/cgi_bin/) 

// 	std::string relativePathToScript = _fullPathToScript.substr(2, _fullPathToScript.length());


// // std::cout << CYAN << _pathToScript << RESET << std::endl;
// // std::cout << CYAN << _fullPathToScript << RESET << std::endl;
// // std::cout << CYAN << relativePathToScript << RESET << std::endl;

// 	/* проверяем существует ли такой файл */
// 	// if (stat(relativePathToScript.c_str(), &_stat) != 0)
// 	// 	return false;


// 	/* проверяем расширение файла */
// 	int cgi = 0;
// 	if (_pathToScript.rfind(".py") != std::string::npos)
// 		this->_pathToHandler = "/usr/bin/python";
// 	else if (_pathToScript.rfind(".php") != std::string::npos)
// 		this->_pathToHandler = "/usr/bin/php";
// 	else if (_pathToScript.rfind(".perl") != std::string::npos || _pathToScript.rfind(".pl") != std::string::npos)
// 		this->_pathToHandler = "/usr/bin/perl";
// 	else // if (_pathToScript.rfind(".cgi") || _pathToScript.rfind(".exe"))
// 		cgi = 1;

// 	const char *arguments_cgi[4] = {_fullPathToScript.c_str(), 0, 0, 0}; //если скрипту не требуется интерпретатор (C++) //path, arg, arg

// 	const char *arguments[3] = {_pathToHandler.c_str(), _fullPathToScript.c_str(), 0}; //если скрипт с интепретатором
// 	std::cout << CYAN << _pathToHandler << RESET << std::endl;

// 	// std::cout << CYAN << _pathToScript << RESET << std::endl;
// std::cout << CYAN << _fullPathToScript << RESET << std::endl;
// // std::cout << CYAN << relativePathToScript << RESET << std::endl;
// // std::cout << CYAN << _pathToHandler << RESET << std::endl;



// //////////////////////////////////////////////////////////////////////////////////////////


// 	// int status;
// 	// int FD[2];
// 	// int tmpFD;
// 	// pid_t pID;
// 	// if ((tmpFD = open("tempbuffer", O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0 || pipe(FD) < 0)
// 	// 	throw Exceptions();

//     // if ((pID = fork()) < 0)
// 	// 	throw Exceptions();
//     // if(pID == 0)
//     // {
// 	// 	close(FD[1]);
// 	// 	dup2(FD[0], 0); 
// 	// 	close(FD[0]);
// 	// 	dup2(tmpFD, 1);
// 	// 	// execve(CGIarguments[0], (char* const*)CGIarguments, (char* const*)_varsArray);
// 	// 	int exec;
// 	// 	if ((exec = execve(arguments[0], (char* const*)arguments, (char* const*)_varsArray)) < 0)
// 	// 		throw Exceptions();
// 	// 	// exit(status);
// 	// } 
// 	// else if (pID < 0)
//     // 	write(2, "Error Fork", 10);
// 	// else if (pID > 0)
// 	// {
// 	// 	close(FD[0]);
// 	// 	strToFd(this->_body, FD[1]);
// 	// 	int stat;
// 	// 	waitpid(pID, &stat, 0);
// 	// 	close(FD[1]);
// 	// 	close(tmpFD);
// 	// }


// 	// 	//Если дочерний процесс завершился, то завершаем и родительский процесс
// 	// 	// int status;
// 	// 	// if (waitpid(pID, &status, 0) > 0)
// 	// 	// 	std::cout << "OKKK";

	

// //////////////////////////////////////////////////////////////////////////////////////////

// //   char s[100];
// //   printf("%s\n", getcwd(s, 100));
// 	// std::cout << CYAN << _pathToScript << RESET << std::endl;
// 	// std::string dir = _pathToScript.substr(0, _pathToScript.find("/") + 1);
// 	// const char *pathToScript_char = "cgi_bin/";
// 	// std::cout << CYAN << dir << RESET << std::endl;


// 	// int tmpFD;
// 	// int FD[2];
// 	// pid_t pid;
// 	// if ((tmpFD = open("tempbuffer", O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0 || pipe(FD) < 0 || (pid = fork()) < 0)
// 	// 	throw Exceptions();
// 	// if (pid == 0)
// 	// {
// 	// 	close(FD[1]);
// 	// 	dup2(FD[0], 0);
// 	// 	close(FD[0]);
// 	// 	dup2(tmpFD, 1);

// 	// 	int exec;

// 	// 	// if (chdir(pathToScript_char)) //переходим к каталогу со скриптом
// 	// 	// 	throw Exceptions();
// 	// 	if (cgi == 1)
// 	// 	{
// 	// 		if ((exec = execve(arguments_cgi[0], (char* const*)arguments_cgi, (char* const*)_varsArray)) < 0)
// 	// 			throw Exceptions();
// 	// 		// std::cout << CYAN << "1" << RESET << std::endl;
// 	// 	}
// 	// 	else if (cgi == 0)
// 	// 	{
// 	// 		if ((exec = execve(arguments[0], (char* const*)arguments, (char* const*)_varsArray)) < 0)
// 	// 			throw Exceptions();
// 	// 		// std::cout << CYAN << "2" << RESET << std::endl;
// 	// 	}
// 	// }
// 	// else if (pid > 0)
// 	// {
// 	// 	close(FD[0]);
// 	// 	strToFd(this->_body, FD[1]);
// 	// 	int stat;
// 	// 	waitpid(pid, &stat, 0);
// 	// 	close(FD[1]);
// 	// 	close(tmpFD);
// 	// }
// //////////////////////////////////////////////////////////////////////////////////////////

// 	// int fdIn, fdOut, status;
//     // pid_t pid = fork();
//     // if (pid == 0) {
//     //     fdIn = open("cgi_file_input.txt", O_RDWR);
//     //     dup2(fdIn, 0);
//     //     fdOut = open("cgi_file_output.txt", O_CREAT | O_RDWR | O_TRUNC, 0666);
//     //     dup2(fdOut, 1);
//     //     // chdir(_client.getHttp().getStartLine().find("change_location")->second.c_str());
//     //     // std::cerr << "dir=\"" << _client.getHttp().getStartLine().find("change_location")->second << "\"" << std::endl;
//     //     // std::cerr << "launch=\"" << getLaunch()[0] << " " << getLaunch()[1] << "\"" << std::endl;
//     //     status = execve(CGIarguments[0], (char* const*)CGIarguments, (char* const*)_varsArray);
//     //     exit(status);
//     // } else if (pid < 0)
//     //     write(2, "Error Fork", 10);
//     // pid = waitpid(pid, &status, WUNTRACED);
//     // close(fdIn);

// //////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////






// pid_t		pid;
// 	int			saveStdin;
// 	int			saveStdout;
// 	char		**env;
// 	std::string	newBody;

// 	// try {
// 	// 	env = this->_getEnvAsCstrArray();
// 	// }
// 	// catch (std::bad_alloc &e) {
// 	// 	std::cerr << RED << e.what() << RESET << std::endl;
// 	// }

// 	// SAVING STDIN AND STDOUT IN ORDER TO TURN THEM BACK TO NORMAL LATER
// 	saveStdin = dup(STDIN_FILENO);
// 	saveStdout = dup(STDOUT_FILENO);

// 	FILE	*fIn = tmpfile();
// 	FILE	*fOut = tmpfile();
// 	long	fdIn = fileno(fIn);
// 	long	fdOut = fileno(fOut);
// 	int		ret = 1;

// 	write(fdIn, _body.c_str(), _body.size());
// 	lseek(fdIn, 0, SEEK_SET);

// 	pid = fork();

// 	if (pid == -1)
// 	{
// 		std::cerr << RED << "Fork crashed." << RESET << std::endl;
// 		return ("Status: 500\r\n\r\n");
// 	}
// 	else if (!pid)
// 	{
// 		char * const * nll = NULL;

// 		dup2(fdIn, STDIN_FILENO);
// 		dup2(fdOut, STDOUT_FILENO);

// 		//  execve(arguments[0], (char* const*)arguments, (char* const*)_varsArray);     //OK py
// 		 execve(arguments_cgi[0], (char* const*)arguments_cgi, (char* const*)_varsArray); //OK c++

// 		// execve(arguments[0], nll, (char* const*)_varsArray);

// 		// execve(scriptName.c_str(), nll, env);
// 		std::cerr << RED << "Execve crashed." << RESET << std::endl;
// 		// write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
// 		exit(0);
// 	}
// 	else
// 	{
// 		char	buffer[6024] = {0};

// 		waitpid(-1, NULL, 0);
// 		lseek(fdOut, 0, SEEK_SET);

// 		ret = 1;
// 		while (ret > 0)
// 		{
// 			memset(buffer, 0, 6024);
// 			ret = read(fdOut, buffer, 6024 - 1);
// 			newBody += buffer;
// 			std::cout << buffer;
// 		}
// 	}

// 	dup2(saveStdin, STDIN_FILENO);
// 	dup2(saveStdout, STDOUT_FILENO);
// 	fclose(fIn);
// 	fclose(fOut);
// 	close(fdIn);
// 	close(fdOut);
// 	close(saveStdin);
// 	close(saveStdout);








// 	return true;
// }




















// void Cgi::launchCGI()
// {
// 	//Формируем в глобальных переменных тело запроса и его длинну
// 	static const std::string strRequestBody = _body.c_str();
// 	std::cout <<CYAN<< _body.c_str() << RESET<< std::endl;

// 	static const std::string strRequestHeader = "Content-Length=" + std::to_string((long long)strRequestBody.length());
// 	//Формируем переменные окружения которые будут отосланы дочернему процессу
// //	static const char *variables[4] = {strRequestHeader.c_str(), "VARIABLE2=erererer", "VARIABLE3=3", 0};

// 	//Формируем переменные командной строки для дочернего процесса. Первая переменная - путь к дочернему процессу.
// 	// static const char *pszChildProcessArgs[4] = {"./cgi_bin/mkcgi", "first argument", "second argument", 0};
// 	static const char *CGIarguments[3] = {"/usr/bin/perl", "./cgi_bin/perl1.cgi", 0};
// 	// static const char *CGIarguments[3] = {"/usr/bin/python", "./cgi_bin/python1.py", 0};

// 	//При желании можно запустить интерпретатор какого-нибудь скрипта. 
// 	//Тогда первый аргумент - путь к интерпретатору, второй - к скрипту
// 	//static const char *pszChildProcessArgs[3] = {"python", "./test.py", 0};

// 	int fdStdInPipe[2], fdStdOutPipe[2];
// 	int p[2];
	
// 	fdStdInPipe[0] = fdStdInPipe[1] = fdStdOutPipe[0] = fdStdOutPipe[1] = -1;
// 	// if (pipe(fdStdInPipe) != 0 || pipe(fdStdOutPipe) != 0)
// 	// {
// 	// 	std::cout << "Cannot create CGI pipe";
// 	// 	// return 0;
// 	// }

// 	// Duplicate stdin and stdout file descriptors
// 	// int fdOldStdIn = dup(fileno(stdin));
// 	// int fdOldStdOut = dup(fileno(stdout));

// 	// // Duplicate end of pipe to stdout and stdin file descriptors
// 	// if ((dup2(fdStdOutPipe[1], fileno(stdout)) == -1) || (dup2(fdStdInPipe[0], fileno(stdin)) == -1))
// 	// 	std::cout << "Cannot create CGI pipe2";

// 	// // Close original end of pipe
// 	// close(fdStdInPipe[0]);
// 	// close(fdStdOutPipe[1]);

// 	//Запускаем дочерний процесс, отдаем ему переменные командной строки и окружения
// 	// const int nChildProcessID = spawn_process(pszChildProcessArgs, variables);


//     /* Create copy of current process */
//     // const int pID = spawn_process(pszChildProcessArgs, variables);
//     pid_t pID = fork();
    
//     /* The parent`s new pid will be 0 */
//     if(pID == 0)
//     {
// 		close(p[1]);
// 		dup2(p[0], 0); 
// 		/* We are now in a child progress 
// 		Execute different process */
// 		// execve(pszChildProcessArgs[0], (char* const*)pszChildProcessArgs, (char* const*)_varsArray);
// 		execve(CGIarguments[0], (char* const*)CGIarguments, (char* const*)_varsArray);
// 		/* This code will never be executed */
// 		exit(0);

// 	}// else if (pID < 0)
//     //     write(2, "Error Fork", 10);

//     /* We are still in the original process */

// 	// Duplicate copy of original stdin an stdout back into stdout
// 	// dup2(fdOldStdIn, fileno(stdin));
// 	// dup2(fdOldStdOut, fileno(stdout));

// 	// // Close duplicate copy of original stdin and stdout
// 	// close(fdOldStdIn);
// 	// close(fdOldStdOut);

// 	// //Отдаем тело запроса дочернему процессу
// 	// write(fdStdInPipe[1], strRequestBody.c_str(), strRequestBody.length());

// 	// while (1)
// 	// {
// 	// 	//Читаем ответ от дочернего процесса
// 	// 	char bufferOut[100000];
// 	// 	int n = read(fdStdOutPipe[0], bufferOut, 100000);
// 	// 	if (n > 0)
// 	// 	{
// 	// 		//Выводим ответ на экран
// 	// 		fwrite(bufferOut, 1, n, stdout);
// 	// 		fflush(stdout);
// 	// 	}

// 	// 	//Если дочерний процесс завершился, то завершаем и родительский процесс

// 	// 	int status;
// 	// 	if (waitpid(pID, &status, 0) > 0)
// 	// 		std::cout << "OKKK";

// 	// }



// }









// // void Cgi::launchCGI()
// // {
// 	//Формируем в глобальных переменных тело запроса и его длинну
// 	static const std::string strRequestBody = _body.c_str();

// 	static const std::string strRequestHeader = "Content-Length=" + std::to_string((long long)strRequestBody.length());
// 	//Формируем переменные окружения которые будут отосланы дочернему процессу
// //	static const char *variables[4] = {strRequestHeader.c_str(), "VARIABLE2=erererer", "VARIABLE3=3", 0};

// 	//Формируем переменные командной строки для дочернего процесса. Первая переменная - путь к дочернему процессу.
// 	// static const char *pszChildProcessArgs[4] = {"./cgi_bin/mkcgi", "first argument", "second argument", 0};
// 	static const char *pszChildProcessArgs[3] = {"/usr/bin/perl", "./cgi_bin/perl1.cgi", 0};
// 	// static const char *pszChildProcessArgs[3] = {"/usr/bin/python", "./cgi_bin/python1.py", 0};

// 	//При желании можно запустить интерпретатор какого-нибудь скрипта. 
// 	//Тогда первый аргумент - путь к интерпретатору, второй - к скрипту
// 	//static const char *pszChildProcessArgs[3] = {"python", "./test.py", 0};

// 	int fdStdInPipe[2], fdStdOutPipe[2];
	
// 	fdStdInPipe[0] = fdStdInPipe[1] = fdStdOutPipe[0] = fdStdOutPipe[1] = -1;
// 	// if (pipe(fdStdInPipe) != 0 || pipe(fdStdOutPipe) != 0)
// 	// {
// 	// 	std::cout << "Cannot create CGI pipe";
// 	// 	// return 0;
// 	// }

// 	// Duplicate stdin and stdout file descriptors
// 	// int fdOldStdIn = dup(fileno(stdin));
// 	// int fdOldStdOut = dup(fileno(stdout));

// 	// // Duplicate end of pipe to stdout and stdin file descriptors
// 	// if ((dup2(fdStdOutPipe[1], fileno(stdout)) == -1) || (dup2(fdStdInPipe[0], fileno(stdin)) == -1))
// 	// 	std::cout << "Cannot create CGI pipe2";

// 	// // Close original end of pipe
// 	// close(fdStdInPipe[0]);
// 	// close(fdStdOutPipe[1]);

// 	//Запускаем дочерний процесс, отдаем ему переменные командной строки и окружения
// 	// const int nChildProcessID = spawn_process(pszChildProcessArgs, variables);


//     /* Create copy of current process */
//     // const int pID = spawn_process(pszChildProcessArgs, variables);
//     pid_t pID = fork();
    
//     /* The parent`s new pid will be 0 */
//     if(pID == 0)
//     {
// 		/* We are now in a child progress 
// 		Execute different process */
// 		// execve(pszChildProcessArgs[0], (char* const*)pszChildProcessArgs, (char* const*)_varsArray);
// 		/* This code will never be executed */
// 		exit(0);

// 	}// else if (pID < 0)
//     //     write(2, "Error Fork", 10);

//     /* We are still in the original process */

// 	// Duplicate copy of original stdin an stdout back into stdout
// 	// dup2(fdOldStdIn, fileno(stdin));
// 	// dup2(fdOldStdOut, fileno(stdout));

// 	// // Close duplicate copy of original stdin and stdout
// 	// close(fdOldStdIn);
// 	// close(fdOldStdOut);

// 	// //Отдаем тело запроса дочернему процессу
// 	// write(fdStdInPipe[1], strRequestBody.c_str(), strRequestBody.length());

// 	// while (1)
// 	// {
// 	// 	//Читаем ответ от дочернего процесса
// 	// 	char bufferOut[100000];
// 	// 	int n = read(fdStdOutPipe[0], bufferOut, 100000);
// 	// 	if (n > 0)
// 	// 	{
// 	// 		//Выводим ответ на экран
// 	// 		fwrite(bufferOut, 1, n, stdout);
// 	// 		fflush(stdout);
// 	// 	}

// 	// 	//Если дочерний процесс завершился, то завершаем и родительский процесс

// 	// 	int status;
// 	// 	if (waitpid(pID, &status, 0) > 0)
// 	// 		std::cout << "OKKK";

// 	// }



// }






























































// //////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////

// #include "Cgi.hpp"

// // Cgi::Cgi() {}
// // Cgi::~Cgi()
// // {         
// // 	for (int i = 0; _varsArray[i]; ++i)
// //         free(_varsArray[i]);
// // }
// // Cgi &Cgi::operator=(const Cgi &copy)
// // {
// // 	return *this;
// // }
// // Cgi::Cgi(const Cgi &cgi) { *this = cgi; }

// Cgi::Cgi(std::string body, struct s_config *config, std::string pathToScript, std::map<std::string, std::string> requestHeaders, std::string requestMethod) 
// :  _config(config), _body(body), _pathToScript(pathToScript)
// {
// 	std::cout << CYAN << "!CGI is working" << RESET << "\n";
// 	_varsArray = getVarsArray(setVariables(requestHeaders, requestMethod));
// }


// std::vector<std::string> Cgi::setVariables(std::map<std::string, std::string> requestHeaders, std::string requestMethod)
// {
// 	// std::vector<std::string> env;
//     // _vars.push_back("AUTH_TYPE=" + locations.auth_data.AuthType);
//     _vars.push_back("CONTENT_LENGTH=" + toString(_body.size()));
// 	if (requestHeaders.find("Content-Type") != requestHeaders.end())
//     	_vars.push_back("CONTENT_TYPE=" + requestHeaders.find("Content-Type")->second);
// 	else
// 		_vars.push_back("CONTENT_TYPE=");
//     _vars.push_back("GATEWAY_INTERFACE=CGI/1.1");
//     // _vars.push_back(getPathInfo("PATH_INFO=" + );
//     // _vars.push_back("PATH_TRANSLATED=" + );
// 	if (requestHeaders.count("query_string"))
//    		_vars.push_back("QUERY_STRING=" + requestHeaders.find("query_string")->second);
// 	else
//         _vars.push_back("QUERY_STRING=");
//     _vars.push_back("REMOTE_ADDR=" + _config->server_name);
//     // _vars.push_back("REMOTE_IDENT=" + );
//     // _vars.push_back("REMOTE_USER=" + );
// 	if (requestMethod.size() > 1)
//     	_vars.push_back("REQUEST_METHOD=" + requestMethod);
// 	else
//         _vars.push_back("REQUEST_METHOD=");
//     // _vars.push_back("RЕQUEST_URI=" + );
//     _vars.push_back("SERVER_NAME=" + _config->server_name);
//     _vars.push_back("SERVER_PORT=" + _config->listen);
//     _vars.push_back("SERVER_PROTOCOL=HTTP/1.1");
//     _vars.push_back("SERVER_SOFTWARE=web_server");
	
// 	// for (int i = 0; i < (int)_vars.size(); i++)
// 	// 	std::cout << _vars[i] << std::endl;
// 	return _vars;
// }



// char **Cgi::getVarsArray(std::vector<std::string> vars) 
// {
//     char **pString = (char **) malloc(sizeof(char **) * (vars.size() + 1));
//     std::vector<std::string>::const_iterator j = vars.begin();
//     std::string::size_type i = 0;
//     for (; i < vars.size(); ++i) {
//         pString[i] = strdup(j->c_str());
//         j++;
//     }
//     pString[i] = NULL;
//     return pString;
// }




// void strToFd(std::string const &str, int fd)
// {
// 	std::streambuf *backup = std::cerr.rdbuf();
// 	dup2(fd, 2);
// 	std::cerr << str;
// 	std::cerr.rdbuf(backup);
// }



// bool Cgi::launchCGI()
// {
// 	this->_fullPathToScript = "./cgi_bin" + this->_pathToScript;
// 	std::string relativePathToScript = _fullPathToScript.substr(2, _fullPathToScript.length());


// std::cout << CYAN << _pathToScript << RESET << std::endl;
// std::cout << CYAN << _fullPathToScript << RESET << std::endl;
// std::cout << CYAN << relativePathToScript << RESET << std::endl;

// 	/* проверяем существует ли такой файл */
// 	if (stat(relativePathToScript.c_str(), &_stat) != 0)
// 		return false;


// 	/* проверяем расширение файла */
// 	int cgi = 0;
// 	if (_pathToScript.rfind(".py") != std::string::npos)
// 		this->_pathToHandler = "/usr/bin/python";
// 	else if (_pathToScript.rfind(".php") != std::string::npos)
// 		this->_pathToHandler = "/usr/bin/php";
// 	else if (_pathToScript.rfind(".perl") != std::string::npos || _pathToScript.rfind(".pl") != std::string::npos)
// 		this->_pathToHandler = "/usr/bin/perl";
// 	else // if (_pathToScript.rfind(".cgi") || _pathToScript.rfind(".exe"))
// 		cgi = 1;

// 	static const char *arguments_cgi[4] = {_fullPathToScript.c_str(), 0, 0, 0}; //если скрипту не требуется интерпретатор (C++) //path, arg, arg

// 	static const char *arguments[3] = {_pathToHandler.c_str(), _fullPathToScript.c_str(), 0}; //если скрипт с интепретатором
// 	std::cout << CYAN << _pathToHandler << RESET << std::endl;


// //////////////////////////////////////////////////////////////////////////////////////////


// 	// int status;
// 	// int p[2];

//     // pid_t pID = fork();
    
//     // if(pID == 0)
//     // {
// 	// 	// close(p[1]);
// 	// 	// dup2(p[0], 0); 
// 	// 	// execve(CGIarguments[0], (char* const*)CGIarguments, (char* const*)_varsArray);
// 	// 	status = execve(CGIarguments[0], (char* const*)CGIarguments, (char* const*)_varsArray);
// 	// 	exit(status);
// 	// } else if (pID < 0)
//     // 	write(2, "Error Fork", 10);


// 	// 	//Если дочерний процесс завершился, то завершаем и родительский процесс
// 	// 	// int status;
// 	// 	if (waitpid(pID, &status, 0) > 0)
// 	// 		std::cout << "OKKK";

	

// //////////////////////////////////////////////////////////////////////////////////////////

// //   char s[100];
// //   printf("%s\n", getcwd(s, 100));
// 	// std::cout << CYAN << _pathToScript << RESET << std::endl;
// 	// std::string dir = _pathToScript.substr(0, _pathToScript.find("/") + 1);
// 	// const char *pathToScript_char = "cgi_bin/";
// 	// std::cout << CYAN << dir << RESET << std::endl;


// 	int tmpFD;
// 	int FD[2];
// 	pid_t pid;
// 	if ((tmpFD = open("tempbuffer", O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0 || pipe(FD) < 0 || (pid = fork()) < 0)
// 		throw Exceptions();
// 	if (pid == 0)
// 	{
// 		close(FD[1]);
// 		dup2(FD[0], 0);
// 		close(FD[0]);
// 		dup2(tmpFD, 1);

// 		int exec;

// 		// if (chdir(pathToScript_char)) //переходим к каталогу со скриптом
// 		// 	throw Exceptions();
// 		if (cgi == 1)
// 		{
// 			if ((exec = execve(arguments_cgi[0], (char* const*)arguments_cgi, (char* const*)_varsArray)) < 0)
// 				throw Exceptions();
// 			// std::cout << CYAN << "1" << RESET << std::endl;
// 		}
// 		else if (cgi == 0)
// 		{
// 			if ((exec = execve(arguments[0], (char* const*)arguments, (char* const*)_varsArray)) < 0)
// 				throw Exceptions();
// 			// std::cout << CYAN << "2" << RESET << std::endl;
// 		}
// 	}
// 	else if (pid > 0)
// 	{
// 		close(FD[0]);
// 		strToFd(this->_body, FD[1]);
// 		int stat;
// 		waitpid(pid, &stat, 0);
// 		close(FD[1]);
// 		close(tmpFD);
// 	}
// //////////////////////////////////////////////////////////////////////////////////////////

// 	// int fdIn, fdOut, status;
//     // pid_t pid = fork();
//     // if (pid == 0) {
//     //     fdIn = open("cgi_file_input.txt", O_RDWR);
//     //     dup2(fdIn, 0);
//     //     fdOut = open("cgi_file_output.txt", O_CREAT | O_RDWR | O_TRUNC, 0666);
//     //     dup2(fdOut, 1);
//     //     // chdir(_client.getHttp().getStartLine().find("change_location")->second.c_str());
//     //     // std::cerr << "dir=\"" << _client.getHttp().getStartLine().find("change_location")->second << "\"" << std::endl;
//     //     // std::cerr << "launch=\"" << getLaunch()[0] << " " << getLaunch()[1] << "\"" << std::endl;
//     //     status = execve(CGIarguments[0], (char* const*)CGIarguments, (char* const*)_varsArray);
//     //     exit(status);
//     // } else if (pid < 0)
//     //     write(2, "Error Fork", 10);
//     // pid = waitpid(pid, &status, WUNTRACED);
//     // close(fdIn);

// 	return true;
// }


