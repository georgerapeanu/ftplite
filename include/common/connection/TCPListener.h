#ifndef TCP_LISTENER_H
#define TCP_LISTENER_H

#include <stdint.h>

class TCPListener {
  int fd;
public:
  TCPListener(uint16_t port);
  TCPListener(const TCPListener& other) = delete;
  TCPListener& operator = (const TCPListener &other) = delete;
  TCPListener(TCPListener&& other);
  TCPListener& operator = (TCPListener &&other);
  ~TCPListener();
  int getFd()const;
};

#endif
