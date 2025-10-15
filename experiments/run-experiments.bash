#!/bin/bash

# --- Setup ---
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
ROOT_DIR="$SCRIPT_DIR/.."
FILES_DIR="$ROOT_DIR/graphs"

# Find all files in $FILES_DIR that contain 'D3' in their name
echo "Searching for files with 'D3' in $FILES_DIR..."
# The mapfile command reads the output of find into the FILES array
mapfile -t FILES < <(find "$FILES_DIR" -type f -name "*D3*" -exec basename {} \;)

if [ ${#FILES[@]} -eq 0 ]; then
    echo "No files containing 'D3' found in $FILES_DIR. Exiting."
    exit 0
fi

echo "Found files: ${FILES[@]}"

TYPES=(
    "dijkstra"
    "bmssp"
)

# --- Compilation ---
echo "Compiling experiment.cpp..."
g++ -std=c++20 -O3 "${ROOT_DIR}/experiment.cpp" -o "${SCRIPT_DIR}/a"

if [ $? -ne 0 ]; then
    echo "Compilation failed. Exiting."
    exit 1
fi
echo "Compilation successful."

# --- Experiment Run ---
OUTPUT_FILE="${SCRIPT_DIR}/results_D3.txt" # Changed output filename for clarity
rm -f "$OUTPUT_FILE"

for FILE in "${FILES[@]}"; do
    for TYPE in "${TYPES[@]}"; do
        FILE_PATH="${FILES_DIR}/${FILE}" 
        echo "Running experiment on ${FILE} with ${TYPE}"
        "${SCRIPT_DIR}/a" "$FILE_PATH" "$TYPE" >> "$OUTPUT_FILE"
    done
done

# --- Cleanup and Report ---
echo "All experiments finished. Results are in $OUTPUT_FILE"