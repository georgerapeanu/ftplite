#include "AppException.h"

using namespace std;

AppException::AppException(const string& msg): msg(msg){}
const char* AppException::what() const noexcept {
  return this->msg.c_str();
}
