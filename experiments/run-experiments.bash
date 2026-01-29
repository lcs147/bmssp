#!/bin/bash

# Check if two arguments were provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <output_filename> <graph_substring>"
    echo "Example: $0 results_ny.txt NY"
    exit 1
fi

# --- Parameters ---
OUTPUT_BASENAME="$1"
SEARCH_SUBSTRING="$2"

# --- Setup ---
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
ROOT_DIR="$SCRIPT_DIR/.."
FILES_DIR="$ROOT_DIR/graphs"
OUTPUT_FILE="${SCRIPT_DIR}/${OUTPUT_BASENAME}" # Full path for the output file

# Find all files in $FILES_DIR that contain the specified substring
echo "Searching for files with '${SEARCH_SUBSTRING}' in $FILES_DIR..."
# The mapfile command reads the output of find into the FILES array
mapfile -t FILES < <(find "$FILES_DIR" -type f -name "*${SEARCH_SUBSTRING}*" -exec basename {} \;)

if [ ${#FILES[@]} -eq 0 ]; then
    echo "No files containing '${SEARCH_SUBSTRING}' found in $FILES_DIR. Exiting."
    exit 0
fi

echo "Found files: ${FILES[@]}"

TYPES=(
    "dijkstra"
    "bmssp"
)

# --- Compilation ---
echo "Compiling experiment.cpp..."
# Using the -o flag to place the executable directly in SCRIPT_DIR
g++ -std=c++20 -O3 "experiment.cpp" -o "${SCRIPT_DIR}/a"

if [ $? -ne 0 ]; then
    echo "Compilation failed. Exiting."
    exit 1
fi
echo "Compilation successful."

# echo "Setting virtual memory limit to unlimited..."
# ulimit -a

# --- Experiment Run ---
echo "Clearing previous results and preparing output file: ${OUTPUT_FILE}"
rm -f "$OUTPUT_FILE"

printf "[\n" >> "$OUTPUT_FILE"
for FILE in "${FILES[@]}"; do
    for TYPE in "${TYPES[@]}"; do
        FILE_PATH="${FILES_DIR}/${FILE}" 
        printf "Running experiment on ${FILE} with ${TYPE}\n"
        "${SCRIPT_DIR}/a" "$FILE_PATH" "$TYPE" 1 0 >> "$OUTPUT_FILE"
        printf ",\n" >> "$OUTPUT_FILE"
    done
done
printf "\n]" >> "$OUTPUT_FILE"
# --- Cleanup and Report ---
echo "All experiments finished. Results are in $OUTPUT_FILE"