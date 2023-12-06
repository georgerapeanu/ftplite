#include "server/ServerProtocolInterpreter.h"
#include "common/connection/ActiveStartedConnection.h"
#include "common/connection/PasiveStartedConnection.h"
#include "common/connection/exceptions.h"
#include "common/exceptions.h"
#include "common/modes/StreamMode.h"
#include "common/types/AbstractType.h"
#include "common/types/ImageType.h"
#include "common/types/exceptions.h"
#include "server/exceptions.h"
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include <pwd.h>
#include <shadow.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

ServerProtocolInterpreter::ServerProtocolInterpreter(std::unique_ptr<AbstractConnection> connection): 
  type(IMAGE), 
  mode(STREAM), 
  stru(STRU_FILE), 
  user(nullopt), 
  loggedIn(false),
  data_listener(nullopt) {

  this->server_sock_addr = connection->getSocketSockAddr();
  this->client_sock_addr = connection->getPeerSockAddr();
  CommunicationWrapper comm_wrapper(move(connection));
  this->connection = make_unique<CommunicationWrapper>(move(comm_wrapper));
  
  sockaddr_in* client_sock_addr_c_ptr = (sockaddr_in*)(client_sock_addr.release());
  client_sock_addr_c_ptr->sin_port = htons(21); 
  client_sock_addr.reset((sockaddr*)client_sock_addr_c_ptr);
}

bool ServerProtocolInterpreter::check_string_users_portable_filename_character_set(const string& s) {
  for(auto &it: s) {
    if('a' <= it && it <= 'z') {
      continue;
    }
    if('A' <= it && it <= 'Z') {
      continue;
    }
    if('0' <= it && it <= '9') {
      continue;
    }
    if(it == '.' || it == '_' || it == '-') {
      continue;
    }
    return false;
  }
  return true;
}

void ServerProtocolInterpreter::handle_type_command(const vector<string>& args) {
  if(!loggedIn) {
    connection->send_next_command("530 Not logged in.");
    return;
  }

  if(args.size() != 2 || args[1].size() != 1) {
    connection->send_next_command("500 Syntax error, command unrecognized.");
    return ;
  }
  
  char type_code = args[1][0];

  if(type_code != 'I') {
    connection->send_next_command("504 Command not implemented for that parameter.");
    return ;
  }

  if(type_code == 'I') {
    this->type = IMAGE;
  }

  connection->send_next_command("200 Command okay");
}

void ServerProtocolInterpreter::handle_mode_command(const vector<string>& args) {
  if(!loggedIn) {
    connection->send_next_command("530 Not logged in.");
    return;
  }
  

  if(args.size() != 2 || args[1].size() != 1) {
    connection->send_next_command("500 Syntax error, command unrecognized.");
    return ;
  }

  char mode_code = args[1][0];

  if(mode_code != 'S') {
    connection->send_next_command("504 Command not implemented for that parameter.");
    return ;
  }

  if(mode_code == 'S') {
    this->mode = STREAM;
  }
  
  connection->send_next_command("200 Command okay");
}

void ServerProtocolInterpreter::handle_stru_command(const vector<string>& args) {
  if(!loggedIn) {
    connection->send_next_command("530 Not logged in.");
    return;
  }
  
  if(args.size() != 2 || args[1].size() != 1) {
    connection->send_next_command("500 Syntax error, command unrecognized.");
    return ;
  }
  
  char stru_code = args[1][0];

  if(stru_code != 'F') {
    connection->send_next_command("504 Command not implemented for that parameter.");
    return ;
  }


  if(stru_code == 'F') {
    this->stru = STRU_FILE;
  }
  
  connection->send_next_command("200 Command okay");
}

void ServerProtocolInterpreter::handle_user_command(const vector<string>& args) {
  if(loggedIn) {
    connection->send_next_command("503 Bad sequence of commands.");
    return ;
  }
  
  if(args.size() != 2) {
    connection->send_next_command("500 Syntax error, command unrecognized.");
    return ;
  }
  string user = args[1];

  if(!check_string_users_portable_filename_character_set(user)) {
    throw MaliciousCounterPartyException("User character set is not allowed");
    return ;
  }
  
  if(getpwnam(user.c_str()) == NULL) {
    connection->send_next_command("530 Not logged in.");
    return ;
  }
  
  this->user = user;
  connection->send_next_command("331 User name okay, need password.");
}

void ServerProtocolInterpreter::handle_pass_command(const vector<string>& args) {
  if(loggedIn) {
    connection->send_next_command("503 Bad sequence of commands.");
    return ;
  }
  
  if(args.size() != 2) {
    connection->send_next_command("500 Syntax error, command unrecognized.");
    return ;
  }

  string pass = args[1];

  if(!check_string_users_portable_filename_character_set(pass)) {
    for(auto &it: pass) it = (char)0;
    throw MaliciousCounterPartyException("Pass character set is not allowed");
    return ;
  }
  
  if(user == nullopt) {
    for(auto &it: pass) it = (char)0;
    connection->send_next_command("530 Not logged in.");
    return ;
  }

  passwd* pwd = getpwnam(user->c_str());
  if(pwd == NULL) {
    for(auto &it: pass) it = (char)0;
    connection->send_next_command("530 Not logged in.");
    return ;
  }

  spwd* s_pwd = getspnam(user->c_str());

  if(s_pwd == NULL && errno == EACCES) {
    throw LoginException("Error accessing spwd file");
  }

  if(s_pwd != NULL) {
    pwd->pw_passwd = s_pwd->sp_pwdp;
  }

  char* encrypted = crypt(pass.c_str(), pwd->pw_passwd);
  for(auto &it: pass) it = (char)0;

  if(encrypted == NULL) {
    throw LoginException("Error crypt failed");
  }

  bool authOk = (strcmp(encrypted, pwd->pw_passwd) == 0);

  if(!authOk) {
    connection->send_next_command("530. Not logged in");
    return ;
  }

  setgid(pwd->pw_gid);
  setuid(pwd->pw_uid);
  loggedIn = true;
  connection->send_next_command("230. Login successful.");
}

void ServerProtocolInterpreter::handle_quit_command(const vector<string>&) {
  connection->send_next_command("221 Goodbye.");
  throw AppException("Session is over");
}

void ServerProtocolInterpreter::handle_port_command(const vector<string>& args) {
  if(!loggedIn) {
    connection->send_next_command("530 Not logged in.");
    return;
  }

  if(args.size() != 2) {
    connection->send_next_command("500 Syntax error, command unrecognized.");
    return ;
  }

  istringstream host_port_stream(args[1]);
  vector<int> host_port_args;
  string arg;
  while(getline(host_port_stream, arg, ',')) {
    try{
      host_port_args.push_back(stoi(arg));
    } catch(std::exception& ex) {
      connection->send_next_command("500 Syntax error, command unrecognized.");
      return ;
    }
  }
  if(host_port_args.size() != 6) {
    connection->send_next_command("500 Syntax error, command unrecognized.");
    return ;
  }

  for(const auto &it: host_port_args) {
    if(it < 0 || it > 255) {
      connection->send_next_command("500 Syntax error, command unrecognized.");
      return ;
    }
  }


  sockaddr_in* client_sock_addr_c_ptr = (sockaddr_in*)(client_sock_addr.release());
  client_sock_addr_c_ptr->sin_port = htons(uint16_t(host_port_args[4] * 256 + host_port_args[5])); 
  inet_aton(
    (
      to_string(host_port_args[0]) + "." +
      to_string(host_port_args[1]) + "." +
      to_string(host_port_args[2]) + "." +
      to_string(host_port_args[3])
    ).c_str(),
    &client_sock_addr_c_ptr->sin_addr
  );
  client_sock_addr.reset((sockaddr*)client_sock_addr_c_ptr);

  this->data_listener = nullopt;

  connection->send_next_command("200 Command okay.");
}

void ServerProtocolInterpreter::handle_pasv_command(const vector<string>& args) {
  if(!loggedIn) {
    connection->send_next_command("530 Not logged in.");
    return;
  }
  
  if(args.size() != 1) {
    connection->send_next_command("500 Syntax error, command unrecognized.");
    return ;
  }

  if(this->data_listener == nullopt) {
    this->data_listener = TCPListener(0); 
  }
  
  string host_string(inet_ntoa(((sockaddr_in*)(server_sock_addr.get()))->sin_addr));
  for(auto &it: host_string) { 
    if(it == '.') {
      it = ',';
    }
  }
 
  int port = this->data_listener->getPort();

  string port_string = to_string(port / 256) + "," + to_string(port % 256);

  connection->send_next_command("227 Entering Passive Mode (" + host_string + "," + port_string + ")");
}

void ServerProtocolInterpreter::handle_retr_command(const vector<string>& args) {
  if(!loggedIn) {
    this->connection->send_next_command("530 Not logged in.");
    return;
  }
  
  if(args.size() != 2) {
    this->connection->send_next_command("500 Syntax error, command unrecognized.");
    return ;
  }
  
  unique_ptr<AbstractReadType> read_type;
  try {
    switch(this->type) {
      case IMAGE: read_type = make_unique<ImageReadType>(args[1]);break;
    } 
  } catch(const ReadFileException& ex) {
    this->connection->send_next_command("550 Requested action not taken. File unavailable.");
    return ;
  }


  unique_ptr<AbstractConnection> connection;
  try {
    if(this->data_listener == nullopt) {
      this->connection->send_next_command("150 File status ok; about to open data connection");
      connection = make_unique<ActiveStartedConnection>(client_sock_addr);
    } else {
      this->connection->send_next_command("150 File status ok; Ok to send data");
      connection = make_unique<PasiveStartedConnection>(*this->data_listener, 5000);
    }
  } catch(SocketCreationException& ex) {
    this->connection->send_next_command("425 Can't open data connection.");
    return ;
  }

  switch(this->mode) {
    case STREAM: StreamMode::send(move(connection), move(read_type));break;
  }
  this->connection->send_next_command("226 Transfer complete.");
  // Compiler barrier to prevent reordering
}

void ServerProtocolInterpreter::handle_stor_command(const vector<string>& args) {
  if(!loggedIn) {
    this->connection->send_next_command("530 Not logged in.");
    return;
  }
  
  if(args.size() != 2) {
    this->connection->send_next_command("500 Syntax error, command unrecognized.");
    return ;
  }
  
  unique_ptr<AbstractWriteType> write_type;
  try {
    switch(this->type) {
      case IMAGE: write_type = make_unique<ImageWriteType>(args[1]);break;
    } 
  } catch(const WriteFileException& ex) {
    this->connection->send_next_command("550 Requested action not taken. File unavailable.");
    return ;
  }

  unique_ptr<AbstractConnection> connection;
  try {
    if(this->data_listener == nullopt) {
      this->connection->send_next_command("150 File status ok; about to open data connection");
      connection = make_unique<ActiveStartedConnection>(client_sock_addr);
    } else {
      this->connection->send_next_command("150 File status ok; Ok to send data");
      connection = make_unique<PasiveStartedConnection>(*this->data_listener, 5000);
    }
  } catch(SocketCreationException& ex) {
    this->connection->send_next_command("425 Can't open data connection.");
    return ;
  }

  switch(this->mode) {
    case STREAM: StreamMode::recv(move(connection), move(write_type));break;
  }
  this->connection->send_next_command("226 Transfer complete.");
};

void ServerProtocolInterpreter::handle_noop_command(const vector<string>& args) {
  if(args.size() != 1) {
    connection->send_next_command("500 Syntax error, command unrecognized.");
    return ;
  }
  connection->send_next_command("200 Command okay.");

}

void ServerProtocolInterpreter::handle_not_implemented_command(const vector<string>&) {
  connection->send_next_command("502 Command not implemented.");
}

void ServerProtocolInterpreter::run() {
  try{
    connection->send_next_command("220 (ftplite 1.0.0)");
    while(true) {
      auto command_stream = istringstream(this->connection->get_next_command());
      vector<string> args;
      string arg;
      while(command_stream >> arg) {
        args.push_back(arg);
      }

      if(args.empty()) {
        continue;
      }

      string command = args[0];

      if(command == "TYPE")       handle_type_command(args);
      else if(command == "MODE")  handle_mode_command(args);
      else if(command == "USER")  handle_user_command(args);
      else if(command == "QUIT")  handle_quit_command(args);
      else if(command == "PORT")  handle_port_command(args);
      else if(command == "STRU")  handle_stru_command(args);
      else if(command == "RETR")  handle_retr_command(args);
      else if(command == "STOR")  handle_stor_command(args);
      else if(command == "NOOP")  handle_noop_command(args);
      else if(command == "PASS")  handle_pass_command(args);
      else if(command == "PASV")  handle_pasv_command(args);
      else                        handle_not_implemented_command(args);
    }
  }
  catch(const AppException& ex) {
    cerr << ex.what() << "\n";
  }
}

