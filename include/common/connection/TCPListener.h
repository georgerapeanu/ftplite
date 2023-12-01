#ifndef TCP_LISTENER_H
#define TCP_LISTENER_H

#include <functional>
#include <stdint.h>
#include <memory>
#include <sys/socket.h>

class TCPListener {
  int fd;
  int port;
public:
  /* 
   * Constructor for TCP listener
   * @param port the port on the host to bind to. If port is 0, it asks the kernel for an available port
   * @throws SocketCreationException if socket creation fails
   */
  TCPListener(uint16_t port);
  /* 
   * Constructor for TCP listener using sockaddr c struct
   * @throws SocketCreationException if socket creation fails
   */
  TCPListener(std::unique_ptr<sockaddr, std::function<void(sockaddr*)>> sock_addr);
  TCPListener(const TCPListener& other) = delete;
  TCPListener& operator = (const TCPListener &other) = delete;
  TCPListener(TCPListener&& other);
  TCPListener& operator = (TCPListener &&other);
  ~TCPListener();
  /*
   * Getter for file descriptor on which this is listening
   * @return the file descriptor on which this is listening
   */
  int getFd()const;

  /*
   * Getter for the port on which this is listening
   * @return the port on which this is listening
   */
  int getPort()const;
};

#endif
