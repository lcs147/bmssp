#!/bin/bash

FILES_DIR="./graphs"

FILES=(
    "USA-road-t.NY.gr"
    "USA-road-t.CAL.gr"
    "USA-road-t.USA.gr"
    "random4x.gr"
    "random4x2.gr"
)
TYPES=(
    "bmssp"
    "dijkstra"
)

echo "Compiling experiment.cpp..."
g++ -std=c++20 -O3 experiment.cpp -o experiment_runner

if [ $? -ne 0 ]; then
    echo "Compilation failed. Exiting."
    exit 1
fi
echo "Compilation successful."

rm -f results.txt

for TYPE in "${TYPES[@]}"; do
    for FILE in "${FILES[@]}"; do
        FILE_PATH="${FILES_DIR}/${FILE}"
        echo "Running experiment on ${FILE} with ${TYPE}"
        ./experiment_runner "${FILE_PATH}" "${TYPE}" >> results.txt
    done
done

echo "All experiments finished. Results are in results.txt"