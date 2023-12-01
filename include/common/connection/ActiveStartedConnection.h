#ifndef ACTIVE_STARTED_CONNECTION_H
#define ACTIVE_STARTED_CONNECTION_H

#include <sys/types.h>
#include <string>
#include "common/connection/AbstractConnection.h"
#include <memory>
#include <sys/socket.h>
#include <functional>

class ActiveStartedConnection: public AbstractConnection{ 
public: 
  /* 
   * Constructor for active started connection
   * @param host the host to connect to
   * @param port the port on the host to connect to
   * @throws SocketCreationException if socket creation fails
   */
  ActiveStartedConnection(const std::string& host, uint16_t port);
  /* 
   * Constructor for active started connection, using sockaddr struct
   * @throws SocketCreationException if socket creation fails
   */
  ActiveStartedConnection(const std::unique_ptr<sockaddr, std::function<void(sockaddr*)> > &sock_addr);
  ActiveStartedConnection(const ActiveStartedConnection& other) = delete;
  ActiveStartedConnection& operator = (const ActiveStartedConnection& other) = delete;

  ActiveStartedConnection(ActiveStartedConnection&& other);
  ActiveStartedConnection& operator = (ActiveStartedConnection&& other);
  ~ActiveStartedConnection();
};

#endif
