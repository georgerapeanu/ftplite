#include "common/connection/ActiveStartedConnection.h"
#include "common/types/ImageType.h"
#include "common/modes/StreamMode.h"
#include <iostream>
#include <memory>

using namespace std;

int main() {
  unique_ptr<AbstractConnection> client = make_unique<ActiveStartedConnection>("localhost", 27015);
  unique_ptr<AbstractReadType> read_type = make_unique<ImageReadType>("/home/georgerapeanu/Desktop/tmp/a.out");

  StreamMode::send(move(client), move(read_type));
  return 0;
}
