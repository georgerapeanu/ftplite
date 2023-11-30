#ifndef IMAGE_TYPE_H
#define IMAGE_TYPE_H

#include <string>
#include "common/types/AbstractType.h"

class ImageReadType: public AbstractReadType {
  const static size_t BUFFER_SIZE = 1 << 16;
  size_t buffer_pos;
  size_t buffer_len;
  unsigned char *buffer;
public:
  ImageReadType(const std::string& path);
  ImageReadType(const ImageReadType& other) = delete;
  ImageReadType& operator = (const ImageReadType& other) = delete;
  ImageReadType(ImageReadType&& other);
  ImageReadType& operator = (ImageReadType&& other);
  virtual unsigned char read_next_byte();
  virtual ~ImageReadType();
};

class ImageWriteType: public AbstractWriteType {
  const static size_t BUFFER_SIZE = 1 << 16;
  size_t buffer_pos;
  unsigned char *buffer;
public:
  ImageWriteType(const std::string& path);
  ImageWriteType(const ImageWriteType& other) = delete;
  ImageWriteType& operator = (const ImageWriteType& other) = delete;
  ImageWriteType(ImageWriteType&& other);
  ImageWriteType& operator = (ImageWriteType&& other);
  virtual void write_next_byte(unsigned char next_byte);
  virtual ~ImageWriteType();
};

#endif
