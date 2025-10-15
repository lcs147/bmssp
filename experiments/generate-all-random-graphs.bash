#!/bin/bash

# --- Configuration ---
# Determine script, root, and output directories based on the script's location.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
ROOT_DIR="$SCRIPT_DIR/.."
FILES_DIR="$ROOT_DIR/graphs"
EXECUTABLE="${SCRIPT_DIR}/a"
GENERATOR_SOURCE="${ROOT_DIR}/experiments/random-graph-generator.cpp"

N_VALUES=(  
            # 10
            # 100 1000 10000 
            # 100000 1000000 10000000
            20000000 30000000
        )

echo "--- 1. Compiling generator ---"
g++ -std=c++20 -O3 "${GENERATOR_SOURCE}" -o "${EXECUTABLE}"

if [ $? -ne 0 ]; then
    echo "ERROR: Compilation failed. Exiting."
    exit 1
fi
echo "Compilation successful. Executable created at: ${EXECUTABLE}"

if [ ! -d "$FILES_DIR" ]; then
    echo "Creating output directory: ${FILES_DIR}"
    mkdir -p "$FILES_DIR"
fi

# 3. Execution Loop
echo -e "\n--- 3. Running generator for N = ${N_VALUES[*]} ---"

for N in "${N_VALUES[@]}"; do
    # Define the output file path using the required format: random${N}D5
    OUTPUT_FILE="${FILES_DIR}/random${N}D5.gr"
    
    echo "Generating graph for N=${N} (Vertices). Output: ${OUTPUT_FILE}"

    "${EXECUTABLE}" "${N}" 5 100000 1 > "${OUTPUT_FILE}"
    
    if [ $? -eq 0 ]; then
        echo "  [SUCCESS] Generated file size: $(du -h "${OUTPUT_FILE}" | cut -f1)"
    else
        echo "  [ERROR] Execution failed for N=${N}."
    fi
done

echo -e "\n--- All graph generation tasks complete. ---"
echo "Generated files are located in: ${FILES_DIR}"
