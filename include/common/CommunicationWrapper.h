#ifndef COMMUNICATION_WRAPPER_H
#define COMMUNICATION_WRAPPER_H

#include "common/connection/AbstractConnection.h"
#include <memory>
#include <string>

/*
 * This is a wrapper around a connection in order to be easier to communicate.
 * It's sole purpose is to transforme tcp packets into strings and vice versa
 */
class CommunicationWrapper {
  std::unique_ptr<AbstractConnection> connection;
  const static size_t BUFFER_SIZE = 1 << 8;
  const static size_t MAX_COMMAND_SIZE = 1 << 8;
  size_t buffer_pos;
  size_t buffer_len;
  unsigned char* buffer;

  bool checkCommandFinished(const std::string& command);

public:
  CommunicationWrapper(std::unique_ptr<AbstractConnection> connection); 
  CommunicationWrapper& operator = (CommunicationWrapper&& other); 
  CommunicationWrapper(CommunicationWrapper&& other); 
  CommunicationWrapper(const CommunicationWrapper& other) = delete; 
  CommunicationWrapper& operator = (const CommunicationWrapper& other) = delete;  
  ~CommunicationWrapper(); 
  /*
   * This function gets the next command from the connection and returns it as string.
   * This function is buffered.
   * @throws ConnectionClosedException if the connection closed
   * @throws SocketOptionChangeException if changing the socket options fails
   * @throws MaliciousCounterPartyException if the other party deomonstrates malicious behavior
   */
  std::string get_next_command();

  /*
   * This function sends the next command over the connection.
   * This function is not buffered
   * @param command the command to be sent over the connection
   * @throws ConnectionClosedException if the connection closed
   */
  void send_next_command(const std::string& command);
};

#endif
