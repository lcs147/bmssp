#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
ROOT_DIR="$SCRIPT_DIR/.."
FILES_DIR="$ROOT_DIR/graphs"

FILES=(
    "USA-road-t.NY.gr"
    # "USA-road-t.BAY.gr"
    # "USA-road-t.COL.gr"
    # "USA-road-t.FLA.gr"
    # "USA-road-t.NW.gr"
    # "USA-road-t.NE.gr"
    # "USA-road-t.CAL.gr"
    # "USA-road-t.LKS.gr"
    # "USA-road-t.E.gr"
    # "USA-road-t.W.gr"
    # "USA-road-t.CTR.gr"
    # "USA-road-t.USA.gr"
    # "random4x.gr"
    # "random4x2.gr"
)
TYPES=(
    "dijkstra"
    "bmssp"
)

echo "Compiling experiment.cpp..."
g++ -std=c++20 -O3 ${ROOT_DIR}/experiment.cpp -o ${SCRIPT_DIR}/experiment_runner

if [ $? -ne 0 ]; then
    echo "Compilation failed. Exiting."
    exit 1
fi
echo "Compilation successful."

OUTPUT_FILE="${SCRIPT_DIR}/results.txt"
rm -f $OUTPUT_FILE

for TYPE in "${TYPES[@]}"; do
    for FILE in "${FILES[@]}"; do
        FILE_PATH="${FILES_DIR}/${FILE}"
        echo "Running experiment on ${FILE} with ${TYPE}"
        ${SCRIPT_DIR}/experiment_runner "${FILE_PATH}" "${TYPE}" >> $OUTPUT_FILE
    done
done

echo "All experiments finished. Results are in $OUTPUT_FILE"