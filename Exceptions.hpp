#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP

# include <iostream>
# include <sys/errno.h>

// class Exceptions: public std::exception
// {
// public:
// 	virtual char const *what() const throw();
// };

class Exceptions: public std::exception
{
  virtual const char* what() const throw()
  {
	  return strerror(errno);
    // return "My exception happened";
  }
};

#endif