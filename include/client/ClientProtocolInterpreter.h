//
// Created by senzationall on 12/1/23.
//

#ifndef FTPLITE_CLIENTPROTOCOLINTERPRETER_H
#define FTPLITE_CLIENTPROTOCOLINTERPRETER_H


#include <memory>
#include <optional>
#include "common/CommunicationWrapper.h"
#include "common/connection/TCPListener.h"

enum TypeEnum {
    IMAGE
};

enum ModeEnum {
    STREAM
};

enum StruEnum {
    STRU_FILE
};

class ClientProtocolInterpreter {
    std::unique_ptr<CommunicationWrapper> connection;
    TypeEnum type;
    ModeEnum mode;
    StruEnum stru;
    std::optional<std::string> user;
    bool loggedIn;
    std::unique_ptr<sockaddr, std::function<void(sockaddr*)>> server_sock_addr;
    std::unique_ptr<sockaddr, std::function<void(sockaddr*)>> client_sock_addr;
    std::optional<TCPListener> data_listener;

    /*
     * Checks that a string uses only the portable filename character set
     * @returns true if only the portable filename character set is used
     */
    bool check_string_users_portable_filename_character_set(const std::string& s);

    void handle_type_command(const std::vector<std::string>& args);
    void handle_mode_command(const std::vector<std::string>& args);
    void handle_user_command(const std::vector<std::string>& args);
    void handle_pass_command(const std::vector<std::string>& args);
    void handle_quit_command(const std::vector<std::string>& args);
    void handle_port_command(const std::vector<std::string>& args);
    void handle_pasv_command(const std::vector<std::string>& args);
    void handle_stru_command(const std::vector<std::string>& args);
    void handle_retr_command(const std::vector<std::string>& args);
    void handle_stor_command(const std::vector<std::string>& args);
    void handle_noop_command(const std::vector<std::string>& args);
    void handle_not_implemented_command(const std::vector<std::string>& command);
    void first_login();

public:

    ClientProtocolInterpreter(std::unique_ptr<AbstractConnection> connection);
    /*
     * Runs the connection in an infinite loop, until connection closes or an error happens
     * @throws MaliciousCounterPartyException if the counterparty is malicious
     */
    void run();
};

#endif //FTPLITE_CLIENTPROTOCOLINTERPRETER_H
