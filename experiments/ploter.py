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
    
    try:
        raw_data = []
        with open(filename, 'r') as f:
            file_content = f.read()

        try:
            # Attempt to load as a single JSON array
            raw_data = json.loads(file_content)
        except json.JSONDecodeError:
            try:
                # Attempt to fix trailing commas (common in generated logs: ", ]" or ", }")
                fixed_content = re.sub(r',\s*([\]}])', r'\1', file_content)
                raw_data = json.loads(fixed_content)
            except json.JSONDecodeError:
                # Fallback: Attempt to read line-by-line (NDJSON)
                # We iterate over the lines of the original content
                for line in file_content.splitlines():
                    line = line.strip()
                    if line:
                        try:
                            raw_data.append(json.loads(line))
                        except json.JSONDecodeError:
                            continue # Skip malformed lines

        # If it loaded a single dict (not a list), wrap it
        if isinstance(raw_data, dict):
            raw_data = [raw_data]

        print(f"Successfully loaded {len(raw_data)} records.")

        for item in raw_data:
            # Extract fields based on the provided JSON schema
            algorithm = item.get('algorithm', 'Unknown')
            graph_file = item.get('graph_name', 'Unknown')
            
            # Use 'size' directly from JSON, fallback to 0 if missing
            graph_size = item.get('size', 0)
            
            # Extract time and convert to ms (assuming input is in microseconds based on 'time_us')
            time_us = float(item.get('time_us', 0.0))
            std_us = float(item.get('std_us', 0.0))
            
            checksum = str(item.get('checksum', ''))

            entry = {
                "algorithm": algorithm,
                "graph_file": graph_file,
                "graph_size": graph_size,
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
            # Only print verbose success for debugging, or keep it clean
            # print(f"✅ OK: {graph_file} (Checksum: {checksum_val})")
    
    if is_valid:
        print("✅ All runs for the same graph produced a matching checksum.")
    else:
        print("\nACTION REQUIRED: Checksum mismatch detected. Results may be inconsistent.")
        
    return is_valid

def create_and_save_time_plot(df, filename, output_dir):
    """
    Generates and saves the performance plot with error bars.
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

        ax.set_xscale('log')
        ax.set_xlabel('Graph Size (Number of Vertices)')
        ax.set_yscale('log')
        ax.set_ylabel('Execution Time (ms)')
        ax.legend(title='Algorithm')
        ax.grid(True, which="both", ls="--", linewidth=0.5)

        ax.set_xticks(sorted(df['graph_size'].unique()))
        
        def power_of_two_formatter(x, pos):
            # Safe log2 calculation
            if x <= 0: return ""
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
    Generates and saves a plot of the ratio of bmssp time to dijkstra time.
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

        ax.set_xscale('log')
        ax.set_xlabel('Graph Size (Number of Vertices - Log Scale)')
        ax.set_ylabel('Performance Ratio (BMSPP Time / Dijkstra Time)')
        ax.legend()
        ax.grid(True, which="both", ls="--", linewidth=0.5)

        ax.set_xticks(sorted(df_ratio['graph_size'].unique()))
        
        def power_of_two_formatter(x, pos):
            if x <= 0: return ""
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

        # Optional: Print simple preview
        # print(df_formatted.head())
        
    except Exception as e:
        print(f"ERROR during table saving: {e}")


if __name__ == "__main__":
    
    if len(sys.argv) < 2:
        print("ERROR: Please provide the path to the results file as a command-line argument.")
        print("Usage: python performance_plotter.py <path_to_results.json>")
        sys.exit(1)
        
    INPUT_FILENAME = sys.argv[1]
    
    # Improved output directory generation:
    # Creates a subfolder in ./outputs based on the input filename (without extension)
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
        # 2. Checksum Validation
        validate_checksums(checksum_map)
        
        # 3. Data Preparation
        df = pd.DataFrame(parsed_results)
        
        # Ensure we have numeric types
        df['graph_size'] = pd.to_numeric(df['n'], errors='coerce')
        df['time'] = pd.to_numeric(df['time'], errors='coerce')
        
        # Filter small graphs if necessary (kept from original logic)
        df = df[df['graph_size'] >= 2**7]
        df = df.sort_values(by='graph_size').reset_index(drop=True)
        
        if df.empty:
            print("Warning: No data remaining after filtering (graph_size >= 128).")
        else:
            # 4. Create Time Plot
            create_and_save_time_plot(df, OUTPUT_PLOT_FILENAME, OUTPUT_DIR)

            # 5. Create Ratio Plot
            df_pivot = df.pivot_table(index='graph_size', columns='algorithm', values='time', aggfunc='mean').reset_index()
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

            # 6. Data Preparation for Summary Table
            df_summary = df.groupby(['graph_size', 'algorithm'])['time'].mean().unstack(level='algorithm')
            df_summary.rename(columns={
                'dijkstra': 'Tempo Dijkstra (ms)',
                'bmssp': 'Tempo BMSPP (ms)'
            }, inplace=True)

            df_summary['Ratio BMSPP / Dijkstra'] = df_summary.get('Tempo BMSPP (ms)', np.nan) / df_summary.get('Tempo Dijkstra (ms)', np.nan)
            df_summary['Número de Vértices'] = df_summary.index
            # Assuming average degree of ~3 or similar metric from original code
            df_summary['Número de Arestas'] = df_summary['Número de Vértices'] * 3

            cols_to_keep = [
                'Número de Vértices',
                'Número de Arestas',
                'Tempo Dijkstra (ms)',
                'Tempo BMSPP (ms)',
                'Ratio BMSPP / Dijkstra'
            ]
            
            # Filter valid columns only
            valid_cols = [c for c in cols_to_keep if c in df_summary.columns]
            
            final_table = df_summary[valid_cols].sort_values(by='Número de Vértices').copy()
            
            # 7. Save the Final Table
            save_performance_table(final_table, OUTPUT_TABLE_FILENAME, OUTPUT_DIR)