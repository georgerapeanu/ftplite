#ifndef COMMON_EXCEPTIONS_H
#define COMMON_EXCEPTIONS_H

#include "AppException.h"

class ConnectionClosedException: public AppException {using AppException::AppException;};
class SocketCreationException: public AppException {using AppException::AppException;};

#endif
