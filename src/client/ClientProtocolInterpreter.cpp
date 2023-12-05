//
// Created by senzationall on 12/1/23.
//

#include "client/ClientProtocolInterpreter.h"
#include "common/connection/ActiveStartedConnection.h"
#include "common/connection/PasiveStartedConnection.h"
#include "common/connection/exceptions.h"
#include "common/exceptions.h"
#include "common/modes/StreamMode.h"
#include "common/types/AbstractType.h"
#include "common/types/ImageType.h"
#include "common/types/exceptions.h"
#include "client/exceptions.h"
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
#include <termios.h>


ClientProtocolInterpreter::ClientProtocolInterpreter(std::unique_ptr<AbstractConnection> connection):
        type(IMAGE),
        mode(STREAM),
        stru(STRU_FILE),
        user(std::nullopt),
        loggedIn(false),
        data_listener(std::nullopt) {

    this->server_sock_addr = connection->getPeerSockAddr();
    this->client_sock_addr = connection->getSocketSockAddr();
    CommunicationWrapper comm_wrapper(move(connection));
    this->connection = std::make_unique<CommunicationWrapper>(std::move(comm_wrapper));

    sockaddr_in* client_sock_addr_c_ptr = (sockaddr_in*)(client_sock_addr.release());
    client_sock_addr_c_ptr->sin_port = htons(21);
    client_sock_addr.reset((sockaddr*)client_sock_addr_c_ptr);
}

bool ClientProtocolInterpreter::check_string_users_portable_filename_character_set(const std::string& s) {

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

void ClientProtocolInterpreter::handle_user_command(const std::vector<std::string> &args) {
    std::string command = "USER ";
    std::string username;
    if(args.size() == 1) {
        if (this->check_string_users_portable_filename_character_set(args[0]))
            username += args[0];
        else
            throw LoginException("Invalid characters into username");
    }
    else{
        std::cout << "USER";
        std::cin >> username;
        if(!this->check_string_users_portable_filename_character_set(username))
            throw LoginException("Invalid characters into username");
    }
    command += username;

    // Send the command
    std::cout << "Name " + username + ": ";
    std::string usern;
    std::getline(std::cin, usern);
    connection->send_next_command(command);

}

void ClientProtocolInterpreter::handle_pass_command(const std::vector<std::string> &args) {
    std::string command = "PASS ";
    std::string password;
    do {
        std::cout << "Enter password: ";
        // Turn off terminal echoing
        struct termios old_termios, new_termios;
        tcgetattr(STDIN_FILENO, &old_termios);
        new_termios = old_termios;
        new_termios.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

        // Read password
        std::cin >> password;
        if(!std::cin) {
          throw AppException("Client over");
        }
        tcsetattr(STDIN_FILENO, TCSANOW, &old_termios); // Restore terminal settings

        std::cout << std::endl;
        command += password;
        connection->send_next_command(command);
        std::string response = connection->get_next_command();
        command = "PASS ";
        std::cout << response << std::endl;
        size_t dotPos = response.find('.');
        if (dotPos != std::string::npos && dotPos + 1 < response.size()) {
            std::string statusCode = response.substr(0, dotPos);
            // Check if the extracted status code is "230"
            if (statusCode == "230")
                // Status code is 230 (Login successful)
                break;
        }

    } while(true);
}

void ClientProtocolInterpreter::first_login() {
    std::vector<std::string> args;
    std::string username = getlogin();
    if(username.empty())
        throw LoginException("Failed to retrieve the username.");
    if(!this->check_string_users_portable_filename_character_set(username))
        throw LoginException("Invalid characters into username");
    args.emplace_back(username); // send the user that is logged in
    this->handle_user_command(args);
    std::cout << connection->get_next_command() << std::endl;
    args.pop_back(); // send empty args param for password
    this->handle_pass_command(args);
}

void ClientProtocolInterpreter::handle_type_command(const std::vector<std::string> &args) {
    std::string typeCommand = "TYPE ";
    if(args.size() != 1 || args[0].size() != 1) {
        std::cout << "500 Syntax error, command unrecognized.";
        return ;
    }
    typeCommand += args[0];
    this->connection->send_next_command(typeCommand);
    std::cout << connection->get_next_command() << std::endl;
}

void ClientProtocolInterpreter::handle_mode_command(const std::vector<std::string> &args) {
    std::string modeCommand = "MODE ";
    if(args.size() != 1 || args[0].size() != 1) {
        std::cout << "500 Syntax error, command unrecognized.";
        return ;
    }
    modeCommand += args[0];
    this->connection->send_next_command(modeCommand);
    std::cout << connection->get_next_command() << std::endl;
}

void ClientProtocolInterpreter::handle_port_command(const std::vector<std::string> &args) {
    std::string command = "PORT ";
    if(args.size() != 1){
        std::cout << "500 Syntax error, command unrecognized.";
        return ;
    }
    command += args[0];
    this->connection->send_next_command(command);
    std::cout << connection->get_next_command() << std::endl;
}


void ClientProtocolInterpreter::handle_stru_command(const std::vector<std::string> &args) {
    std::string struCommand = "STRU ";
    if(args.size() != 1 || args[0].size() != 1) {
        std::cout << "500 Syntax error, command unrecognized.";
        return ;
    }
    struCommand += args[0];
    this->connection->send_next_command(struCommand);
    std::cout << connection->get_next_command() << std::endl;
}


void ClientProtocolInterpreter::handle_retr_command(const std::vector<std::string> &args) {
    std::string command = "RETR ";
    if(args.size() != 1){
        std::cout << "500 Syntax error, command unrecognized.";
        return ;
    }
    if(!this->check_string_users_portable_filename_character_set(args[0])){
        std::cout << "500 Syntax error, command unrecognized.";
        return ;
    }
    command += args[0];
    this->connection->send_next_command(command);
    std::cout << connection->get_next_command() << std::endl;
}

void ClientProtocolInterpreter::handle_pasv_command(const std::vector<std::string> &args) {

}


void ClientProtocolInterpreter::handle_quit_command(const std::vector<std::string> &args) {
    this->connection->send_next_command("QUIT");
    std::cout << connection->get_next_command() << std::endl;
}



void ClientProtocolInterpreter::run() {
    try{

        std::cout << connection->get_next_command() << std::endl;
        this->first_login();

        std::string command;
        std::vector<std::string> emptyArgs;
        while(true){
            std::cout << "\nftp> ";
            std::string full_command;
            std::getline(std::cin, full_command); // Read the entire line

            if(!std::cin) {
              this->handle_quit_command(emptyArgs);
              break;
            }

            std::istringstream iss(full_command);
            std::vector<std::string> parts;
            std::string part;

            // Split the input command by spaces and store each part
            while (iss >> part) {
                parts.push_back(part);
            }

            if (parts.empty()) {
                // No command entered
                continue;
            }

            std::string main_command = parts[0]; // First part is the main command

            if (main_command == "QUIT") {
                this->handle_quit_command(emptyArgs);
                break;
            } else if (main_command == "USER") {
                // Handle USER command with arguments (if any)
                parts.erase(parts.begin()); // Remove the main command from parts
                this->handle_user_command(parts);
            } else if (main_command == "PASS") {
                // Handle PASS command with arguments (if any)
                parts.erase(parts.begin()); // Remove the main command from parts
                this->handle_pass_command(parts);
            } else if (main_command == "TYPE") {
                // Handle TYPE command with arguments (if any)
                parts.erase(parts.begin()); // Remove the main command from parts
                this->handle_type_command(parts);
            } else if (main_command == "MODE") {
                parts.erase(parts.begin());
                this->handle_mode_command(parts);
            } else if (main_command == "PORT") {
                parts.erase(parts.begin());
                this->handle_port_command(parts);
            } else if (main_command == "STRU") {
                parts.erase(parts.begin());
                this->handle_stru_command(parts);
            } else if (main_command == "RETR") {
                parts.erase(parts.begin());
                this->handle_retr_command(parts);
            }
            else {
                std::cout << "Unknown command: " << main_command << std::endl;
            }

        }
    }
    catch(const AppException& ex){
        std::cerr << ex.what() << "\n";
    }
}















