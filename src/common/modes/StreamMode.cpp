#include "common/modes/StreamMode.h"
#include "common/types/exceptions.h"

#include <memory>
#include <cstring>
#include <string>

const size_t BUFFER_SIZE = 1 << 16;
void StreamMode::send(std::unique_ptr<AbstractConnection> connection, std::unique_ptr<AbstractReadType> read_type) {
  size_t buffer_pos = 0;
  unsigned char* buffer = (unsigned char*)malloc(BUFFER_SIZE);

  try {
    while(true) {
      unsigned char next_byte = read_type->read_next_byte();
      if(buffer_pos == BUFFER_SIZE) {
        connection->write(buffer, BUFFER_SIZE);
        buffer_pos = 0;
      }
      buffer[buffer_pos++] = next_byte;
    }
  } catch(const TypeIterationEndedException &ex) {
    ;
  } catch(const AppException &ex) {
    free(buffer);
    throw ex;
  }

  if(buffer_pos > 0) {
    connection->write(buffer, buffer_pos);
  }
  free(buffer);
}

void StreamMode::recv(std::unique_ptr<AbstractConnection> connection, std::unique_ptr<AbstractWriteType> write_type) {
  size_t buffer_len = 0;
  unsigned char* buffer = (unsigned char*)malloc(BUFFER_SIZE);

  while(true) {
    buffer_len = connection->read(buffer, BUFFER_SIZE);
    if(buffer_len == 0) {
      break;
    }
    for(size_t i = 0; i < buffer_len; i++) {
      write_type->write_next_byte(buffer[i]);
    }
  }
  free(buffer);
}
