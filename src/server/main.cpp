#include "common/connection/PasiveStartedConnection.h"
#include "common/connection/TCPListener.h"
#include "common/types/ImageType.h"
#include "common/modes/StreamMode.h"
#include "server/ServerProtocolInterpreter.h"
#include <iostream>
#include <memory>

using namespace std;

int main() {
  auto listener = TCPListener(27015);
  unique_ptr<AbstractConnection> server = make_unique<PasiveStartedConnection>(listener);
  auto pi = ServerProtocolInterpreter(move(server));
  pi.run();
  return 0;
}
