#!/bin/bash

# --- Configuration ---
# Determine script, root, and output directories based on the script's location.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
ROOT_DIR="$SCRIPT_DIR/.."
FILES_DIR="$ROOT_DIR/graphs"
EXECUTABLE="${SCRIPT_DIR}/a"
GENERATOR_SOURCE="${ROOT_DIR}/experiments/grid-generator.cpp"


echo "--- 1. Compiling generator ---"
g++ -std=c++20 -O3 "${GENERATOR_SOURCE}" -o "${EXECUTABLE}"

if [ $? -ne 0 ]; then
    echo "ERROR: Compilation failed. Exiting."
    exit 1
fi
echo "Compilation successful. Executable created at: ${EXECUTABLE}"


# --- Dynamic N Generation ---
# Generates N values starting at 10, multiplying by 2, up to 30,000,000.
N_VALUES=()
N=8
MAX_N=5000

echo "--- 2. Generating N values: Starting at 8, multiplying by 2 up to ${MAX_N} ---"

while (( N <= MAX_N )); do
    N_VALUES+=("$N")
    # Using arithmetic expansion to multiply N by 2
    N=$(( N * 2 ))
done

echo "Generated Ns: ${N_VALUES[*]}"
# -----------------------------


if [ ! -d "$FILES_DIR" ]; then
    echo "Creating output directory: ${FILES_DIR}"
    mkdir -p "$FILES_DIR"
fi

echo -e "\n--- 3. Running generator for ${#N_VALUES[@]} square grid-graph euclidian distance ---"

for N in "${N_VALUES[@]}"; do
    OUTPUT_FILE="${FILES_DIR}/random${N}X${N}SGridED.gr"
    
    echo "Generating grid-graph ${N}X${N} (Vertices). Output: ${OUTPUT_FILE}"

    # Arguments: N (n_row), N (n_col), 0 (Max weight), 1971 (Seed)
    "${EXECUTABLE}" "${N}" "${N}" 0 1971 > "${OUTPUT_FILE}"
    
    if [ $? -eq 0 ]; then
        echo "  [SUCCESS] Generated file size: $(du -h "${OUTPUT_FILE}" | cut -f1)"
    else
        echo "  [ERROR] Execution failed for ${N}X${N}."
    fi
done


echo -e "\n--- 4. Running generator for ${#N_VALUES[@]} square grid-graph with random distance  ---"

for N in "${N_VALUES[@]}"; do
    OUTPUT_FILE="${FILES_DIR}/random${N}X${N}SGridR.gr"
    
    echo "Generating grid-graph ${N}X${N} (Vertices). Output: ${OUTPUT_FILE}"

    # Arguments: N (n_row), N (n_col), 0 (Max weight), 1971 (Seed)
    "${EXECUTABLE}" "${N}" "${N}" 100000 1971 > "${OUTPUT_FILE}"
    
    if [ $? -eq 0 ]; then
        echo "  [SUCCESS] Generated file size: $(du -h "${OUTPUT_FILE}" | cut -f1)"
    else
        echo "  [ERROR] Execution failed for ${N}X${N}."
    fi
done

echo -e "\n--- 5. Running generator for ${#N_VALUES[@]} rectangular grid-graph euclidian distance  ---"

for N in "${N_VALUES[@]}"; do
    OUTPUT_FILE="${FILES_DIR}/random$((N/2))X$((N*2))RGridED.gr"

    echo "Generating grid-graph $((N/2))X$((N*2)) (Vertices). Output: ${OUTPUT_FILE}"

    "${EXECUTABLE}" "$((N/2))" "$((N*2))" 0 1971 > "${OUTPUT_FILE}"
    
    if [ $? -eq 0 ]; then
        echo "  [SUCCESS] Generated file size: $(du -h "${OUTPUT_FILE}" | cut -f1)"
    else
        echo "  [ERROR] Execution failed for $((N/2))X$((N*2))."
    fi
done

echo -e "\n--- 6. Running generator for ${#N_VALUES[@]} rectangular with random distance  ---"

for N in "${N_VALUES[@]}"; do
    OUTPUT_FILE="${FILES_DIR}/random$((N/2))X$((N*2))RGridR.gr"
    
    echo "Generating grid-graph $((N/2))X$((N*2)) (Vertices). Output: ${OUTPUT_FILE}"

    # Arguments: N (n_row), N (n_col), 0 (Max weight), 1971 (Seed)
    "${EXECUTABLE}" "$((N/2))" "$((N*2))" 0 1971 > "${OUTPUT_FILE}"
    
    if [ $? -eq 0 ]; then
        echo "  [SUCCESS] Generated file size: $(du -h "${OUTPUT_FILE}" | cut -f1)"
    else
        echo "  [ERROR] Execution failed for $((N/2))X$((N*2))."
    fi
done



echo -e "\n--- All graph generation tasks complete. ---"
echo "Generated files are located in: ${FILES_DIR}"
