#include "AppException.h"
#include "common/connection/PasiveStartedConnection.h"
#include "common/connection/TCPListener.h"
#include "common/types/ImageType.h"
#include "common/modes/StreamMode.h"
#include "server/ServerProtocolInterpreter.h"
#include <exception>
#include <iostream>
#include <memory>
#include <signal.h>
#include <sys/wait.h>

using namespace std;

void sigchld_handler(int) {
  wait(NULL);
}

void sigint_handler(int) {
  exit(0);
}

int main(int argc, char** argv) {
  signal(SIGCHLD, sigchld_handler);

  if(argc != 2) {
    cout << "Usage: " << argv[0] << " <PORT>\n";
    return 1;
  }
  int port = 0;
  try {
    port = stoi(string(argv[1]));
  } catch(std::exception) {
    cout << "Invalid port value\n";
    return 1;
  }

  if(port < 0 || port >= (1 << 16)) {
    cout << "Invalid port value\n";
  }

  auto listener = TCPListener((uint16_t)port);

  while(true) {
    try {
      unique_ptr<AbstractConnection> server = make_unique<PasiveStartedConnection>(listener);
      if(fork() == 0) {
        auto pi = ServerProtocolInterpreter(move(server));
        pi.run();
        server.reset();
        pi.~ServerProtocolInterpreter();
        exit(0);
      }
      server->move_to_child();
    } catch(const AppException& ex) {
      cerr << ex.what();
    }
  }

  return 0;
}
