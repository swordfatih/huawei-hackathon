import os
import re

# Directories
SRC_DIR = './src'
INCLUDE_DIR = './include'
MAIN_FILE = './main.cpp'  # Main entry file in the project root
OUTPUT_FILE = 'merged.cpp'  # Final combined file

# Regular expressions to detect includes
LOCAL_INCLUDE_REGEX = re.compile(r'#include\s+"([^"]+)"')
GLOBAL_INCLUDE_REGEX = re.compile(r'#include\s+<([^>]+)>')

# Used to track global includes
global_includes = set()

# Used to track processed local headers to avoid duplication
processed_headers = set()

def read_file(file_path):
    """Reads a file and returns its content."""
    with open(file_path, 'r') as file:
        return file.read()

def inline_local_includes(content, current_dir):
    """Recursively replaces local #includes with the actual file content."""
    lines = content.splitlines()
    result = []
    
    for line in lines:
        local_include_match = LOCAL_INCLUDE_REGEX.match(line)
        global_include_match = GLOBAL_INCLUDE_REGEX.match(line)

        if local_include_match:
            # Found a local include, process it
            header_file = local_include_match.group(1)
            header_path = os.path.join(INCLUDE_DIR, header_file)
            
            if header_file not in processed_headers and os.path.exists(header_path):
                processed_headers.add(header_file)
                header_content = read_file(header_path)
                # Inline the local header recursively
                result.append(f"// Inlined from {header_file}\n")
                result.append(inline_local_includes(header_content, os.path.dirname(header_path)))
            else:
                result.append(f"// Skipped duplicate or missing {header_file}")
        
        elif global_include_match:
            # Found a global include, include it only once
            global_include = global_include_match.group(1)
            if global_include not in global_includes:
                global_includes.add(global_include)
                result.append(line)  # Add global include
        else:
            result.append(line)  # Regular line, keep it
    
    return '\n'.join(result)

def combine_files():
    """Combines all .cpp and .hpp files into one main.cpp file."""
    combined_content = []

    # Process all source files
    for filename in os.listdir(SRC_DIR):
        if filename.endswith('.cpp'):
            file_path = os.path.join(SRC_DIR, filename)
            content = read_file(file_path)
            combined_content.append(f"// Source file: {filename}\n")
            combined_content.append(inline_local_includes(content, SRC_DIR))
            combined_content.append("\n")

    # Process the main entry file (./main.cpp)
    if os.path.exists(MAIN_FILE):
        main_content = read_file(MAIN_FILE)
        combined_content.append(f"// Main entry file: {MAIN_FILE}\n")
        combined_content.append(inline_local_includes(main_content, '.'))

    # Write the combined content to the output file
    with open(OUTPUT_FILE, 'w') as output_file:
        output_file.write("// Combined main_combined.cpp file\n\n")
        output_file.write('\n'.join(combined_content))

if __name__ == '__main__':
    combine_files()
    print(f"Combined file written to {OUTPUT_FILE}")
