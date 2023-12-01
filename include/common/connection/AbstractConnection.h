#ifndef ABSTRACT_CONNECTION_H
#define ABSTRACT_CONNECTION_H

#include <memory>
#include <sys/socket.h>
#include <sys/types.h>
#include <functional>

/*
 * @class AbstractConnection: Abstract class for manaing TCP connections
 *
 */
class AbstractConnection {
protected:
  int fd;
  AbstractConnection();
public: 

  AbstractConnection(const AbstractConnection& other) = delete;
  AbstractConnection& operator = (const AbstractConnection& other) = delete;
  AbstractConnection(AbstractConnection&& other);
  AbstractConnection& operator = (AbstractConnection&& other);
  virtual ~AbstractConnection() = 0;

  /* 
   * Reads count bytes from connection into buffer buff. 
   * @param buff the buffer into which to read the bytes
   * @param count the number of bytes to read
   * @return the number of bytes read
   * @throws ConnectionClosedException if connection closes
   */
  size_t read(void* buff, size_t count);

  /* 
   * Reads at most count bytes from connection into buffer buff. 
   * @param buff the buffer into which to read the bytes
   * @param count the number of bytes to read
   * @return the number of bytes read
   * @throws ConnectionClosedException if connection closes
   * @throws SocketOptionChangeException if changing the socket options fails
   */
  size_t read_non_blocking(void* buff, size_t count);
  
  /* 
   * Writes count bytes from buffer buff into connection. 
   * @param buf the buffer into which to write the bytes
   * @param count the number of bytes to write
   * @return the number of bytes written
   * @throws ConnectionClosedException if connection closes
   */
  size_t write(const void* buff, size_t count);

  /*
   * @return the current side of the connection's sockaddr
   * @throws SockAddrException if syscall goes wrong
   */
  std::unique_ptr<sockaddr, std::function<void(sockaddr*)>> getSocketSockAddr() const ;
  /*
   * @return the other side of the connection's sockaddr
   * @throws SockAddrException if syscall goes wrong
   */
  std::unique_ptr<sockaddr, std::function<void(sockaddr*)>> getPeerSockAddr() const ;

};

#endif
