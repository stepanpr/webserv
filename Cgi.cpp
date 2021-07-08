#include "Cgi.hpp"


// Cgi::Cgi() {}

// Cgi::~Cgi() {}



//Cgi::Cgi(std::string body, struct s_config *config)
//{
//
//}

Cgi::Cgi(const Cgi &cgi) { *this = cgi; }

//Cgi &Cgi::operator=(const Cgi &copy)
//{
//	return *this;
//}

void Cgi::launchCGI()
{

	// this->setVariables();
	pid_t PID;
	PID = fork();

	if(PID == 0)
	{
		// execve()
	}

	// waitpid()
}
















// std::map<std::string, std::string> aka;

// struct akaInit
// {
//     akaInit()
//     {
//         aka[ "George" ] = "John";
//         aka[ "Joe" ] = "Al";
//         aka[ "Phil" ] = "Sue";
//         aka[ "Smitty" ] = "Yando";
//     }
// } AkaInit;

// using namespace std;

// map<int,int> create_map()
// {
//   map<int,int> m;
//   m[1] = 2;
//   m[3] = 4;
//   m[5] = 6;
//   return m;
// }

// map<int,int> m = create_map();

