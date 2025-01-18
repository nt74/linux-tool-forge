#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <array>
#include <memory>
#include <cstring> // Include for strcmp

// Function to execute a shell command and capture its output
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

// Function to display help information
void display_help() {
    std::cout << "Usage: orphans-remove [OPTION]" << std::endl;
    std::cout << "Remove all orphan packages on Arch Linux." << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help    Display this help information" << std::endl;
}

int main(int argc, char* argv[]) {
    // Check if --help is passed as an argument
    if (argc > 1 && std::strcmp(argv[1], "--help") == 0) {
        display_help();
        return 0;
    }

    std::cout << "This program removes all orphan packages on Arch Linux." << std::endl;

    // Command to list orphan packages
    std::string list_orphans_cmd = "pacman -Qdtq";
    std::string orphans;

    try {
        orphans = exec(list_orphans_cmd.c_str());
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    // If no orphan packages are found
    if (orphans.empty()) {
        std::cout << "No orphan packages found." << std::endl;
        return 0;
    }

    // Splitting the result into individual package names
    std::istringstream iss(orphans);
    std::vector<std::string> packages((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());

    // Construct the remove command
    std::string remove_orphans_cmd = "sudo pacman -Rns";
    for (const auto& pkg : packages) {
        remove_orphans_cmd += " " + pkg;
    }

    // Execute the command to remove orphan packages
    int ret = system(remove_orphans_cmd.c_str());
    if (ret != 0) {
        std::cerr << "Failed to remove orphan packages." << std::endl;
        return 1;
    }

    std::cout << "Orphan packages removed successfully." << std::endl;
    return 0;
}
