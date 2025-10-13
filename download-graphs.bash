#!/bin/bash

DOWNLOAD_DIR="./graphs"

URLS=(
    "https://www.diag.uniroma1.it/~challenge9/data/USA-road-t/USA-road-t.NY.gr.gz"
    "https://www.diag.uniroma1.it/~challenge9/data/USA-road-t/USA-road-t.CAL.gr.gz"
    "https://www.diag.uniroma1.it/~challenge9/data/USA-road-t/USA-road-t.USA.gr.gz"
)

mkdir -p "$DOWNLOAD_DIR"

for URL in "${URLS[@]}"; do
    FINAL_FILENAME=$(basename "$URL" .gz)
    FINAL_FILE_PATH="$DOWNLOAD_DIR/$FINAL_FILENAME"

    if [ -f "$FINAL_FILE_PATH" ]; then
        echo "File '${FINAL_FILENAME}' already exists. Skipping."
    else
        echo "Downloading ${URL}..."
        wget -P "$DOWNLOAD_DIR" "$URL"
        
        DOWNLOADED_GZ_FILE="$DOWNLOAD_DIR/$(basename "$URL")"

        if [ -f "$DOWNLOADED_GZ_FILE" ]; then
            echo "Download complete. Extracting file..."
            gunzip "$DOWNLOADED_GZ_FILE"
            
            echo "Extraction complete. File is available at: ${FINAL_FILE_PATH}"
        else
            echo "Error: Download failed for ${URL}."
        fi
    fi
done

echo "All required files are present in $DOWNLOAD_DIR."