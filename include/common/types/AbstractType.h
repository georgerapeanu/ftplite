#ifndef ABSTRACT_TYPE_H
#define ABSTRACT_TYPE_H

class AbstractReadType {
protected:
  AbstractReadType();
  int fd;
public:
  AbstractReadType(const AbstractReadType& other) = delete;
  AbstractReadType& operator = (const AbstractReadType& other) = delete;
  AbstractReadType(AbstractReadType&& other);
  AbstractReadType& operator = (AbstractReadType&& other);
  virtual unsigned char read_next_byte() = 0;
  virtual ~AbstractReadType() = 0;
};

class AbstractWriteType {
protected:
  AbstractWriteType();
  int fd;
public:
  AbstractWriteType(const AbstractWriteType& other) = delete;
  AbstractWriteType& operator = (const AbstractWriteType& other) = delete;
  AbstractWriteType(AbstractWriteType&& other);
  AbstractWriteType& operator = (AbstractWriteType&& other);
  virtual void write_next_byte(unsigned char next_byte) = 0;
  virtual ~AbstractWriteType() = 0;
};

#endif
