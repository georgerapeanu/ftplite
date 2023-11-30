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
  /*
   * Constructor for the ImageReadType
   * @throws ReadFileException if open fails
   */
  ImageReadType(const std::string& path);
  ImageReadType(const ImageReadType& other) = delete;
  ImageReadType& operator = (const ImageReadType& other) = delete;
  ImageReadType(ImageReadType&& other);
  ImageReadType& operator = (ImageReadType&& other);
  /*
   * This function returns the next byte to be read in the specified FTP Image Type.
   * This function is buffered.
   * @return the next byte corresponding to the FTP Image type.
   * @throws ReadFileException if reading fails
   * @throws TypeIterationEndedException if all was read
   */
  virtual unsigned char read_next_byte();
  virtual ~ImageReadType();
};

class ImageWriteType: public AbstractWriteType {
  const static size_t BUFFER_SIZE = 1 << 16;
  size_t buffer_pos;
  unsigned char *buffer;
public:
  /*
   * Constructor for the ImageWriteType
   * @throws WriteFileException if open fails
   */
  ImageWriteType(const std::string& path);
  ImageWriteType(const ImageWriteType& other) = delete;
  ImageWriteType& operator = (const ImageWriteType& other) = delete;
  ImageWriteType(ImageWriteType&& other);
  ImageWriteType& operator = (ImageWriteType&& other);
  /*
   * This function  writes the next_byte from the FTP Image Type definition, and interprets it in order to be written to a disk file.
   * This function is buffered.
   * @throws WriteFileException if writing fails
   */
  virtual void write_next_byte(unsigned char next_byte);
  virtual ~ImageWriteType();
};

#endif
