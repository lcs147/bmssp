import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os
import sys
import json
import re
from collections import defaultdict

# --- Configuration ---
OUTPUT_PLOT_FILENAME = "performance_plot.png"
OUTPUT_TABLE_FILENAME = "performance_summary_table.csv"
OUTPUT_DIR = "./outputs"
US_TO_MS_FACTOR = 1e-3

def parse_results_file(filename):
    """
    Parses the experiment results file from JSON format.
    Supports standard JSON, JSON with trailing commas, and Line-Delimited JSON (NDJSON).
    """
    parsed_data = []
    checksums = defaultdict(set)
    bad_checksum = ''
    try:
        raw_data = []
        with open(filename, 'r') as f:
            file_content = f.read()
            fixed_content = re.sub(r',\s*([\]}])', r'\1', file_content) # erase last comma, if there
            raw_data = json.loads(fixed_content)

        print(f"Successfully loaded {len(raw_data)} records.")

        for item in raw_data:
            algorithm = item.get('algorithm', 'Unknown')
            graph_file = item.get('graph_name', 'Unknown')
            n = item.get('n', 0)
            m = item.get('m', 0)
            time_us = float(item.get('time_us', 0.0))
            std_us = float(item.get('std_us', 0.0))
            
            checksum = str(item.get('checksum', ''))
            if(checksum == ''):
                bad_checksum += '@'
                checksum = bad_checksum

            entry = {
                "algorithm": algorithm,
                "graph_file": graph_file,
                "n": n,
                "m": m,
                "time": time_us * US_TO_MS_FACTOR,
                "std": std_us * US_TO_MS_FACTOR,
                "checksum": checksum
            }
            
            parsed_data.append(entry)
            checksums[graph_file].add(checksum)

    except FileNotFoundError:
        print(f"ERROR: Input file '{filename}' not found.")
        return None, None
    except Exception as e:
        print(f"ERROR: An unexpected error occurred while parsing: {e}")
        return None, None
        
    return parsed_data, checksums

def validate_checksums(checksums):
    is_valid = True
    print("\n--- Checksum Validation ---")
    for graph_file, cs_set in checksums.items():
        print(cs_set)
        if len(cs_set) > 1:
            print(f"❌ MISMATCH found for graph: {graph_file}")
            print(f"   Observed Checksums: {cs_set}")
            is_valid = False
    
    if is_valid:
        print("✅ All runs for the same graph produced a matching checksum.")
    else:
        print("\nACTION REQUIRED: Checksum mismatch detected. Results may be inconsistent.")
        
    return is_valid

def create_and_save_time_plot(df, filename, output_dir):
    print("\n--- Generating Execution Time Plot ---")
    try:
        full_filepath = os.path.join(output_dir, "time_" + filename)
        
        fig, ax = plt.subplots(figsize=(10, 6))

        for algo, group in df.groupby('algorithm'):
            ax.errorbar(
                group['n'], 
                group['time'], 
                yerr=group['std'], 
                fmt='-o', 
                capsize=5, 
                label=algo
            )

        ax.set_xscale('log')
        ax.set_xlabel('Graph Size (Number of Vertices)')
        ax.set_yscale('log')
        ax.set_ylabel('Execution Time (ms)')
        ax.legend(title='Algorithm')
        ax.grid(True, which="both", ls="--", linewidth=0.5)

        ax.set_xticks(sorted(df['n']))
        ax.set_xticklabels([f"{n:.1e}" for n in sorted(df['n'])])
        plt.setp(ax.get_xticklabels(), rotation=45, ha="right")

        plt.tight_layout()
        plt.savefig(full_filepath)
        print(f"Time plot saved successfully to: {full_filepath}")

    except Exception as e:
        print(f"ERROR during time plot generation: {e}")

def create_and_save_ratio_plot(df_ratio, filename, output_dir):
    """
    Generates and saves a plot of the ratio of bmssp time to dijkstra time.
    """
    print("\n--- Generating Ratio Plot ---")
    try:
        ratio_filename = "ratio_" + filename
        full_filepath = os.path.join(output_dir, ratio_filename)
        
        fig, ax = plt.subplots(figsize=(10, 6))

        ax.plot(
            df_ratio['n'],
            df_ratio['ratio'],
            marker='o',
            linestyle='-',
            color='r',
            label='BMSPP Time / Dijkstra Time'
        )

        ax.axhline(1.0, color='gray', linestyle='--', linewidth=0.8, label='Ratio = 1.0 (Break-Even)')

        ax.set_xscale('log')
        ax.set_xlabel('Graph Size (Number of Vertices - Log Scale)')
        ax.set_ylabel('Performance Ratio (BMSPP Time / Dijkstra Time)')
        ax.legend()
        ax.grid(True, which="both", ls="--", linewidth=0.5)

        ax.set_xticks(sorted(df['n']))
        ax.set_xticklabels([f"{n:.1e}" for n in sorted(df['n'])])
        plt.setp(ax.get_xticklabels(), rotation=45, ha="right")

        plt.tight_layout()
        plt.savefig(full_filepath)
        print(f"Ratio plot saved successfully to: {full_filepath}")

    except Exception as e:
        print(f"ERROR during ratio plot generation: {e}")

def save_performance_table(df, filename, output_dir):
    """
    Saves the performance summary table to a CSV file.
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

        print(df_formatted)
        
    except Exception as e:
        print(f"ERROR during table saving: {e}")


if __name__ == "__main__":
    
    if len(sys.argv) < 2:
        print("ERROR: Please provide the path to the results file as a command-line argument.")
        print("Usage: python performance_plotter.py <path_to_results.json>")
        sys.exit(1)
        
    INPUT_FILENAME = sys.argv[1]
    
    base_name = os.path.splitext(os.path.basename(INPUT_FILENAME))[0]
    OUTPUT_DIR = os.path.join(OUTPUT_DIR, base_name)
    
    try:
        os.makedirs(OUTPUT_DIR, exist_ok=True)
    except Exception as e:
        print(f"ERROR creating output directory: {e}")
        sys.exit(1)

    # 1. Parse the results
    parsed_results, checksum_map = parse_results_file(INPUT_FILENAME)

    if parsed_results:
        validate_checksums(checksum_map)
        
        df = pd.DataFrame(parsed_results)
        
        df['n'] = pd.to_numeric(df['n'], errors='coerce')
        df['time'] = pd.to_numeric(df['time'], errors='coerce')
        
        df = df[df['n'] >= 2**7] # filter too small graphs
        df = df.sort_values(by='n').reset_index(drop=True)
        
        if df.empty:
            print("Warning: No data remaining after filtering for big graphs.")
        else:
            create_and_save_time_plot(df, OUTPUT_PLOT_FILENAME, OUTPUT_DIR)

            df_pivot = df.pivot_table(index='n', columns='algorithm', values='time', aggfunc='mean').reset_index()
            df_pivot.columns.name = None

            if 'bmssp' in df_pivot.columns and 'dijkstra' in df_pivot.columns:
                df_pivot['ratio'] = df_pivot['bmssp'] / df_pivot['dijkstra']
                df_ratio = df_pivot.dropna(subset=['ratio']) 

                if not df_ratio.empty:
                    create_and_save_ratio_plot(df_ratio, OUTPUT_PLOT_FILENAME, OUTPUT_DIR)
                else:
                     print("\nSkipping Ratio Plot: Could not find matching data for ratio calculation.")
            else:
                print("\nSkipping Ratio Plot: 'bmssp' or 'dijkstra' data missing in results.")

            df_summary = df.groupby(['n', 'm', 'graph_file', 'algorithm'])['time'].mean().unstack(level='algorithm')

            df_summary = df_summary.reset_index()
            df_summary.columns.name = None
            print(df_summary)

            df_summary.rename(columns={
                'graph_file': 'Graph File',
                'dijkstra': 'Tempo Dijkstra (ms)',
                'bmssp': 'Tempo BMSPP (ms)',
                'n': 'Número de Vértices',
                'm': 'Número de Arestas'
            }, inplace=True)

            df_summary['Ratio BMSPP / Dijkstra'] = df_summary.get('Tempo BMSPP (ms)', np.nan) / df_summary.get('Tempo Dijkstra (ms)', np.nan)
            
            cols_order = [
                'Graph File',
                'Número de Vértices',
                'Número de Arestas',
                'Tempo Dijkstra (ms)',
                'Tempo BMSPP (ms)',
                'Ratio BMSPP / Dijkstra'
            ]

            final_table = df_summary[cols_order].sort_values(by='Número de Vértices').copy()

            # 7. Save the Final Table
            save_performance_table(final_table, OUTPUT_TABLE_FILENAME, OUTPUT_DIR)