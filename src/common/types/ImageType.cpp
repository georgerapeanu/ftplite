#include "common/types/ImageType.h"
#include "common/types/exceptions.h"
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <cstring>

ImageReadType::~ImageReadType() {
  free(this->buffer);
  AbstractReadType::~AbstractReadType();
}

ImageReadType::ImageReadType(const std::string& path) {
  this->fd = open(path.c_str(), O_RDONLY);
  this->buffer_pos = 0;
  this->buffer_len = 0;
  this->buffer = (unsigned char*)malloc(BUFFER_SIZE);
  memset(this->buffer, 0, BUFFER_SIZE);
}

ImageReadType::ImageReadType(ImageReadType&& other): AbstractReadType(std::move(other)) {
  this->buffer_pos = other.buffer_pos;
  this->buffer_len = other.buffer_len;
  this->buffer = other.buffer;

  other.buffer_pos = 0;
  other.buffer_len = 0;
  other.buffer = NULL;
}

ImageReadType& ImageReadType::operator = (ImageReadType&& other) {
  AbstractReadType::operator=(std::move(other));
  
  this->buffer_pos = other.buffer_pos;
  this->buffer_len = other.buffer_len;
  this->buffer = other.buffer;

  other.buffer_pos = 0;
  other.buffer_len = 0;
  other.buffer = NULL;

  return *this;
}

unsigned char ImageReadType::read_next_byte() {
  if(this->fd == -1) {
    throw TypeIterationEndedException("Attempt to read from closed file");
  }

  if(this->buffer_pos == this->buffer_len) {
    ssize_t cnt = read(this->fd, this->buffer, BUFFER_SIZE);
    if(cnt == -1) {
      throw ReadFileException("File read error happened");
    }

    if(cnt == 0) {
      close(this->fd);
      throw TypeIterationEndedException("Read whole file");
    }

    this->buffer_pos = 0;
    this->buffer_len = (size_t)cnt;
  }
  return this->buffer[this->buffer_pos++];
}

ImageWriteType::~ImageWriteType() {
  if(this->buffer_pos != 0) {
    write(this->fd, this->buffer, this->buffer_pos);
  }
  AbstractWriteType::~AbstractWriteType();
}

ImageWriteType::ImageWriteType(const std::string& path) {
  this->fd = open(path.c_str(), O_WRONLY | O_CREAT);
  this->buffer_pos = 0;
  this->buffer = (unsigned char*)malloc(BUFFER_SIZE);
  memset(this->buffer, 0, BUFFER_SIZE);
}

ImageWriteType::ImageWriteType(ImageWriteType&& other): AbstractWriteType(std::move(other)) {
  this->buffer_pos = other.buffer_pos;
  this->buffer = other.buffer;

  other.buffer_pos = 0;
  other.buffer = NULL;
}

ImageWriteType& ImageWriteType::operator = (ImageWriteType&& other) {
  AbstractWriteType::operator=(std::move(other));
  
  this->buffer_pos = other.buffer_pos;
  this->buffer = other.buffer;

  other.buffer_pos = 0;
  other.buffer = NULL;

  return *this;
}

void ImageWriteType::write_next_byte(unsigned char next_byte) {
  if(this->fd == -1) {
    throw WriteFileException("Attempt to write to closed file");
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

