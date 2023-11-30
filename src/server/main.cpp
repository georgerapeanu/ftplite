#include "common/connection/PasiveStartedConnection.h"
#include "common/connection/TCPListener.h"
#include "common/types/ImageType.h"
#include "common/modes/StreamMode.h"
#include <iostream>
#include <memory>

using namespace std;

int main() {
  auto listener = TCPListener(27015);
  auto server = make_unique<PasiveStartedConnection>(listener);
  unique_ptr<AbstractWriteType> write_type = make_unique<ImageWriteType>("/home/georgerapeanu/Desktop/tmp/ftp_testing/othello.cpp");

  StreamMode::recv(move(server), move(write_type));
  return 0;
}
