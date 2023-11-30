#include "common/connection/TCPListener.h"
#include "common/connection/exceptions.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>

TCPListener::TCPListener(uint16_t port) {
  this->fd = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if(bind(this->fd, (sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
    close(this->fd);
    throw SocketCreationException("Error binding socket");
  }
}

TCPListener::TCPListener(TCPListener&& other) {
  this->fd = other.fd;
  other.fd = -1;
}

TCPListener& TCPListener::operator = (TCPListener &&other) {
  this->fd = other.fd;
  other.fd = -1;
  return *this;
}

TCPListener::~TCPListener() {
  if(this->fd != -1) {
    close(this->fd);
  }
}

int TCPListener::getFd() const {
  return this->fd;
}
