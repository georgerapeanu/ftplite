#ifndef STREAM_MODE_H
#define STREAM_MODE_H

#include "common/connection/AbstractConnection.h"
#include "common/types/AbstractType.h"

#include <memory>

namespace StreamMode {
  const int BUFFER_SIZE = 1 << 16;
  /*
   * Sends a file using the read FTP type format over the connection
   * @throws ConnectionClosedException if connection closes
   * @throws ReadFileException if reading fails
   */
  void send(std::unique_ptr<AbstractConnection> connection, std::unique_ptr<AbstractReadType> read_type);
  /*
   * Receives a file using the write FTP type format over the connection
   * @throws ConnectionClosedException if connection closes
   * @throws WriteFileException if writing fails
   */
  void recv(std::unique_ptr<AbstractConnection> connection, std::unique_ptr<AbstractWriteType> write_type);
};

#endif
