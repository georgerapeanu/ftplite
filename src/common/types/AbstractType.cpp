#include "common/types/AbstractType.h"
#include "common/types/exceptions.h"
#include <unistd.h>

AbstractReadType::AbstractReadType() {};

AbstractReadType::~AbstractReadType() {
  if(this->fd != -1) {
    close(this->fd);
  }
}

AbstractReadType::AbstractReadType(AbstractReadType&& other) {
  this->fd = other.fd;
  other.fd = -1;
}

AbstractReadType& AbstractReadType::operator = (AbstractReadType&& other) {
  this->fd = other.fd;
  other.fd = -1;

  return *this;
}

AbstractWriteType::AbstractWriteType() {};

AbstractWriteType::~AbstractWriteType() {
  if(this->fd != -1) {
    close(this->fd);
  }
}

AbstractWriteType::AbstractWriteType(AbstractWriteType&& other) {
  this->fd = other.fd;
  other.fd = -1;
}

AbstractWriteType& AbstractWriteType::operator = (AbstractWriteType&& other) {
  this->fd = other.fd;
  other.fd = -1;

  return *this;
}

