#include "common/connection/ActiveStartedConnection.h"
#include "common/connection/exceptions.h"
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>

using namespace std;

ActiveStartedConnection::ActiveStartedConnection(std::unique_ptr<sockaddr> sock_addr) {
  const sockaddr* sock_addr_c_ptr = sock_addr.get();
  this->fd = socket(AF_INET, SOCK_STREAM, 0);
  if(this->fd == -1) {
    throw SocketCreationException("Error creating socket");
  }
  int status = connect(this->fd, sock_addr_c_ptr, sizeof(sockaddr));

  if(status == -1) {
    close(this->fd);
    this->fd = -1;
    throw SocketCreationException("Error connecting with socket");
  }
}

ActiveStartedConnection::ActiveStartedConnection(const string& host, uint16_t port) {
  this->fd = -1;

  string port_string = to_string(port);
  addrinfo hints, *result;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if(getaddrinfo(host.c_str(), port_string.c_str(), &hints, &result) != 0) {
    if(result) {
      freeaddrinfo(result);
    }
    throw SocketCreationException("Error getaddr info");
  }

  for(addrinfo* rp = result; rp != NULL; rp = rp->ai_next) {
    this->fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (this->fd == -1)
      continue;
    if (connect(this->fd, rp->ai_addr, rp->ai_addrlen) != -1)
      break; 

    close(this->fd);
    this->fd = -1;
  }

  freeaddrinfo(result);

  if(this->fd == -1) {
    throw SocketCreationException("Error creating active connection");
  }
}

ActiveStartedConnection::ActiveStartedConnection(ActiveStartedConnection&& other):AbstractConnection(std::move(other)) {}

ActiveStartedConnection& ActiveStartedConnection::operator = (ActiveStartedConnection&& other) {
  AbstractConnection::operator = (std::move(other));
  return *this;
}

ActiveStartedConnection::~ActiveStartedConnection() {
  AbstractConnection::~AbstractConnection();
}
