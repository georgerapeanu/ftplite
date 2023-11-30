#include <iostream>
#include "common/connection/TCPListener.h"
#include "common/connection/PasiveStartedConnection.h"

using namespace std;

int main() {
  TCPListener listener(27015);
  PasiveStartedConnection server(listener);

  char msg[2048];

  server.write("hello from server", 16);
  size_t cnt = server.read(msg, 2048);
  msg[cnt] = 0;

  cout << msg;
  return 0;
}

