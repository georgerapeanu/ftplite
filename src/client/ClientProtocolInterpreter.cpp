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
#include "common/types/ASCIIType.h"
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
        type(ASCII),
        mode(STREAM),
        stru(STRU_FILE),
        user(std::nullopt),
        loggedIn(false),
        data_listener(0) {

    this->server_sock_addr = connection->getPeerSockAddr();
    this->client_sock_addr = connection->getSocketSockAddr();
    CommunicationWrapper comm_wrapper(move(connection));
    this->connection = std::make_unique<CommunicationWrapper>(std::move(comm_wrapper));

    sockaddr_in* server_sock_addr_c_ptr = (sockaddr_in*)(server_sock_addr.release());
    server_sock_addr_c_ptr->sin_port = htons(21);
    server_sock_addr.reset((sockaddr*)server_sock_addr_c_ptr);
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
    std::string username = "";
    if(args.size() == 1) {
        if (this->check_string_users_portable_filename_character_set(args[0]))
            username = args[0];
        else
            throw LoginException("Invalid characters into username");
    }
    std::cout << "USER " << username << ":";
    // Send the command

    std::string user;
    std::getline(std::cin, user);

    if(user != "") {
        if(this->check_string_users_portable_filename_character_set(user))
            username = user;
        else
            throw LoginException("Invalid characters into username");
    }
    command += username;
    connection->send_next_command(command);

}

void ClientProtocolInterpreter::handle_pass_command(const std::vector<std::string> &args) {
    std::string command = "PASS ";
    std::string password;
    do {
        std::cout << "Enter password: ";
        // Turn off terminal echoing
        struct termios old_termios, new_termios;
        memset(&old_termios, 0, sizeof(old_termios));
        memset(&new_termios, 0, sizeof(new_termios));
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
        if(!this->check_string_users_portable_filename_character_set(password))
            throw LoginException("Invalid characters into password");
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
            if (statusCode == "230"){
                // Status code is 230 (Login successful)
                this->loggedIn = true;
                break;
            }

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
    args = {std::to_string(this->data_listener->getPort())};
    this->handle_port_command(args);
}

void ClientProtocolInterpreter::handle_type_command(const std::vector<std::string> &args) {

    std::string typeCommand = "TYPE ";
    if(args.size() != 1 || args[0].size() != 1) {
        std::cout << "Syntax error, command unrecognized.\nUSAGE TYPE [MODE]";
        return ;
    }
    if(!this->check_string_users_portable_filename_character_set(args[0])){
        std::cout << "Syntax error, invalid characters.\n";
        return ;
    }
    if(!loggedIn){
        std::cout << "Not logged in.\n";
        return ;
    }
    TypeEnum next_type = IMAGE;
    if(args[0] == "I") {
        next_type = IMAGE;
    } else if (args[0] == "A") {
        next_type = ASCII;
    } else {
        std::cout << "unrecognized type\n";
        return ;
    }
    typeCommand += args[0];
    this->connection->send_next_command(typeCommand);
    std::string response = this->connection->get_next_command();
    if(response.starts_with("200")) {
        this->type = next_type;
    }
    std::cout << response << std::endl;
}

void ClientProtocolInterpreter::handle_mode_command(const std::vector<std::string> &args) {
    std::string modeCommand = "MODE ";
    if(args.size() != 1 || args[0].size() != 1) {
        std::cout << "Syntax error, command unrecognized.\nUSAGE MODE [MODE]";
        return ;
    }

    if(args[0] != "S"){
        std::cout << "Command not implemented for that parameter.\n";
        return ;
    }
    if(!loggedIn){
        std::cout << "Not logged in.\n";
        return ;
    }

    modeCommand += args[0];
    this->connection->send_next_command(modeCommand);
    std::string result = connection->get_next_command();
    if(result.starts_with("200"))
        std::cout << "Command okay" << std::endl;
    else
        std::cout << "Bad sequence of commands.\n";
}

void ClientProtocolInterpreter::handle_port_command(const std::vector<std::string> &args) {
    if(args.size() != 1){
        std::cout << "Syntax error, command unrecognized.\nUSAGE PORT [PORT]";
        return ;
    }
    if(!loggedIn){
        std::cout << "Not logged in.\n";
        return ;
    }

    int port;
    try {
        port = std::stoi(args[0]);
    }
    catch (const std::exception& e){
        std::cout << e.what() << "\n";
        return;
    }

    if(this->data_listener) {
        this->data_listener = std::nullopt;
    }

    this->data_listener = TCPListener(port);

    port = this->data_listener->getPort();


    std::string host_string(inet_ntoa(((sockaddr_in*)(client_sock_addr.get()))->sin_addr));
    for(auto &it: host_string) {
        if(it == '.') {
            it = ',';
        }
    }

    std::string port_string = std::to_string(port / 256) + "," + std::to_string(port % 256);
    connection->send_next_command("PORT " + host_string + "," + port_string + "");
    std::cout << connection->get_next_command() << std::endl;
}


void ClientProtocolInterpreter::handle_stru_command(const std::vector<std::string> &args) {
    std::string struCommand = "STRU ";
    if(args.size() != 1 || args[0].size() != 1) {
        std::cout << "Syntax error, command unrecognized.\nUSAGE STRU [MODE]";
        return ;
    }
    if(!this->check_string_users_portable_filename_character_set(args[0])){
        std::cout << "Syntax error, illegal characters\n";
        return ;
    }
    if(args[0] != "F"){
        std::cout << "Command not implemented for that parameter.\n";
        return ;
    }
    if(!loggedIn){
        std::cout << "Not logged in.\n";
        return ;
    }

    struCommand += args[0];
    this->connection->send_next_command(struCommand);
    std::string result = connection->get_next_command();
    if(result.starts_with("200"))
        std::cout << "Command okay" << std::endl;
    else
        std::cout << "Bad sequence of commands.\n";
}


void ClientProtocolInterpreter::handle_retr_command(const std::vector<std::string> &args) {
    std::string command = "RETR ";
    if(args.size() != 1){
        std::cout << "Syntax error, command unrecognized.\nUSAGE RETR [FILE]";
        return ;
    }
    if(!this->check_string_users_portable_filename_character_set(args[0])){
        std::cout << "Syntax error, command unrecognized.\nUSAGE RETR [FILE]";
        return ;
    }
    if(!loggedIn){
        std::cout << "Not logged in.\n";
        return ;
    }
    command += args[0];
    this->connection->send_next_command(command);

    std::string result = this->connection->get_next_command();

    if (result.compare(0, 3, "150") == 0) {
        std::cout << result << std::endl;
    } else {
        std::cout << result << std::endl;
        return;
    }

    std::unique_ptr<AbstractConnection> connection;
    try {
        if(this->data_listener == std::nullopt) {
            connection = std::make_unique<ActiveStartedConnection>(server_sock_addr);
        } else {
            connection = std::make_unique<PasiveStartedConnection>(*this->data_listener, 5000);
        }
    } catch(SocketCreationException& ex) {
        std::cout << ex.what() << std::endl;
        return;
    }


    std::unique_ptr<AbstractWriteType> write_type;
    try {
        switch(this->type) {
            case IMAGE: write_type = make_unique<ImageWriteType>(args[0]);break;
            case ASCII: write_type = make_unique<ASCIIWriteType>(args[0]);break;
        }
    } catch(const WriteFileException& ex) {
        this->connection->send_next_command("Requested action not taken. File unavailable.");
        return ;
    }

    switch(this->mode) {
        case STREAM: StreamMode::recv(move(connection), move(write_type));break;
    }


    std::cout << this->connection->get_next_command() << std::endl;
}

void ClientProtocolInterpreter::handle_stor_command(const std::vector<std::string> &args) {
    std::string command = "STOR ";
    if(args.size() != 1){
        std::cout << "Syntax error, command unrecognized.\n USAGE STOR [FILE]";
        return ;
    }
    if(!this->check_string_users_portable_filename_character_set(args[0])){
        std::cout << "Syntax error, command unrecognized.\n USAGE STOR [FILE]";
        return ;
    }
    if(!loggedIn){
        std::cout << "Not logged in.\n";
        return ;
    }

    command += args[0];
    this->connection->send_next_command(command);
    std::string result = this->connection->get_next_command();
    if(result.starts_with("150")){
        std::cout << result << std::endl;
    }
    else{
        std::cout << result << std::endl;
        return;
    }

    std::unique_ptr<AbstractConnection> connection;
    try {
        if(this->data_listener == std::nullopt) {
            connection = std::make_unique<ActiveStartedConnection>(server_sock_addr);
        } else {
            connection = std::make_unique<PasiveStartedConnection>(*this->data_listener, 5000);
        }
    } catch(SocketCreationException& ex) {
        std::cout << ex.what() << std::endl;
        return;
    }


    std::unique_ptr<AbstractReadType> read_type;
    try {
        switch(this->type) {
            case IMAGE: read_type = make_unique<ImageReadType>(args[0]);break;
            case ASCII: read_type = make_unique<ASCIIReadType>(args[0]);break;
        }
    } catch(const ReadFileException& ex) {
        std::cout << ex.what() << std::endl;
        return ;
    }



    switch(this->mode) {
        case STREAM: StreamMode::send(move(connection), move(read_type));break;
    }

    std::cout << this->connection->get_next_command() << std::endl;
}

void ClientProtocolInterpreter::handle_noop_command(const std::vector<std::string> &args) {
    if (args.empty()){
        std::cout << "Syntax error, command unrecognized.\nUSAGE NOOP";
        return;
    }
    if(!loggedIn){
        std::cout << "Not logged in.\n";
        return ;
    }
    std::string command = "NOOP";
    this->connection->send_next_command(command);
    std::cout << connection->get_next_command() << std::endl;

}
void ClientProtocolInterpreter::handle_pasv_command(const std::vector<std::string> &args) {
    if (!args.empty()){
        std::cout << "Syntax error, command unrecognized.\n USAGE PASV";
        return;
    }
    if(!loggedIn){
        std::cout << "Not logged in.\n";
        return ;
    }
    std::string command = "PASV";
    this->connection->send_next_command(command);
    std::string response = this->connection->get_next_command();
    std::cout << response << " ";
    if(!response.starts_with("227 Entering Passive Mode (") || !response.ends_with(")")){
        std::cout << "Failed to enter passive mode";
        return;
    }
    int drop_prefix_len = std::string("227 Entering Passive Mode (").size();
    std::istringstream host_port_stream(response.substr(drop_prefix_len, response.size() -1 - drop_prefix_len));
    std::vector<int> host_port_args;
    std::string arg;
    while(getline(host_port_stream, arg, ',')) {
        try{
            host_port_args.push_back(stoi(arg));
        } catch(std::exception& ex) {
            std::cout << ex.what() << std::endl;
            return ;
        }
    }

    sockaddr_in* server_sock_addr_c_ptr = (sockaddr_in*)(server_sock_addr.release());
    server_sock_addr_c_ptr->sin_port = htons(uint16_t(host_port_args[4] * 256 + host_port_args[5]));
    std::cout << host_port_args[0] << "." << host_port_args[1] << "." << host_port_args[2] << "." << host_port_args[3] << ":" << ntohs(server_sock_addr_c_ptr->sin_port) << std::endl;
    inet_aton(
            (
                    std::to_string(host_port_args[0]) + "." +
                    std::to_string(host_port_args[1]) + "." +
                    std::to_string(host_port_args[2]) + "." +
                    std::to_string(host_port_args[3])
            ).c_str(),
            &server_sock_addr_c_ptr->sin_addr
    );
    server_sock_addr.reset((sockaddr*)server_sock_addr_c_ptr);

    this->data_listener = std::nullopt;
}


void ClientProtocolInterpreter::handle_quit_command(const std::vector<std::string> &args) {
    if (!args.empty()){
        std::cout << "Syntax error, command unrecognized.\n USAGE QUIT";
        return;
    }
    if(!loggedIn){
        std::cout << "Not logged in.\n";
        return ;
    }
    this->connection->send_next_command("QUIT");
    std::cout << connection->get_next_command() << std::endl;
}

void ClientProtocolInterpreter::handle_not_implemented_command(const std::vector<std::string> &args) {
    if(!args.empty() && this->check_string_users_portable_filename_character_set(args[0])) {
        connection->send_next_command(args[0]);
        std::cout << connection->get_next_command() << std::endl;
    }
    else{
        std::cout << "Syntax error, command unrecognized.";
        return;
    }
}

void ClientProtocolInterpreter::run() {
    try{

        std::cout << connection->get_next_command() << std::endl;
        this->first_login();

        std::string command;
        std::vector<std::string> emptyArgs;
        bool firstRun = true;
        while(true){
            if(firstRun)
                firstRun = false;
            else
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
            } else if (main_command == "STOR") {
                parts.erase(parts.begin());
                this->handle_stor_command(parts);
            } else if (main_command == "NOOP") {
                parts.erase(parts.begin());
                this->handle_noop_command(parts);
            } else if (main_command == "PASV") {
                parts.erase(parts.begin());
                this->handle_pasv_command(parts);
            }
            else {
                this->handle_not_implemented_command(parts);
            }

        }
    }
    catch(const AppException& ex){
        std::cerr << ex.what() << "\n";
    }
}
















