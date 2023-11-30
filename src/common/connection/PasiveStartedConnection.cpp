#include "common/connection/PasiveStartedConnection.h"
#include "common/connection/exceptions.h"
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>

using namespace std;

PasiveStartedConnection::PasiveStartedConnection(const TCPListener& tcp_listener) {
  if(tcp_listener.getFd() == -1) {
    throw SocketCreationException("Attempt to accept on -1 socket");
  }

  if(listen(tcp_listener.getFd(), 5) != 0) {
    throw SocketCreationException("Error listening");
  }
  this->fd = accept(tcp_listener.getFd(), NULL, NULL);

  if(this->fd == -1) {
    throw SocketCreationException("Error accepting connection");
  }
}

PasiveStartedConnection::PasiveStartedConnection(PasiveStartedConnection&& other):AbstractConnection(std::move(other)) {}

PasiveStartedConnection& PasiveStartedConnection::operator = (PasiveStartedConnection&& other) {
  AbstractConnection::operator = (std::move(other));
  return *this;
}

PasiveStartedConnection::~PasiveStartedConnection() {
  AbstractConnection::~AbstractConnection();
}
