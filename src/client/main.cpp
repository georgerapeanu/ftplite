#include "common/connection/ActiveStartedConnection.h"
#include <iostream>

using namespace std;

int main() {
  ActiveStartedConnection client("localhost", 27015);

  char msg[2048];

  client.write("hello from client", 16);
  size_t cnt = client.read(msg, 2048);
  msg[cnt] = 0;

  cout << msg;
  return 0;
}
