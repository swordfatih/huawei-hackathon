#include "Scheduler.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

void check_test_cases(const std::string& input_directory, const std::string& output_directory)
{
    // Create the output directory if it doesn't exist
    std::filesystem::create_directories(output_directory);

    // Iterate over each file in the input directory
    for(const auto& entry: std::filesystem::directory_iterator(input_directory))
    {
        if(entry.is_regular_file())
        {
            const std::string& input_file_path = entry.path().string();
            std::ifstream      input_file(input_file_path);
            if(!input_file.is_open())
            {
                std::cerr << "Failed to open input file: " << input_file_path << std::endl;
                continue;
            }

            // Process the input file using the Scheduler class
            Scheduler scheduler(input_file);
            scheduler.schedule();

            // Get the base filename of the input file (without directory and extension)
            std::string filename = entry.path().stem().string();

            // Create the output file path in the output directory
            std::string   output_file_path = output_directory + "/" + filename + "_output.txt";
            std::ofstream output_file(output_file_path);

            if(!output_file.is_open())
            {
                std::cerr << "\033[1;33m" << "Failed to open output file: " << output_file_path << "\033[0m" << std::endl;
                continue;
            }

            // Write the scheduler result (to_string) to the output file
            output_file << scheduler.to_string();

            bool sequence = scheduler.check_sequence();
            bool bandwidth = scheduler.check_bandwidth();
            if(sequence && bandwidth)
            {
                std::cerr << "\033[1;32m" << "Test " << "\033[0m" << filename << "\033[1;32m" << " passed with score " << scheduler.calculate_score() << "\033[0m" << std::endl;
            }
            else
            {
                std::cerr << "\033[1;31m" << "Test " << "\033[0m" << filename << "\033[1;31m" " failed" << "\033[0m" << std::endl;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    std::string input_directory;
    std::string output_directory;

    // Command line arguments for input and output directories
    for(int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if(arg == "-i" || arg == "--input" || arg == "-input")
        {
            input_directory = argv[i + 1];
        }
        else if(arg == "-o" || arg == "--output" || arg == "-output")
        {
            output_directory = argv[i + 1];
        }
    }

    if(input_directory.empty() || output_directory.empty())
    {
        // Process the input from standard input and output to standard output
        Scheduler scheduler(std::cin);
        scheduler.schedule();
        std::cout << scheduler.to_string();
    }
    else
    {
        // Check test cases in the specified directory and log results
        check_test_cases(input_directory, output_directory);
    }

    return 0;
}
