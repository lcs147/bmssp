#!/bin/bash

# --- Configuration ---
# Determine script, root, and output directories based on the script's location.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
ROOT_DIR="$SCRIPT_DIR/.."
FILES_DIR="$ROOT_DIR/graphs"
EXECUTABLE="${SCRIPT_DIR}/a"
GENERATOR_SOURCE="${ROOT_DIR}/experiments/hamiltonian-random-graph-generator.cpp"

# --- Dynamic N Generation ---
# Generates N values starting at 10, multiplying by 2, up to 30,000,000.
N_VALUES=()
N=8
MAX_N=50000000

echo "--- 1. Generating N values: Starting at 8, multiplying by 2 up to ${MAX_N} ---"

while (( N <= MAX_N )); do
    N_VALUES+=("$N")
    # Using arithmetic expansion to multiply N by 2
    N=$(( N * 2 ))
done

echo "Generated Ns: ${N_VALUES[*]}"
# -----------------------------

echo "--- 2. Compiling generator ---"
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

echo -e "\n--- 3. Running generator for ${#N_VALUES[@]} graph sizes with 3D ---"

for N in "${N_VALUES[@]}"; do
    OUTPUT_FILE="${FILES_DIR}/H3random${N}.gr"
    
    echo "Generating graph for N=${N} (Vertices). Output: ${OUTPUT_FILE}"

    # Arguments: N (Vertices), 100000 (Max weight), 10 (% density) 1971 (Seed)
    "${EXECUTABLE}" "${N}" 100000 3 1971 > "${OUTPUT_FILE}"
    
    if [ $? -eq 0 ]; then
        echo "  [SUCCESS] Generated file size: $(du -h "${OUTPUT_FILE}" | cut -f1)"
    else
        echo "  [ERROR] Execution failed for N=${N}."
    fi
done

echo -e "\n--- 4. Running generator for ${#N_VALUES[@]} graph sizes with 5D density ---"

for N in "${N_VALUES[@]}"; do
    OUTPUT_FILE="${FILES_DIR}/H5random${N}.gr"
    
    echo "Generating graph for N=${N} (Vertices). Output: ${OUTPUT_FILE}"

    # Arguments: N (Vertices), 100000 (Max weight), 10 (% density) 1971 (Seed)
    "${EXECUTABLE}" "${N}" 100000 5 1971 > "${OUTPUT_FILE}"
    
    if [ $? -eq 0 ]; then
        echo "  [SUCCESS] Generated file size: $(du -h "${OUTPUT_FILE}" | cut -f1)"
    else
        echo "  [ERROR] Execution failed for N=${N}."
    fi
done


echo -e "\n--- 5. Running generator for ${#N_VALUES[@]} graph sizes with 10D density ---"

for N in "${N_VALUES[@]}"; do
    OUTPUT_FILE="${FILES_DIR}/H10random${N}.gr"
    
    echo "Generating graph for N=${N} (Vertices). Output: ${OUTPUT_FILE}"

    # Arguments: N (Vertices), 100000 (Max weight), 10 (density) 1971 (Seed)
    "${EXECUTABLE}" "${N}" 100000 10 1971 > "${OUTPUT_FILE}"
    
    if [ $? -eq 0 ]; then
        echo "  [SUCCESS] Generated file size: $(du -h "${OUTPUT_FILE}" | cut -f1)"
    else
        echo "  [ERROR] Execution failed for N=${N}."
    fi
done


echo -e "\n--- All graph generation tasks complete. ---"
echo "Generated files are located in: ${FILES_DIR}"
