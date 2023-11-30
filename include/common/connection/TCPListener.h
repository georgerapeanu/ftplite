#ifndef TCP_LISTENER_H
#define TCP_LISTENER_H

#include <stdint.h>

class TCPListener {
  int fd;
public:
  /* 
   * Constructor for TCP listener
   * @param port the port on the host to bind to
   * @throws SocketCreationException if socket creation fails
   */
  TCPListener(uint16_t port);
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
};

#endif
