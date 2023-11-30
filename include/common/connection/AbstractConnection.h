#ifndef ABSTRACT_CONNECTION_H
#define ABSTRACT_CONNECTION_H

#include <sys/types.h>

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

  size_t read(void* buff, size_t count);
  size_t write(const void* buff, size_t count);
};

#endif
