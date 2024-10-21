#include <fstream>
#include <iostream>
#include "Scheduler.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "Scheduler.hpp"

void check_test_cases(const std::string& input_directory, const std::string& output_directory) {
    // Create the output directory if it doesn't exist
    std::filesystem::create_directories(output_directory);
    
    // Iterate over each file in the input directory
    for (const auto& entry : std::filesystem::directory_iterator(input_directory)) {
        if (entry.is_regular_file()) {
            const std::string& input_file_path = entry.path().string();
            std::ifstream input_file(input_file_path);
            if (!input_file.is_open()) {
                std::cerr << "Failed to open input file: " << input_file_path << std::endl;
                continue; 
            }

            // Process the input file using the Scheduler class
            Scheduler scheduler(input_file);
            scheduler.schedule();

            // Get the base filename of the input file (without directory and extension)
            std::string filename = entry.path().stem().string();

            // Create the output file path in the output directory
            std::string output_file_path = output_directory + "/" + filename + "_output.txt";
            std::ofstream output_file(output_file_path);
            if (!output_file.is_open()) {
                std::cerr << "Failed to open output file: " << output_file_path << std::endl;
                continue;
            }

            // Write the scheduler result (to_string) to the output file
            output_file << scheduler.to_string();
            if (scheduler.check_sequence()) {
                std::cerr << " Test Passed" << std::endl;
            } else {
                std::cerr << "Test Failed" << std::endl;
            }
            
            
        }
    }
}

int main(int argc, char* argv[]) {
    std::string input_directory;
    std::string output_directory;

    // Command line arguments for input and output directories
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-input") {
            input_directory = argv[i + 1];
        } else if (std::string(argv[i]) == "-output") {
            output_directory = argv[i + 1];
        }
    }

    if (input_directory.empty() || output_directory.empty()) {
        std::cerr << "Usage: " << argv[0] << " -input <input_directory> -output <output_directory>" << std::endl;
        return 1;
    }

    // Check test cases in the specified directory and log results
    check_test_cases(input_directory, output_directory);

    return 0;
}
