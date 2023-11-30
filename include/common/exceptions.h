#ifndef COMMON_EXCEPTION_H
#define COMMON_EXCEPTION_H

#include "AppException.h"

class MaliciousCounterPartyException: public AppException {using AppException::AppException;};

#endif
