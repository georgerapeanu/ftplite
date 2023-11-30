#ifndef PASIVE_STARTED_CONNETION_H
#define PASIVE_STARTED_CONNETION_H

#include <sys/types.h>
#include <string>
#include "common/connection/AbstractConnection.h"
#include "common/connection/TCPListener.h"

class PasiveStartedConnection: public AbstractConnection{ 
public: 
  PasiveStartedConnection(const TCPListener& tcp_listener);
  PasiveStartedConnection(const AbstractConnection& other) = delete;
  PasiveStartedConnection& operator = (const PasiveStartedConnection& other) = delete;

  PasiveStartedConnection(PasiveStartedConnection&& other);
  PasiveStartedConnection& operator = (PasiveStartedConnection&& other);
  ~PasiveStartedConnection();
};

#endif
