#ifndef ABSTRACT_TYPE_H
#define ABSTRACT_TYPE_H

/*
 * @class AbstractReadType this class is the abstract class that defined a read end of a type, as defined in the FTP RFC
 * This class is associated with a local file, and is used to read it.
 */
class AbstractReadType {
protected:
  AbstractReadType();
  int fd;
public:
  AbstractReadType(const AbstractReadType& other) = delete;
  AbstractReadType& operator = (const AbstractReadType& other) = delete;
  AbstractReadType(AbstractReadType&& other);
  AbstractReadType& operator = (AbstractReadType&& other);
  /*
   * This function returns the next byte to be read in the specified FTP Type.
   * It is recommended for this function to be buffered.
   * @return the next byte corresponding to the FTP Type.
   * @throws ReadFileException if reading fails, TypeIterationEndedException if all was read
   */
  virtual unsigned char read_next_byte() = 0;
  virtual ~AbstractReadType() = 0;
};

/*
 * @class AbstractReadType this class is the abstract class that defined a write end of a type, as defined in the FTP RFC
 * This class is associated with a local file, and is used to write to it.
 */
class AbstractWriteType {
protected:
  AbstractWriteType();
  int fd;
public:
  AbstractWriteType(const AbstractWriteType& other) = delete;
  AbstractWriteType& operator = (const AbstractWriteType& other) = delete;
  AbstractWriteType(AbstractWriteType&& other);
  AbstractWriteType& operator = (AbstractWriteType&& other);
  /*
   * This function  writes the next_byte from the FTP Type definition, and interprets it in order to be written to a disk file.
   * It is recommended for this function to be buffered.
   * @throws WriteFileException if writing fails
   */
  virtual void write_next_byte(unsigned char next_byte) = 0;
  virtual ~AbstractWriteType() = 0;
};

#endif
