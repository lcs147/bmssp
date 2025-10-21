import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import re
import os
import sys
from collections import defaultdict
import matplotlib.ticker as mticker

# --- Configuration ---
OUTPUT_PLOT_FILENAME = "performance_plot.png"
OUTPUT_TABLE_FILENAME = "performance_summary_table.csv"
OUTPUT_DIR = "./outputs"
US_TO_MS_FACTOR = 1e-3

def parse_results_file(filename):
    """
    Parses the experiment results file line by line to extract data.
    Updated to handle the new format with algorithm, graph file, and size on one line.
    """
    parsed_data = []
    checksums = defaultdict(set)
    entry = {}
    id_counter = 0

    try:
        with open(filename, 'r') as f:
            for line in f:
                line = line.strip()
                if not line:  # Skip empty lines
                    continue

                words = line.split()

                if id_counter == 0:
                    # Line format: dijkstra on USA-road-t.E.gr source: 1 reps: 5 size: 3,598,623
                    if len(words) < 6 or words[1] != 'on' or words[-2] != 'size:':
                        print(f"WARNING: Skipping malformed header line: '{line}'")
                        continue
                    
                    algorithm = words[0]
                    graph_file = words[2]
                    # Find size, which is the last word, and remove commas
                    graph_size_str = words[-1].replace(',', '')
                    graph_size = int(graph_size_str)

                    entry = {
                        "algorithm": algorithm,
                        "graph_file": graph_file,
                        "graph_size": graph_size,
                    }
                elif id_counter == 1:
                    # Line format: time: 545761 us
                    entry['time'] = float(words[1]) * US_TO_MS_FACTOR
                elif id_counter == 2:
                    # Line format: std: 4068 us
                    entry['std'] = float(words[1]) * US_TO_MS_FACTOR
                elif id_counter == 3:
                    # Line format: checksum: 922352948308500119
                    entry['checksum'] = words[1]
                    checksums[entry["graph_file"]].add(words[1])
                    parsed_data.append(entry)
                    entry = {}

                id_counter = (id_counter + 1) % 4

    except FileNotFoundError:
        print(f"ERROR: Input file '{filename}' not found.")
        return None, None
    except (IndexError, ValueError) as e:
        print(f"ERROR: Could not parse line: '{line}'. Malformed file? Details: {e}")
        return None, None

    return parsed_data, checksums


def validate_checksums(checksums):
    """
    Checks if all runs on the same graph have the same checksum.
    """
    is_valid = True
    print("\n--- Checksum Validation ---")
    for graph_file, cs_set in checksums.items():
        if len(cs_set) > 1:
            print(f"❌ MISMATCH found for graph: {graph_file}")
            print(f"   Observed Checksums: {cs_set}")
            is_valid = False
        else:
            checksum_val = next(iter(cs_set), 'N/A')
            print(f"✅ OK: {graph_file} (Checksum: {checksum_val})")
    
    if is_valid:
        print("\nAll runs for the same graph produced a matching checksum.")
    else:
        print("\nACTION REQUIRED: Checksum mismatch detected. Results may be inconsistent.")
        
    return is_valid

def create_and_save_time_plot(df, filename, output_dir):
    """
    Generates and saves the performance plot with error bars to the specified directory.
    """
    print("\n--- Generating Execution Time Plot ---")
    try:
        full_filepath = os.path.join(output_dir, "time_" + filename)
        
        fig, ax = plt.subplots(figsize=(12, 7))

        for algo, group in df.groupby('algorithm'):
            sorted_group = group.sort_values('graph_size')
            ax.errorbar(
                sorted_group['graph_size'], 
                sorted_group['time'], 
                yerr=sorted_group['std'], 
                fmt='-o', 
                capsize=5, 
                label=algo
            )

        # Set plot properties
        ax.set_xscale('log') 
        ax.set_xlabel('Graph Size (Number of Vertices - Log Scale)')
        ax.set_yscale('log')
        ax.set_ylabel('Execution Time (ms)')
        ax.set_title('Algorithm Performance vs. Graph Size')
        ax.legend(title='Algorithm')
        ax.grid(True, which="both", ls="--", linewidth=0.5)

        # Format axes for better readability with non-power-of-two data
        ax.get_xaxis().set_major_formatter(mticker.ScalarFormatter())
        ax.get_xaxis().set_minor_formatter(mticker.NullFormatter())
        ax.set_xticks(df['graph_size'].unique())
        plt.setp(ax.get_xticklabels(), rotation=45, ha="right", rotation_mode="anchor")


        plt.tight_layout()
        plt.savefig(full_filepath)
        print(f"Time plot saved successfully to: {full_filepath}")

    except Exception as e:
        print(f"ERROR during time plot generation: {e}")

def create_and_save_ratio_plot(df_ratio, filename, output_dir):
    """
    Generates and saves a plot of the ratio of bmssp time to dijkstra time to the specified directory.
    """
    print("\n--- Generating Ratio Plot ---")
    try:
        ratio_filename = "ratio_" + filename
        full_filepath = os.path.join(output_dir, ratio_filename)
        
        fig, ax = plt.subplots(figsize=(12, 7))
        
        sorted_df = df_ratio.sort_values('graph_size')

        ax.plot(
            sorted_df['graph_size'],
            sorted_df['ratio'],
            marker='o',
            linestyle='-',
            color='r',
            label='BMSPP Time / Dijkstra Time'
        )

        ax.axhline(1.0, color='gray', linestyle='--', linewidth=0.8, label='Ratio = 1.0 (Break-Even)')

        # Set plot properties
        ax.set_xscale('log')
        ax.set_xlabel('Graph Size (Number of Vertices - Log Scale)')
        ax.set_ylabel('Performance Ratio (BMSPP Time / Dijkstra Time)')
        ax.set_title('BMSPP vs. Dijkstra Performance Ratio')
        ax.legend()
        ax.grid(True, which="both", ls="--", linewidth=0.5)

        # Format axes for better readability
        ax.get_xaxis().set_major_formatter(mticker.ScalarFormatter())
        ax.get_xaxis().set_minor_formatter(mticker.NullFormatter())
        ax.set_xticks(df_ratio['graph_size'].unique())
        plt.setp(ax.get_xticklabels(), rotation=45, ha="right", rotation_mode="anchor")


        plt.tight_layout()
        plt.savefig(full_filepath)
        print(f"Ratio plot saved successfully to: {full_filepath}")

    except Exception as e:
        print(f"ERROR during ratio plot generation: {e}")

def save_performance_table(df, filename, output_dir):
    """
    Saves the performance summary table to a CSV file in the specified directory.
    """
    full_filepath = os.path.join(output_dir, filename)
    
    print(f"\n--- Saving Performance Summary Table to {full_filepath} ---")
    try:
        df_formatted = df.copy()
        
        for col in ['Tempo Dijkstra (ms)', 'Tempo BMSPP (ms)', 'Ratio BMSPP / Dijkstra']:
            if col in df_formatted.columns:
                df_formatted[col] = df_formatted[col].apply(lambda x: f'{x:.3f}' if pd.notna(x) else '')
        
        df_formatted.to_csv(full_filepath, index=False)
        print(f"Table saved successfully to: {full_filepath}")

        print("\n--- Summary Table Preview (Markdown Format) ---")
        print(df_formatted.to_markdown(index=False))
        
    except Exception as e:
        print(f"ERROR during table saving: {e}")


if __name__ == "__main__":
    
    # Check if the filename argument was provided
    if len(sys.argv) < 2:
        print("ERROR: Please provide the path to the results file as a command-line argument.")
        print("Usage: python script_name.py <path_to_results_file.txt>")
        sys.exit(1)
        
    INPUT_FILENAME = sys.argv[1]
    
    # Create a more robust directory name from the input file
    base_name = os.path.basename(INPUT_FILENAME)
    dir_suffix = os.path.splitext(base_name)[0]
    
    # Check if 'results' is in the suffix and clean it up
    dir_suffix = dir_suffix.replace('results', '')
    
    OUTPUT_DIR_FINAL = f"{OUTPUT_DIR}{dir_suffix}"

    
    # 1. Ensure the output directory exists
    try:
        os.makedirs(OUTPUT_DIR_FINAL, exist_ok=True)
    except Exception as e:
        print(f"ERROR creating output directory: {e}")
        sys.exit(1)

    # 2. Parse the results
    parsed_results, checksum_map = parse_results_file(INPUT_FILENAME)

    if parsed_results:
        # 3. Checksum Validation
        validate_checksums(checksum_map)
        
        # 4. Data Preparation for Plotting
        df = pd.DataFrame(parsed_results)
        df = df.sort_values(by='graph_size').reset_index(drop=True)
        
        # Calculate and print the average time for each algorithm
        print("\n--- Average Execution Times ---")
        avg_times = df.groupby('algorithm')['time'].mean()
        for algo, avg_time in avg_times.items():
            print(f"Average time for {algo}: {avg_time:.3f} ms")

        # 5. Create Time Plot
        create_and_save_time_plot(df, OUTPUT_PLOT_FILENAME, OUTPUT_DIR_FINAL)

        # 6. Create Ratio Plot
        df_pivot = df.pivot(index='graph_size', columns='algorithm', values='time').reset_index()
        df_pivot.columns.name = None

        if 'bmssp' in df_pivot.columns and 'dijkstra' in df_pivot.columns:
            df_pivot['ratio'] = df_pivot['bmssp'] / df_pivot['dijkstra']
            df_ratio = df_pivot.dropna(subset=['ratio']) 

            if not df_ratio.empty:
                create_and_save_ratio_plot(df_ratio, OUTPUT_PLOT_FILENAME, OUTPUT_DIR_FINAL)
            else:
                 print("\nSkipping Ratio Plot: Could not find matching 'bmssp' and 'dijkstra' data for ratio calculation.")
        else:
            print("\nSkipping Ratio Plot: 'bmssp' or 'dijkstra' data missing in results.")

        # 7. Data Preparation for Summary Table
        df_summary = df.groupby(['graph_size', 'algorithm'])['time'].mean().unstack(level='algorithm')
        df_summary.rename(columns={
            'dijkstra': 'Tempo Dijkstra (ms)',
            'bmssp': 'Tempo BMSPP (ms)'
        }, inplace=True)
        
        # Add graph names to the summary
        graph_names = df.groupby('graph_size')['graph_file'].first()
        df_summary = df_summary.join(graph_names)


        df_summary['Ratio BMSPP / Dijkstra'] = df_summary.get('Tempo BMSPP (ms)', np.nan) / df_summary.get('Tempo Dijkstra (ms)', np.nan)
        df_summary.reset_index(inplace=True)
        df_summary.rename(columns={'graph_size': 'Número de Vértices', 'graph_file': 'Graph'}, inplace=True)

        final_table = df_summary[[
            'Graph',
            'Número de Vértices',
            'Tempo Dijkstra (ms)',
            'Tempo BMSPP (ms)',
            'Ratio BMSPP / Dijkstra'
        ]].sort_values(by='Número de Vértices').copy()
        
        # 8. Save the Final Table
        save_performance_table(final_table, OUTPUT_TABLE_FILENAME, OUTPUT_DIR_FINAL)

