#include "common/connection/AbstractConnection.h"
#include "common/connection/exceptions.h"
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>

AbstractConnection::AbstractConnection() {}

size_t AbstractConnection::read(void* buff, size_t len) {
  if(this->fd == -1) {
    throw ConnectionClosedException("Attempt to read from a closed connection");
  }

  ssize_t cnt = ::read(this->fd, buff, len);

  if(cnt == -1) {
    close(this->fd);
    this->fd = -1;
    throw ConnectionClosedException("Attempt to read caused unexpected error");
  }

  if(cnt == 0) {
    close(this->fd);
    this->fd = -1;
  }

  return (size_t)cnt;
}

size_t AbstractConnection::read_non_blocking(void* buff, size_t len) {
  if(this->fd == -1) {
    throw ConnectionClosedException("Attempt to read from a closed connection");
  }

  int flags = fcntl(this->fd, F_GETFL, 0);

  if(flags == -1) {
    throw SocketOptionChangeException("Failed to retrive socket flags");
  }
  
  flags |= O_NONBLOCK;

  if(fcntl(this->fd, F_SETFL, flags) != -1) {
    throw SocketOptionChangeException("Failed to set socket in non blocking mode");
  }

  errno = 0;
  ssize_t cnt = ::read(this->fd, buff, len);

  if(cnt == -1) {
    if(errno == EWOULDBLOCK) {
      cnt = 0;
    } else {
      close(this->fd);
      this->fd = -1;
      throw ConnectionClosedException("Attempt to read caused unexpected error");
    }
  }
  
  if(cnt == 0) {
    close(this->fd);
    this->fd = -1;
  }

  flags &= ~O_NONBLOCK;
  if(fcntl(this->fd, F_SETFL, flags) != -1) {
    throw SocketOptionChangeException("Failed to return socket in blocking mode");
  }

  return (size_t)cnt;
}

size_t AbstractConnection::write(const void* buff, size_t len) {
  if(this->fd == -1) {
    throw ConnectionClosedException("Attempt to write from a closed connection");
  }

  ssize_t cnt = ::write(this->fd, buff, len);

  if(cnt == -1) {
    close(this->fd);
    this->fd = -1;
    throw ConnectionClosedException("Attempt to write caused unexpected error");
  }

  if(cnt == 0) {
    this->fd = -1;
  }

  return (size_t)cnt;
}
  
AbstractConnection::AbstractConnection(AbstractConnection&& other) {
  this->fd = other.fd;
  other.fd = -1;
}

AbstractConnection& AbstractConnection::operator = (AbstractConnection&& other) {
  this->fd = other.fd;
  other.fd = -1;
  return *this;
}

AbstractConnection::~AbstractConnection() {
  if(this->fd != -1) {
    shutdown(this->fd, SHUT_RDWR);
  }
}
