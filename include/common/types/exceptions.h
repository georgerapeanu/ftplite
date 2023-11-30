#ifndef COMMON_TYPE_EXCEPTIONS_H
#define COMMON_TYPE_EXCEPTIONS_H

#include "AppException.h"

class TypeIterationEndedException: public AppException {using AppException::AppException;};
class ReadFileException: public AppException {using AppException::AppException;};
class WriteFileException: public AppException {using AppException::AppException;};

#endif
