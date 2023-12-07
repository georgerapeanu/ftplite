#include "common/types/ASCIIType.h"
#include "common/types/exceptions.h"
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cstring>

ASCIIReadType::~ASCIIReadType() {
  if(this->buffer != NULL) {
    free(this->buffer);
  }
  if(this->fd != -1) {
    close(this->fd);
  }
  AbstractReadType::~AbstractReadType();
}

ASCIIReadType::ASCIIReadType(const std::string& path) {
  this->buffer = NULL;
  this->fd = open(path.c_str(), O_RDONLY);
  if(this->fd == -1) {
    throw ReadFileException("Error when attempting to open read file");
  }
  this->buffer_pos = 0;
  this->buffer_len = 0;
  this->buffer = (unsigned char*)malloc(BUFFER_SIZE);
  this->previously_sent_cr_for_lf = false;
  if(this->buffer == NULL) {
    close(this->fd);
    throw ReadFileException("Error allocating buffer");
  }
  memset(this->buffer, 0, BUFFER_SIZE);
}

ASCIIReadType::ASCIIReadType(ASCIIReadType&& other): AbstractReadType(std::move(other)) {
  this->buffer_pos = other.buffer_pos;
  this->buffer_len = other.buffer_len;
  this->buffer = other.buffer;
  this->previously_sent_cr_for_lf = other.previously_sent_cr_for_lf;

  other.buffer_pos = 0;
  other.buffer_len = 0;
  other.buffer = NULL;
  other.previously_sent_cr_for_lf = false;
}

ASCIIReadType& ASCIIReadType::operator = (ASCIIReadType&& other) {
  AbstractReadType::operator=(std::move(other));
  
  this->buffer_pos = other.buffer_pos;
  this->buffer_len = other.buffer_len;
  this->buffer = other.buffer;
  this->previously_sent_cr_for_lf = other.previously_sent_cr_for_lf;

  other.buffer_pos = 0;
  other.buffer_len = 0;
  other.buffer = NULL;
  other.previously_sent_cr_for_lf = false;

  return *this;
}

unsigned char ASCIIReadType::read_next_byte() {
  if(this->fd == -1) {
    throw ReadFileException("Attempt to read from closed file");
  }

  if(this->buffer_pos == this->buffer_len) {
    ssize_t cnt = read(this->fd, this->buffer, BUFFER_SIZE);
    if(cnt == -1) {
      throw ReadFileException("File read error happened");
    }

    if(cnt == 0) {
      throw TypeIterationEndedException("Read whole file");
    }

    this->buffer_pos = 0;
    this->buffer_len = (size_t)cnt;
  }
  if(this->buffer[this->buffer_pos] == '\n' && this->previously_sent_cr_for_lf == false) {
    this->previously_sent_cr_for_lf = true;
    return '\r';
  }
  this->previously_sent_cr_for_lf = false;
  return this->buffer[this->buffer_pos++];
}

ASCIIWriteType::~ASCIIWriteType() {
  if(this->previously_read_cr) {
    this->write_next_byte('\r');
  }
  if(this->fd != -1 && this->buffer_pos != 0) {
    write(this->fd, this->buffer, this->buffer_pos);
  }
 
  if(this->buffer != NULL) {
    free(this->buffer);
  }
  
  if(this->fd != -1) {
    close(this->fd);
  }
  AbstractWriteType::~AbstractWriteType();
}

ASCIIWriteType::ASCIIWriteType(const std::string& path) {
  this->buffer = NULL;
  this->fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);

  if(this->fd == -1) {
    throw WriteFileException("Error opening write file");
  }

  this->buffer_pos = 0;
  this->buffer = (unsigned char*)malloc(BUFFER_SIZE);
  this->previously_read_cr = false;
  if(buffer == NULL) {
    throw AppException("malloc failed");
  }
  memset(this->buffer, 0, BUFFER_SIZE);
}

ASCIIWriteType::ASCIIWriteType(ASCIIWriteType&& other): AbstractWriteType(std::move(other)) {
  this->buffer_pos = other.buffer_pos;
  this->buffer = other.buffer;
  this->previously_read_cr = other.previously_read_cr;

  other.buffer_pos = 0;
  other.buffer = NULL;
  other.previously_read_cr = false;
}

ASCIIWriteType& ASCIIWriteType::operator = (ASCIIWriteType&& other) {
  AbstractWriteType::operator=(std::move(other));
  
  this->buffer_pos = other.buffer_pos;
  this->buffer = other.buffer;
  this->previously_read_cr = other.previously_read_cr;

  other.buffer_pos = 0;
  other.buffer = NULL;
  other.previously_read_cr = false;

  return *this;
}

void ASCIIWriteType::write_next_byte(unsigned char next_byte) {
  if(this->fd == -1) {
    throw WriteFileException("Attempt to write to closed file");
  }

  if(next_byte == '\r') {
    if(previously_read_cr) {
      previously_read_cr = true;
    } else {
      previously_read_cr = true;
      return ;
    }
  } else {
    previously_read_cr = false;
  }
  

  if(this->buffer_pos == BUFFER_SIZE) {
    ssize_t cnt = write(this->fd, this->buffer, BUFFER_SIZE);
    if(cnt == -1 || cnt == 0) {
      throw WriteFileException("File write error happened");
    }
    this->buffer_pos = 0;
  }
  this->buffer[this->buffer_pos++] = next_byte;
}

