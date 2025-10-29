import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import re
import os
import sys
from collections import defaultdict

# --- Configuration ---
OUTPUT_PLOT_FILENAME = "performance_plot.png"
OUTPUT_TABLE_FILENAME = "performance_summary_table.csv"
OUTPUT_DIR = "./outputs"
US_TO_MS_FACTOR = 1e-3
def parse_results_file(filename):
    """
    Parses the experiment results file line by line to extract data.
    Updated to handle 'us' time unit and negative checksums.
    """
    parsed_data = []
    checksums = defaultdict(set)
    current_entry = {}
    bad_size = 2
    try:
        id = 0
        parsed_data = []
        entry = {}
        with open(filename, 'r') as f:
            for line in f:
                line = line.strip()

                words = line.split(' ')

                if(id == 0):
                    algorithm = words[0]
                    graph_file = words[2]
                    graph_size_match = re.search(r'random(\d+)', graph_file)
                    graph_size = int(graph_size_match.group(1)) if graph_size_match else -1
                    if graph_size == -1:
                        graph_size = bad_size * 2
                        bad_size *= 2
                    entry = {
                        "algorithm": algorithm,
                        "graph_file": graph_file,
                        "graph_size": graph_size,
                    }
                elif id == 1:
                    entry['time'] = (float(words[1]) + 1) * US_TO_MS_FACTOR
                elif id == 2:
                    entry['std'] = (float(words[1])) * US_TO_MS_FACTOR
                elif id == 3:
                    entry['checksum'] = words[1]
                    checksums[entry["graph_file"]].add(words[1])
                    parsed_data.append(entry)
                    print(entry)
                    entry = {}

                id += 1
                if id == 4: id = 0

                    
    except FileNotFoundError:
        print(f"ERROR: Input file '{filename}' not found.")
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
        
        fig, ax = plt.subplots(figsize=(10, 6))

        for algo, group in df.groupby('algorithm'):
            ax.errorbar(
                group['graph_size'], 
                group['time'], 
                yerr=group['std'], 
                fmt='-o', 
                capsize=5, 
                label=algo
            )

        # Set plot properties
        ax.set_xscale('log')
        ax.set_xlabel('Graph Size (Number of Vertices)')
        ax.set_yscale('log')
        ax.set_ylabel('Execution Time (ms)')
        # ax.set_title('Algorithm Performance vs. Graph Size')
        ax.legend(title='Algorithm')
        ax.grid(True, which="both", ls="--", linewidth=0.5)

        # Ensure x-axis labels are readable
        ax.set_xticks(df['graph_size'].unique())
        
        # Custom formatter for power of two
        def power_of_two_formatter(x, pos):
            exponent = np.log2(x)
            return f'$2^{{{int(exponent)}}}$'
        
        ax.get_xaxis().set_major_formatter(plt.FuncFormatter(power_of_two_formatter))

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
        
        fig, ax = plt.subplots(figsize=(10, 6))

        ax.plot(
            df_ratio['graph_size'],
            df_ratio['ratio'],
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
        # ax.set_title('BMSPP vs. Dijkstra Performance Ratio')
        ax.legend()
        ax.grid(True, which="both", ls="--", linewidth=0.5)

        # Ensure x-axis labels are readable
        ax.set_xticks(df_ratio['graph_size'].unique())
        
        # Custom formatter for power of two
        def power_of_two_formatter(x, pos):
            exponent = np.log2(x)
            return f'$2^{{{int(exponent)}}}$'
        
        ax.get_xaxis().set_major_formatter(plt.FuncFormatter(power_of_two_formatter))

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
    OUTPUT_DIR += INPUT_FILENAME.split('.')[0].split('results')[1]
    
    # 1. Ensure the output directory exists
    try:
        os.makedirs(OUTPUT_DIR, exist_ok=True)
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
        df = df[df['graph_size'] >= 2**7]
        df = df.sort_values(by='graph_size').reset_index(drop=True)
        
        # 5. Create Time Plot
        create_and_save_time_plot(df, OUTPUT_PLOT_FILENAME, OUTPUT_DIR)

        # 6. Create Ratio Plot
        df_pivot = df.pivot(index='graph_size', columns='algorithm', values='time').reset_index()
        df_pivot.columns.name = None

        if 'bmssp' in df_pivot.columns and 'dijkstra' in df_pivot.columns:
            df_pivot['ratio'] = df_pivot['bmssp'] / df_pivot['dijkstra']
            df_ratio = df_pivot.dropna(subset=['ratio']) 

            if not df_ratio.empty:
                create_and_save_ratio_plot(df_ratio, OUTPUT_PLOT_FILENAME, OUTPUT_DIR)
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

        df_summary['Ratio BMSPP / Dijkstra'] = df_summary.get('Tempo BMSPP (ms)', np.nan) / df_summary.get('Tempo Dijkstra (ms)', np.nan)
        df_summary['Número de Vértices'] = df_summary.index
        df_summary['Número de Arestas'] = df_summary['Número de Vértices'] * 3

        final_table = df_summary[[
            'Número de Vértices',
            'Número de Arestas',
            'Tempo Dijkstra (ms)',
            'Tempo BMSPP (ms)',
            'Ratio BMSPP / Dijkstra'
        ]].sort_values(by='Número de Vértices').copy()
        
        # 8. Save the Final Table
        save_performance_table(final_table, OUTPUT_TABLE_FILENAME, OUTPUT_DIR)