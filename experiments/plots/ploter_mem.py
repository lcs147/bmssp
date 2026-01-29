import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os
import sys
import json
import re
from collections import defaultdict

# --- Configuration ---
OUTPUT_PLOT_FILENAME = "memory_scaling_plot.png"
OUTPUT_TABLE_FILENAME = "memory_summary_table.csv"
OUTPUT_DIR = "../outputs"
B_TO_MB_FACTOR = 1 / (1024 * 1024)

algo_names = set({})
algname1 = ""
algname2 = ""
def parse_results_file(filename):
    parsed_data = []
    checksums = defaultdict(set)
    bad_checksum = ''
    try:
        with open(filename, 'r') as f:
            file_content = f.read()
            # Fix common JSON errors: double commas and trailing commas
            fixed_content = re.sub(r',\s*,', ',', file_content)
            fixed_content = re.sub(r',\s*([\]}])', r'\1', fixed_content)
            raw_data = json.loads(fixed_content)

        print(f"Successfully loaded {len(raw_data)} records.")

        for item in raw_data:
            algorithm = item.get('algorithm', 'Unknown')
            graph_file = item.get('graph_name', 'Unknown')
            n = item.get('n', 0)
            m = item.get('m', 0)
            memory_bytes = float(item.get('max_memory', 0.0))
            
            checksum = str(item.get('checksum', ''))
            if checksum == '':
                bad_checksum += '@'
                checksum = bad_checksum

            algo_names.add(algorithm)
            entry = {
                "algorithm": algorithm,
                "graph_file": graph_file,
                "n": n,
                "m": m,
                "memory_mb": memory_bytes * B_TO_MB_FACTOR,
                "checksum": checksum
            }
            parsed_data.append(entry)
            checksums[graph_file].add(checksum)

    except Exception as e:
        print(f"ERROR: An unexpected error occurred while parsing: {e}")
        return None, None
        
    return parsed_data, checksums

def create_and_save_memory_plot(df, filename, output_dir):
    print("\n--- Generating Memory Usage Plot ---")
    full_filepath = os.path.join(output_dir, "mem_" + filename)
    fig, ax = plt.subplots(figsize=(10, 6))

    for algo, group in df.groupby('algorithm'):
        ax.plot(group['n'], group['memory_mb'], '-o', label=algo)

    ax.set_xscale('log')
    ax.set_yscale('log')
    ax.set_xlabel('Graph Size (n)')
    ax.set_ylabel('Peak Memory Usage (MB)')
    ax.set_title('Memory Usage Comparison')
    ax.legend(title='Algorithm')
    ax.grid(True, which="both", ls="--", linewidth=0.5)

    ticks = sorted(df['n'].unique())
    ax.set_xticks(ticks)
    ax.set_xticklabels([f"{n:.1e}" for n in ticks])
    plt.setp(ax.get_xticklabels(), rotation=45, ha="right")

    plt.tight_layout()
    plt.savefig(full_filepath)
    print(f"Memory plot saved to: {full_filepath}")

def save_performance_table(df, filename, output_dir):
    full_filepath = os.path.join(output_dir, filename)
    print(f"\n--- Saving Memory Summary Table to {full_filepath} ---")
    df_formatted = df.copy()
    
    # Format decimals for cleaner CSV
    cols_to_format = [f'Mem {algname1} (MB)', 'Mem {algname2} (MB)', 'Ratio {algname2} / {algname1}']
    for col in cols_to_format:
        if col in df_formatted.columns:
            df_formatted[col] = df_formatted[col].apply(lambda x: f'{x:.2f}' if pd.notna(x) else 'N/A')
    
    df_formatted.to_csv(full_filepath, index=False)
    print(df_formatted)

def df_para_tabela_latex(df, nome_arquivo):
    cabecalho_latex = f"""
\\begin{{tabular}}{{lccccc}}
\hline
\\textbf{{Instance}} & \\textbf{{n}} & \\textbf{{m}} & \\textbf{{{algname1} (MB)}} & \\textbf{{{algname2} (MB)}} & \\textbf{{Ratio}} \\\\ \hline"""
        
    linhas_latex = []
    for _, row in df.iterrows():
        d_mem = f"{row[f'Mem {algname1} (MB)']:.2f}" if pd.notna(row[f'Mem {algname1} (MB)']) else "N/A"
        b_mem = f"{row[f'Mem {algname2} (MB)']:.2f}" if pd.notna(row[f'Mem {algname2} (MB)']) else "N/A"
        ratio = f"{row[f'Ratio {algname2} / {algname1}']:.3f}" if pd.notna(row[f'Ratio {algname2} / {algname1}']) else "N/A"
        
        linha = f"{row['Graph File']} & {row['n']} & {row['m']} & {d_mem} & {b_mem} & {ratio} \\\\"
        linhas_latex.append(linha)
    
    tabela_completa = cabecalho_latex + "\n" + "\n".join(linhas_latex) + "\n" + r"\hline" + "\n" + r"\end{tabular}"
    
    with open(nome_arquivo, 'w') as f:
        f.write(tabela_completa)
    print(f"LaTeX table saved to: {nome_arquivo}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python script_name.py <path_to_results.json>")
        sys.exit(1)
        
    INPUT_FILENAME = sys.argv[1]
    base_name = os.path.splitext(os.path.basename(INPUT_FILENAME))[0]
    OUTPUT_DIR = os.path.join(OUTPUT_DIR, base_name)
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    parsed_results, _ = parse_results_file(INPUT_FILENAME)
    
    names = list(algo_names)
    algname1, algname2 = names

    if parsed_results:
        df = pd.DataFrame(parsed_results)
        df = df.sort_values(by='n').reset_index(drop=True)

        # Plotting
        create_and_save_memory_plot(df, OUTPUT_PLOT_FILENAME, OUTPUT_DIR)

        # Pivoting for the summary table
        df_summary = df.pivot_table(
            index=['n', 'm', 'graph_file'], 
            columns='algorithm', 
            values='memory_mb'
        ).reset_index()

        # Rename columns to match your desired output
        df_summary.rename(columns={
            'graph_file': 'Graph File',
            f'{algname1}': f'Mem {algname1} (MB)',
            f'{algname2}': f'Mem {algname2} (MB)'
        }, inplace=True)

        if f'Mem {algname2} (MB)' in df_summary and f'Mem {algname1} (MB)' in df_summary:
            df_summary[f'Ratio {algname2} / {algname1}'] = df_summary[f'Mem {algname2} (MB)'] / df_summary[f'Mem {algname1} (MB)']

        # Save outputs
        save_performance_table(df_summary, OUTPUT_TABLE_FILENAME, OUTPUT_DIR)
        df_para_tabela_latex(df_summary, os.path.join(OUTPUT_DIR, "table_memory_latex.tex"))