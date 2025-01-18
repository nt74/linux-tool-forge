#include <iostream>
#include <cstdlib>

// Function to check if a command is available
bool isCommandAvailable(const std::string& command) {
    std::string cmd = "command -v " + command + " > /dev/null 2>&1";
    return std::system(cmd.c_str()) == 0;
}

// Function to update the system packages for Arch Linux, CachyOS, and Manjaro
void updateArchBasedPackages() {
    std::cout << "Updating Arch-based Linux packages (Arch, CachyOS, Manjaro)..." << std::endl;

    // Check if pacman is available
    if (!isCommandAvailable("pacman")) {
        std::cerr << "Error: 'pacman' is not installed. Please install it to proceed." << std::endl;
        return;
    }

    // Check if paru is available
    bool paruAvailable = isCommandAvailable("paru");

    // Check if flatpak is available
    bool flatpakAvailable = isCommandAvailable("flatpak");

    // Update the package database and upgrade all packages
    int result = std::system("sudo pacman -Syu");
    if (result != 0) {
        std::cerr << "Failed to update packages with pacman." << std::endl;
        return;
    }

    // Update AUR packages managed by paru if available
    if (paruAvailable) {
        result = std::system("paru -Syu");
        if (result != 0) {
            std::cerr << "Failed to update packages with paru." << std::endl;
            return;
        }
    } else {
        std::cout << "'paru' is not installed, skipping AUR package update." << std::endl;
    }

    // Update flatpak packages if available
    if (flatpakAvailable) {
        result = std::system("flatpak update -y");
        if (result != 0) {
            std::cerr << "Failed to update packages with flatpak." << std::endl;
            return;
        }
    } else {
        std::cout << "'flatpak' is not installed, skipping flatpak package update." << std::endl;
    }

    std::cout << "Arch-based Linux system update complete." << std::endl;
}

// Function to update the system packages for Rocky Linux
void updateRockyLinuxPackages() {
    std::cout << "Updating Rocky Linux packages..." << std::endl;

    // Check if dnf is available
    if (!isCommandAvailable("dnf")) {
        std::cerr << "Error: 'dnf' is not installed. Please install it to proceed." << std::endl;
        return;
    }

    // Update the package database and upgrade all packages
    int result = std::system("sudo dnf update -y");
    if (result != 0) {
        std::cerr << "Failed to update packages with dnf." << std::endl;
        return;
    }

    std::cout << "Rocky Linux system update complete." << std::endl;
}

// Function to update the system packages for Debian-based distributions (Ubuntu, Debian)
void updateDebianBasedPackages() {
    std::cout << "Updating Debian-based Linux packages (Ubuntu, Debian)..." << std::endl;

    // Check if apt is available
    if (!isCommandAvailable("apt")) {
        std::cerr << "Error: 'apt' is not installed. Please install it to proceed." << std::endl;
        return;
    }

    // Check if snap is available
    bool snapAvailable = isCommandAvailable("snap");

    // Check if flatpak is available
    bool flatpakAvailable = isCommandAvailable("flatpak");

    // Update the package database and upgrade all packages
    int result = std::system("sudo apt update && sudo apt upgrade -y");
    if (result != 0) {
        std::cerr << "Failed to update packages with apt." << std::endl;
        return;
    }

    // Update snap packages if available
    if (snapAvailable) {
        result = std::system("sudo snap refresh");
        if (result != 0) {
            std::cerr << "Failed to update packages with snap." << std::endl;
            return;
        }
    } else {
        std::cout << "'snap' is not installed, skipping snap package update." << std::endl;
    }

    // Update flatpak packages if available
    if (flatpakAvailable) {
        result = std::system("flatpak update -y");
        if (result != 0) {
            std::cerr << "Failed to update packages with flatpak." << std::endl;
            return;
        }
    } else {
        std::cout << "'flatpak' is not installed, skipping flatpak package update." << std::endl;
    }

    std::cout << "Debian-based Linux system update complete." << std::endl;
}

// Function to print usage instructions
void printUsage() {
    std::cout << "Usage: update_system [arch|rocky|debian]" << std::endl;
    std::cout << "This binary updates all system packages for the specified distribution." << std::endl;
    std::cout << "Supported distributions:" << std::endl;
    std::cout << "  - arch: Updates packages using 'pacman', 'paru', and 'flatpak' if they are installed (for Arch Linux, CachyOS, Manjaro)." << std::endl;
    std::cout << "  - rocky: Updates packages using 'dnf' (for Rocky Linux)." << std::endl;
    std::cout << "  - debian: Updates packages using 'apt', 'snap', and 'flatpak' if they are installed (for Debian, Ubuntu)." << std::endl;
    std::cout << "You will need to enter your sudo password to execute the update commands." << std::endl;
}

int main(int argc, char* argv[]) {
    // Print usage instructions if requested or if no argument is provided
    if (argc != 2 || std::string(argv[1]) == "--help") {
        printUsage();
        return 0;
    }

    // Determine which distribution to update based on the argument
    std::string distro = argv[1];
    if (distro == "arch") {
        updateArchBasedPackages();
    } else if (distro == "rocky") {
        updateRockyLinuxPackages();
    } else if (distro == "debian") {
        updateDebianBasedPackages();
    } else {
        std::cerr << "Error: Unsupported distribution specified." << std::endl;
        printUsage();
    }

    return 0;
}
