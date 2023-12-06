#ifndef ASCII_TYPE_H
#define ASCII_TYPE_H

#include <string>
#include "common/types/AbstractType.h"

class ASCIIReadType: public AbstractReadType {
  const static size_t BUFFER_SIZE = 1 << 16;
  size_t buffer_pos;
  size_t buffer_len;
  unsigned char *buffer;
  bool previously_sent_cr_for_lf;
public:
  /*
   * Constructor for the ASCIIReadType
   * @throws ReadFileException if open fails
   */
  ASCIIReadType(const std::string& path);
  ASCIIReadType(const ASCIIReadType& other) = delete;
  ASCIIReadType& operator = (const ASCIIReadType& other) = delete;
  ASCIIReadType(ASCIIReadType&& other);
  ASCIIReadType& operator = (ASCIIReadType&& other);
  /*
   * This function returns the next byte to be read in the specified FTP ASCII Type.
   * This function is buffered.
   * @return the next byte corresponding to the FTP ASCII type.
   * @throws ReadFileException if reading fails
   * @throws TypeIterationEndedException if all was read
   */
  virtual unsigned char read_next_byte();
  virtual ~ASCIIReadType();
};

class ASCIIWriteType: public AbstractWriteType {
  const static size_t BUFFER_SIZE = 1 << 16;
  size_t buffer_pos;
  unsigned char *buffer;
  bool previously_read_cr;
public:
  /*
   * Constructor for the ASCIIWriteType
   * @throws WriteFileException if open fails
   */
  ASCIIWriteType(const std::string& path);
  ASCIIWriteType(const ASCIIWriteType& other) = delete;
  ASCIIWriteType& operator = (const ASCIIWriteType& other) = delete;
  ASCIIWriteType(ASCIIWriteType&& other);
  ASCIIWriteType& operator = (ASCIIWriteType&& other);
  /*
   * This function  writes the next_byte from the FTP ASCII Type definition, and interprets it in order to be written to a disk file.
   * This function is buffered.
   * @throws WriteFileException if writing fails
   */
  virtual void write_next_byte(unsigned char next_byte);
  virtual ~ASCIIWriteType();
};

#endif
