#ifndef APP_EXCEPTION_H
#define APP_EXCEPTION_H

#include <exception>
#include <string>

class AppException: public std::exception {
  std::string msg;

public:

  AppException(const std::string &msg);
  const char* what() const noexcept;
};

#endif
