#include "common/connection/TCPListener.h"
#include "common/connection/exceptions.h"
#include <functional>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>

TCPListener::TCPListener(std::unique_ptr<sockaddr, std::function<void (sockaddr*)>> &sock_addr) {
  this->fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  if(this->fd == -1) {
    throw SocketCreationException("Error creating socket");
  }
  const sockaddr* sock_addr_c_ptr = sock_addr.get();
  if(bind(this->fd, (sockaddr*) &sock_addr_c_ptr, sizeof(sockaddr)) == -1) {
    close(this->fd);
    throw SocketCreationException("Error binding socket");
  }
  
  sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  socklen_t sock_len = sizeof(server_addr);
  if(getsockname(this->fd, (sockaddr*)&server_addr, &sock_len) != 0) {
    close(this->fd);
    throw SocketCreationException("Error querying socket");
  }
  this->port = ntohs(server_addr.sin_port);

  if(listen(this->fd, 5) != 0) {
    close(this->fd);
    this->fd = -1;
    throw SocketCreationException("Error listening on socket");
  }
}

TCPListener::TCPListener(uint16_t port) {
  this->fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  if(this->fd == -1) {
    throw SocketCreationException("Error creating socket");
  }
  sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if(bind(this->fd, (sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
    close(this->fd);
    throw SocketCreationException("Error binding socket");
  }

  socklen_t sock_len = sizeof(server_addr);
  if(getsockname(this->fd, (sockaddr*)&server_addr, &sock_len) != 0) {
    close(this->fd);
    throw SocketCreationException("Error querying socket");
  }
  this->port = ntohs(server_addr.sin_port);
  
  if(listen(this->fd, 5) != 0) {
    close(this->fd);
    this->fd = -1;
    throw SocketCreationException("Error listening on socket");
  }
}

TCPListener::TCPListener(TCPListener&& other) {
  this->fd = other.fd;
  this->port = other.port;
  other.fd = -1;
  other.port = 0;
}

TCPListener& TCPListener::operator = (TCPListener &&other) {
  this->fd = other.fd;
  this->port = other.port;
  other.fd = -1;
  other.port = 0;
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

int TCPListener::getPort() const {
  return this->port;
}
