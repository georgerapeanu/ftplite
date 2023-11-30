#ifndef ACTIVE_STARTED_CONNECTION_H
#define ACTIVE_STARTED_CONNECTION_H

#include <sys/types.h>
#include <string>
#include "common/connection/AbstractConnection.h"

class ActiveStartedConnection: public AbstractConnection{ 
public: 
  /* 
   * Constructor for active started connection
   * @param host the host to connect to
   * @param port the port on the host to connect to
   * @throws SocketCreationException if socket creation fails
   */
  ActiveStartedConnection(const std::string& host, uint16_t port);
  ActiveStartedConnection(const AbstractConnection& other) = delete;
  ActiveStartedConnection& operator = (const ActiveStartedConnection& other) = delete;

  ActiveStartedConnection(ActiveStartedConnection&& other);
  ActiveStartedConnection& operator = (ActiveStartedConnection&& other);
  ~ActiveStartedConnection();
};

#endif
