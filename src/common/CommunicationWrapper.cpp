#include "common/CommunicationWrapper.h"
#include "common/exceptions.h"
#include <memory>
#include <string>

using namespace std;

CommunicationWrapper::CommunicationWrapper(std::unique_ptr<AbstractConnection> connection): connection(move(connection)) {
  this->buffer = (unsigned char*) malloc(BUFFER_SIZE);
  this->buffer_pos = 0;
  this->buffer_len = 0;
}

CommunicationWrapper& CommunicationWrapper::operator = (CommunicationWrapper&& other) {
  this->connection = move(other.connection);
  this->buffer = other.buffer;
  this->buffer_pos = other.buffer_pos;
  this->buffer_len = other.buffer_len;
  other.buffer = NULL;
  return *this;
}

CommunicationWrapper::CommunicationWrapper(CommunicationWrapper&& other) {
  this->connection = move(other.connection);
  this->buffer = other.buffer;
  this->buffer_pos = other.buffer_pos;
  this->buffer_len = other.buffer_len;
  other.buffer = NULL;
}

CommunicationWrapper::~CommunicationWrapper() {
  if(this->buffer != NULL) {
    free(this->buffer);
  }
}

bool CommunicationWrapper::checkCommandFinished(const string& command) {
  return command.size() >= 2 && command[command.size() - 2] == '\r' && command[command.size() - 1] == '\n';
}

std::string CommunicationWrapper::get_next_command() {
  std::string command = "";

  while(command.size() == 0) {
    command = "";
    while(command.size() <= MAX_COMMAND_SIZE && !checkCommandFinished(command)) {
      if(this->buffer_pos == this->buffer_len) {
        this->buffer_len = this->connection->read_non_blocking(this->buffer, BUFFER_SIZE);
        this->buffer_pos = 0;
      }

      if(this->buffer_pos == this->buffer_len) {
        this->buffer_len = this->connection->read(this->buffer, 1);
        this->buffer_pos = 0;
      }

      while(this->buffer_pos < this->buffer_len && command.size() <= MAX_COMMAND_SIZE && !checkCommandFinished(command)) {
        command += (char)this->buffer[this->buffer_pos++];
      }
    }

    if(command.size() > MAX_COMMAND_SIZE) {
      throw MaliciousCounterPartyException("Counterpary provided a malicious command(too long)");
    }

    command.pop_back();
    command.pop_back();
  }

  return command;
}

void CommunicationWrapper::send_next_command(const std::string& command) {
  string actual_command = command + "\r\n";
  this->connection->write(actual_command.c_str(), actual_command.size());
}

