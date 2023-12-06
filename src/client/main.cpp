#include "common/connection/ActiveStartedConnection.h"
#include "common/types/ImageType.h"
#include "common/modes/StreamMode.h"
#include "common/connection/TCPListener.h"
#include "common/connection/PasiveStartedConnection.h"
#include "client/ClientProtocolInterpreter.h"
#include "AppException.h"
#include <iostream>
#include <memory>
#include <csignal>
#include <sys/wait.h>
#include <regex>


void sigchld_handler(int) {
    wait(NULL);
}

std::optional<TCPListener> listener;
std::unique_ptr<AbstractConnection> client;

void sigint_handler(int) {
    client.~unique_ptr<AbstractConnection>();
    listener.~optional<TCPListener>();
    exit(0);
}

int main(int argc, char** argv) {
    signal(SIGCHLD, sigchld_handler);
    signal(SIGINT, sigint_handler);

    if(argc != 3) {
        std::cout << "Usage: " << argv[0] << " <HOST> <PORT>\n";
        return 1;
    }
    std::string host;
    std::regex pattern(
            "^(localhost|127\\.0\\.0\\.1|0\\.0\\.0\\.0|(25[0-5]|2[0-4][0-9]|[01]?[0-9]{1,2})\\."
            "(25[0-5]|2[0-4][0-9]|[01]?[0-9]{1,2})\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9]{1,2})\\."
            "(25[0-5]|2[0-4][0-9]|[01]?[0-9]{1,2}))$"
    );
    if(std::regex_match(argv[1], pattern))
        host = argv[1];
    else{
        std::cout << "Invalid host value\n";
        return 1;
    }


    int port = 0;
    try {
        port = stoi(std::string(argv[2]));
    } catch(std::exception) {
        std::cout << "Invalid port value\n";
        return 1;
    }

    if(port < 0 || port >= (1 << 16)) {
        std::cout << "Invalid port value\n";
    }


    try {
        client = std::make_unique<ActiveStartedConnection>(host, port);
        auto pi = ClientProtocolInterpreter(std::move(client));
        pi.run();
    } catch(const AppException& ex) {
        std::cerr << ex.what();
    }

    return 0;
}
