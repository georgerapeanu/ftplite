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

optional<TCPListener> listener;
unique_ptr<AbstractConnection> server;

void sigint_handler(int) {
  server.~unique_ptr<AbstractConnection>();
  listener.~optional<TCPListener>();
  exit(0);
}

int main(int argc, char** argv) {
  signal(SIGCHLD, sigchld_handler);
  signal(SIGINT, sigint_handler);

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

  listener = TCPListener((uint16_t)port);

  while(true) {
    try {
      server = make_unique<PasiveStartedConnection>(*listener);
      if(fork() == 0) {
        auto pi = ServerProtocolInterpreter(move(server));
        pi.run();
        server.reset();
        pi.~ServerProtocolInterpreter();
        listener.~optional<TCPListener>();
        exit(0);
      }
      server->move_to_child();
    } catch(const AppException& ex) {
      cerr << ex.what();
    }
  }

  return 0;
}
