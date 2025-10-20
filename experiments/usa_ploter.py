import re
import pandas as pd
import sys

def process_log_file(file_path):
    """
    Processes a log file, extracts Dijkstra and BMSPP times in microseconds (us), 
    converts them to milliseconds (ms), and calculates their ratio.
    """
    # Dictionary to store results, keyed by file name
    results = {}

    try:
        # Read all non-empty, stripped lines
        with open(file_path, 'r') as f:
            lines = [line.strip() for line in f if line.strip()]
    except FileNotFoundError:
        print(f"Error: File not found at '{file_path}'. Please check the path and try again.")
        return

    # Process lines in blocks of 4 (algorithm line, time line, std line, checksum line)
    i = 0
    while i < len(lines):
        # Line 1: Algorithm and file name
        line1 = lines[i]
        
        # Regex to extract algorithm ('dijkstra' or 'bmssp') and the file name (ending in '.gr')
        match = re.search(r'(dijkstra|bmssp) on (\S+\.gr)', line1)
        
        if match:
            algorithm = match.group(1)
            file_name = match.group(2)

            # Line 2: Time (still in us at this stage)
            if i + 1 < len(lines):
                line2 = lines[i + 1]
                time_match = re.search(r'time: (\d+) us', line2)
                
                if time_match:
                    time_us = int(time_match.group(1))

                    if file_name not in results:
                        results[file_name] = {}
                    # Store the time in us for calculation
                    results[file_name][algorithm] = time_us
            
        # Move to the next block of 4 lines
        i += 4

    # Prepare data for DataFrame
    data_list = []
    for file_name, times in results.items():
        dijkstra_time_us = times.get('dijkstra')
        bmssp_time_us = times.get('bmssp')
        
        # --- Conversion to Milliseconds (ms) ---
        dijkstra_time_ms = dijkstra_time_us / 1000 if dijkstra_time_us is not None else None
        bmssp_time_ms = bmssp_time_us / 1000 if bmssp_time_us is not None else None

        # Calculate ratio (ratio is independent of the unit, but we check for valid data)
        if dijkstra_time_us is not None and bmssp_time_us is not None and dijkstra_time_us != 0:
            ratio = bmssp_time_us / dijkstra_time_us
        else:
            ratio = None

        data_list.append({
            'File Name': file_name,
            # Store times in ms, formatted to 3 decimal places
            'Dijkstra Time (ms)': f'{dijkstra_time_ms:.3f}' if dijkstra_time_ms is not None else 'N/A',
            'BMSPP Time (ms)': f'{bmssp_time_ms:.3f}' if bmssp_time_ms is not None else 'N/A',
            'Ratio (BMSPP/Dijkstra)': ratio
        })

    df = pd.DataFrame(data_list)
    
    # Format the Ratio column to 3 decimal places
    df['Ratio (BMSPP/Dijkstra)'] = df['Ratio (BMSPP/Dijkstra)'].apply(
        lambda x: f'{x:.3f}' if pd.notna(x) else 'N/A'
    )
    
    # Print the table in markdown format
    print(df.to_markdown(index=False))

# Main execution block
if __name__ == "__main__":
    # Check if a file path argument was provided
    if len(sys.argv) != 2:
        print("Usage: python process_log_ms.py <path_to_your_log_file>")
        sys.exit(1)
    
    log_file_path = sys.argv[1]
    process_log_file(log_file_path)