#include <libssh/libssh.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>

struct SSHConfig {
    std::string hostname;
    std::string username;
    int port;
};

void showHelp() {
    std::cout << "Usage: ssh_waypipe [OPTIONS]\n"
              << "Options:\n"
              << "  -h, --help             Show this help message and exit\n"
              << "\nDescription:\n"
              << "  This binary SSHs to a remote host using the 'waypipe' command to enable remote Wayland applications.\n"
              << "\nConfiguration File:\n"
              << "  The configuration file should be located at '~/.config/ssh_waypipe/ssh_waypipe.conf' and have the following format:\n"
              << "    hostname=your_remote_host\n"
              << "    username=your_username\n"
              << "    port=your_port\n";
}

SSHConfig readConfig(const std::string& filepath) {
    SSHConfig config;
    std::ifstream configFile(filepath);
    if (!configFile.is_open()) {
        std::cerr << "Error opening config file: " << filepath << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(configFile, line)) {
        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '=')) {
            std::string value;
            if (std::getline(is_line, value)) {
                if (key == "hostname") {
                    config.hostname = value;
                } else if (key == "username") {
                    config.username = value;
                } else if (key == "port") {
                    config.port = std::stoi(value);
                }
            }
        }
    }
    configFile.close();
    return config;
}

bool isHostUp(const std::string& hostname) {
    std::stringstream ss;
    ss << "ping -c 1 " << hostname << " > /dev/null 2>&1";
    int result = std::system(ss.str().c_str());
    return result == 0;
}

void executeWaypipe(const SSHConfig& config) {
    std::stringstream ss;
    ss << "waypipe ssh " << config.username << "@" << config.hostname << " -p " << config.port;
    std::string command = ss.str();
    std::cout << "Executing command: " << command << std::endl;
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Failed to execute waypipe command" << std::endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "-h" || arg == "--help") {
            showHelp();
            return 0;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            showHelp();
            return 1;
        }
    }

    std::string homeDir = std::getenv("HOME");
    std::string configDir = homeDir + "/.config/ssh_waypipe";
    std::string configPath = configDir + "/ssh_waypipe.conf";

    // Ensure the config directory exists
    struct stat st;
    if (stat(configDir.c_str(), &st) != 0) {
        if (mkdir(configDir.c_str(), 0700) != 0) {
            std::cerr << "Error creating config directory: " << configDir << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    SSHConfig config = readConfig(configPath);
    
    // Check if the host is up
    if (!isHostUp(config.hostname)) {
        std::cerr << "The host " << config.hostname << " is down. Exiting." << std::endl;
        exit(EXIT_FAILURE);
    }

    executeWaypipe(config);
    return 0;
}
