#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <atomic>
#include <errno.h>

#include "logger.hpp"
#include "multiProcessHandler.hpp"

#define PORT 8080

// ---------------- Server State ----------------

enum class ServerState : int {
    STARTING,
    RUNNING,
    SHUTTING_DOWN,
    STOPPED
};

std::atomic<ServerState> server_state{ServerState::STOPPED};

// ---------------- Socket Init ----------------

int init_listen_socket() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        log_msg(LOG_ERROR, "socket() failed: %s\n", strerror(errno));
        return -1;
    }

    sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Handles TIME_WAIT on restart
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR,
                   &opt, sizeof(opt)) < 0) {
        log_msg(LOG_WARN,
                "setsockopt(SO_REUSEADDR) failed: %s\n",
                strerror(errno));
    }

    // bind
    if (bind(server_socket,
             (struct sockaddr *)&server_address,
             sizeof(server_address)) < 0) {

        log_msg(LOG_ERROR, "bind() failed: %s\n", strerror(errno));
        close(server_socket);
        return -1;
    }

    log_msg(LOG_INFO, "bind successful\n");

    // starts listening
    if (listen(server_socket, 128) < 0) {
        log_msg(LOG_ERROR, "listen() failed: %s\n", strerror(errno));
        close(server_socket);
        return -1;
    }

    return server_socket;
}

// ---------------- Main ----------------

int main() {

    // logger setup
    if (start_logger("access.log", LOG_INFO) < 0) {
        return 1;
    }

    // server starting
    server_state.store(ServerState::STARTING, std::memory_order_relaxed);
    log_msg(LOG_INFO, "server starting\n");

    int server_socket = init_listen_socket();
    if (server_socket < 0) {
        server_state.store(ServerState::STOPPED, std::memory_order_relaxed);
        log_msg(LOG_ERROR, "failed to init listen socket\n");
        stop_logger();
        return 1;
    }

    server_state.store(ServerState::RUNNING, std::memory_order_relaxed);
    log_msg(LOG_INFO, "server running\n");

    
    
}
