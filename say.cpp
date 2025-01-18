#include <iostream>
#include <cstdlib>
#include <string>

void print_help() {
    std::cout << "Usage: say [-l language_code] <text to speak>\n";
    std::cout << "Options:\n";
    std::cout << "  -l, --lang    Specify the language code (default is 'en' for English)\n";
    std::cout << "  -h, --help    Display this help message\n";
    std::cout << "Available Languages:\n";

    // Execute gtts-cli --all to list available languages
    std::system("gtts-cli --all");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [-l language_code] <text to speak> or " << argv[0] << " [-h, --help] for help\n";
        return 1;
    }

    std::string lang = "en";  // Default language is English
    std::string text;
    bool langSpecified = false;

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-l" || arg == "--lang") {
            if (i + 1 < argc) {
                lang = argv[++i];
                langSpecified = true;
            } else {
                std::cerr << "Error: Language code must follow the -l or --lang option." << std::endl;
                return 1;
            }
        } else if (arg == "-h" || arg == "--help") {
            print_help();
            return 0;
        } else {
            if (!text.empty()) {
                text += " ";
            }
            text += arg;
        }
    }

    if (text.empty()) {
        std::cerr << "Error: No text provided for speech." << std::endl;
        return 1;
    }

    // Create the command to use gtts-cli with the specified language and suppress mpg123 output
    std::string command = "gtts-cli \"" + text + "\" --lang " + lang + " -o /tmp/output.mp3 && mpg123 -q /tmp/output.mp3 && rm /tmp/output.mp3";

    // Execute the command
    int result = std::system(command.c_str());
    if (result != 0) {
        std::cerr << "Failed to execute gtts-cli or play the audio." << std::endl;
        return 1;
    }

    return 0;
}
