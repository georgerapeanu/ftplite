#ifndef STREAM_MODE_H
#define STREAM_MODE_H

#include "common/connection/AbstractConnection.h"
#include "common/types/AbstractType.h"

#include <memory>

namespace StreamMode {
  const int BUFFER_SIZE = 1 << 16;
  void send(std::unique_ptr<AbstractConnection> connection, std::unique_ptr<AbstractReadType> read_type);
  void recv(std::unique_ptr<AbstractConnection> connection, std::unique_ptr<AbstractWriteType> write_type);
};

#endif
