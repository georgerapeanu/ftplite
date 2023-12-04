//check the way common/exceptions.h is built

#include "AppException.h"


class LoginException: public AppException {using AppException::AppException;};
