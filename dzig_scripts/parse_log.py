#!/usr/bin/env python3
"""
Parse run_dzig_local.log and group results by baseline, batch size, and graphs.
"""

import re
from collections import defaultdict
import json


def parse_log_file(log_file):
    """
    Parse the log file and extract results grouped by baseline, batch size, and graph.
    
    Returns a nested dictionary structure:
    {
        'baseline_name': {
            'batch_size': {
                'graph_name': {
                    'algorithm': [list of result pairs]
                }
            }
        }
    }
    """
    results = defaultdict(lambda: defaultdict(lambda: defaultdict(dict)))
    
    with open(log_file, 'r') as f:
        lines = f.readlines()
    
    i = 0
    current_baseline = None
    
    while i < len(lines):
        line = lines[i].strip()
        
        # Look for baseline name in "--- scripts:  Running run_xxx on..."
        if line.startswith("--- scripts:") and "Running" in line:
            # Extract baseline name (e.g., run_absl_btree_set)
            match = re.search(r'Running (\S+) on', line)
            if match:
                current_baseline = match.group(1)
        
        # Look for ">>> Begin" marker
        if line.startswith(">>> Begin"):
            input_file = None
            batch_size = None

            # Collect metadata until the first >>> Alg or >>> End
            j = i + 1
            while j < len(lines):
                metadata_line = lines[j].strip()
                if metadata_line.startswith(">>> Alg") or metadata_line.startswith(">>> End"):
                    break
                if metadata_line.startswith(">>> Input_file"):
                    input_file = metadata_line.split()[-1]
                elif metadata_line.startswith(">>> Batch_size"):
                    batch_size = metadata_line.split()[-1]
                j += 1

            # Extract graph name from input_file
            graph_name = None
            if input_file:
                graph_file = input_file.split('/')[-1]
                graph_name_match = re.match(r'(.+?)\.base\.\d+\.adj', graph_file)
                graph_name = graph_name_match.group(1) if graph_name_match else graph_file

            # Now iterate through all >>> Alg / >>> Res: pairs until >>> End
            while j < len(lines) and not lines[j].strip().startswith(">>> End"):
                alg_line = lines[j].strip()
                if alg_line.startswith(">>> Alg"):
                    algorithm = alg_line.split(">>> Alg")[1].strip()
                    j += 1

                    # Expect >>> Res: next
                    if j < len(lines) and lines[j].strip().startswith(">>> Res:"):
                        j += 1
                        result_pairs = []

                        while j < len(lines):
                            result_line = lines[j].strip()
                            if result_line.startswith(">>> Alg") or result_line.startswith(">>> End"):
                                break
                            if result_line:
                                parts = result_line.split()
                                if len(parts) == 2:
                                    try:
                                        result_pairs.append([float(parts[0]), float(parts[1])])
                                    except ValueError:
                                        pass
                            j += 1

                        if current_baseline and graph_name and batch_size and algorithm:
                            results[current_baseline][batch_size][graph_name][algorithm] = result_pairs
                else:
                    j += 1

            i = j
        else:
            i += 1
    
    return results


def print_results_summary(results):
    """Print a summary of the parsed results."""
    print("=" * 80)
    print("PARSED RESULTS SUMMARY")
    print("=" * 80)
    
    for baseline in sorted(results.keys()):
        print(f"\n{baseline.upper()}")
        print("-" * 80)
        
        for batch_size in sorted(results[baseline].keys(), key=int):
            print(f"\n  Batch Size: {batch_size}")
            
            for graph in sorted(results[baseline][batch_size].keys()):
                print(f"\n    Graph: {graph}")
                
                for algorithm in sorted(results[baseline][batch_size][graph].keys()):
                    data = results[baseline][batch_size][graph][algorithm]
                    print(f"      {algorithm}: {len(data)} measurements")
                    
                    if data:
                        # Calculate some basic statistics
                        col1_values = [pair[0] for pair in data]
                        col2_values = [pair[1] for pair in data]
                        
                        col1_avg = sum(col1_values) / len(col1_values)
                        col2_avg = sum(col2_values) / len(col2_values)
                        
                        print(f"        Column 1 - avg: {col1_avg:.6f}, min: {min(col1_values):.6f}, max: {max(col1_values):.6f}")
                        print(f"        Column 2 - avg: {col2_avg:.6f}, min: {min(col2_values):.6f}, max: {max(col2_values):.6f}")


def save_results_json(results, output_file):
    """Save results to a JSON file."""
    # Convert defaultdict to regular dict for JSON serialization
    regular_dict = {}
    for baseline, batch_data in results.items():
        regular_dict[baseline] = {}
        for batch_size, graph_data in batch_data.items():
            regular_dict[baseline][batch_size] = {}
            for graph, alg_data in graph_data.items():
                regular_dict[baseline][batch_size][graph] = dict(alg_data)
    
    with open(output_file, 'w') as f:
        json.dump(regular_dict, f, indent=2)
    
    print(f"\nResults saved to: {output_file}")


def save_results_csv(results, output_file):
    """Save results to a CSV file."""
    with open(output_file, 'w') as f:
        # Write header
        f.write("baseline,batch_size,graph,algorithm,measurement_idx,insert_time,algorithm_time\n")
        
        # Write data
        for baseline in sorted(results.keys()):
            for batch_size in sorted(results[baseline].keys(), key=int):
                for graph in sorted(results[baseline][batch_size].keys()):
                    for algorithm in sorted(results[baseline][batch_size][graph].keys()):
                        data = results[baseline][batch_size][graph][algorithm]
                        for idx, (val1, val2) in enumerate(data):
                            f.write(f"{baseline},{batch_size},{graph},{algorithm},{idx},{val1},{val2}\n")
    
    print(f"Results saved to: {output_file}")


if __name__ == "__main__":
    import sys
    import os

    if len(sys.argv) < 2:
        print("Usage: parse_log.py <log_file>")
        sys.exit(1)

    log_file = sys.argv[1]

    if not os.path.exists(log_file):
        print(f"Error: file not found: {log_file}")
        sys.exit(1)

    print(f"Parsing {log_file}...")
    results = parse_log_file(log_file)
    
    # Print summary
    print_results_summary(results)
    
    base_name = os.path.splitext(os.path.basename(log_file))[0]
    output_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
    os.makedirs(output_dir, exist_ok=True)

    # Save to CSV
    save_results_csv(results, os.path.join(output_dir, f"{base_name}.csv"))
    
    print("\n" + "=" * 80)
    print("Parsing complete!")
    print("=" * 80)
