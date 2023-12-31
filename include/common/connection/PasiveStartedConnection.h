#ifndef PASIVE_STARTED_CONNETION_H
#define PASIVE_STARTED_CONNETION_H

#include <sys/types.h>
#include <string>
#include "common/connection/AbstractConnection.h"
#include "common/connection/TCPListener.h"

class PasiveStartedConnection: public AbstractConnection{ 
public: 
  /* 
   * Constructor for pasive started connection
   * @param tcp_listener the tcp listener used to create connection
   * @throws SocketCreationException if socket creation fails
   */
  PasiveStartedConnection(const TCPListener& tcp_listener, int timeout = -1);
  PasiveStartedConnection(const PasiveStartedConnection& other) = delete;
  PasiveStartedConnection& operator = (const PasiveStartedConnection& other) = delete;

  PasiveStartedConnection(PasiveStartedConnection&& other);
  PasiveStartedConnection& operator = (PasiveStartedConnection&& other);
  ~PasiveStartedConnection();
};

#endif
