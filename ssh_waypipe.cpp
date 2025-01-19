#include <libssh/libssh.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#include <ctime>
#include <getopt.h>

struct SSHConfig {
    std::string hostname;
    std::string username;
    int port;
};

// Function to log messages to a log file
void logMessage(const std::string &message, bool loggingEnabled) {
    if (loggingEnabled) {
        std::ofstream logFile("ssh_waypipe.log", std::ios_base::app);
        std::time_t now = std::time(nullptr);
        logFile << std::ctime(&now) << ": " << message << std::endl;
    }
}

// Function to display help message
void showHelp() {
    std::cout << "Usage: ssh_waypipe [OPTIONS]\n"
              << "Options:\n"
              << "  -h, --help             Show this help message and exit\n"
              << "  -c, --config FILE      Specify the configuration file\n"
              << "  --log                  Enable logging\n"
              << "\nDescription:\n"
              << "  This binary SSHs to a remote host using the 'waypipe' command to enable remote Wayland applications.\n"
              << "\nConfiguration File:\n"
              << "  The configuration file should be located at '~/.config/ssh_waypipe/ssh_waypipe.conf' and have the following format:\n"
              << "    hostname=your_remote_host\n"
              << "    username=your_username\n"
              << "    port=your_port\n";
}

// Function to read and parse the configuration file
SSHConfig readConfig(const std::string& filepath, bool loggingEnabled) {
    SSHConfig config;
    std::ifstream configFile(filepath);
    if (!configFile.is_open()) {
        std::cerr << "Error opening config file: " << filepath << std::endl;
        logMessage("Error opening config file: " + filepath, loggingEnabled);
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
                    try {
                        config.port = std::stoi(value);
                    } catch (const std::invalid_argument& e) {
                        std::cerr << "Invalid port value in config file" << std::endl;
                        logMessage("Invalid port value in config file", loggingEnabled);
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
    }
    configFile.close();

    if (config.hostname.empty() || config.username.empty() || config.port <= 0) {
        std::cerr << "Configuration file is missing required fields" << std::endl;
        logMessage("Configuration file is missing required fields", loggingEnabled);
        exit(EXIT_FAILURE);
    }

    return config;
}

// Function to check if the host is up using ncat
bool isHostUp(const std::string& hostname, int port, bool loggingEnabled) {
    std::stringstream ss;
    ss << "ncat -z -v " << hostname << " " << port << " > /dev/null 2>&1";
    std::string command = ss.str();
    int result = std::system(command.c_str());
    logMessage("Host check command: " + command, loggingEnabled);
    return result == 0;
}

// Function to execute the waypipe command
void executeWaypipe(const SSHConfig& config, bool loggingEnabled) {
    std::stringstream ss;
    ss << "waypipe ssh -p " << config.port << " " << config.username << "@" << config.hostname;
    std::string command = ss.str();
    std::cout << "Executing command: " << command << std::endl;
    logMessage("Executing command: " + command, loggingEnabled);
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Failed to execute waypipe command" << std::endl;
        logMessage("Failed to execute waypipe command", loggingEnabled);
    }
}

int main(int argc, char *argv[]) {
    std::string configFilePath;
    bool loggingEnabled = false;

    // Command-line options
    struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"config", required_argument, 0, 'c'},
        {"log", no_argument, 0, 'l'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "hc:l", long_options, NULL)) != -1) {
        switch (opt) {
        case 'h':
            showHelp();
            return 0;
        case 'c':
            configFilePath = optarg;
            break;
        case 'l':
            loggingEnabled = true;
            break;
        default:
            showHelp();
            return 1;
        }
    }

    if (configFilePath.empty()) {
        std::string homeDir = std::getenv("HOME");
        std::string configDir = homeDir + "/.config/ssh_waypipe";
        configFilePath = configDir + "/ssh_waypipe.conf";

        // Ensure the config directory exists
        struct stat st;
        if (stat(configDir.c_str(), &st) != 0) {
            if (mkdir(configDir.c_str(), 0700) != 0) {
                std::cerr << "Error creating config directory: " << configDir << std::endl;
                logMessage("Error creating config directory: " + configDir, loggingEnabled);
                exit(EXIT_FAILURE);
            }
        }
    }

    SSHConfig config = readConfig(configFilePath, loggingEnabled);

    // Check if the host is up
    if (!isHostUp(config.hostname, config.port, loggingEnabled)) {
        std::cerr << "The host " << config.hostname << " is down. Exiting." << std::endl;
        logMessage("The host " + config.hostname + " is down. Exiting.", loggingEnabled);
        exit(EXIT_FAILURE);
    }

    executeWaypipe(config, loggingEnabled);
    return 0;
}
