#ifndef SERVER_EXCEPTION_H
#define SERVER_EXCEPTION_H

#include "AppException.h"

class LoginException: public AppException {using AppException::AppException;};

#endif
