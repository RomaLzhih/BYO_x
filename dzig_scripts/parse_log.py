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
            # Extract metadata from the following lines
            input_file = None
            batch_file = None
            batch_num = None
            batch_size = None
            algorithm = None
            
            # Parse metadata lines
            j = i + 1
            while j < len(lines) and not lines[j].strip().startswith(">>> Res:"):
                metadata_line = lines[j].strip()
                
                if metadata_line.startswith(">>> Input_file"):
                    input_file = metadata_line.split()[-1]
                elif metadata_line.startswith(">>> Batch_file"):
                    batch_file = metadata_line.split()[-1]
                elif metadata_line.startswith(">>> Batch_num"):
                    batch_num = metadata_line.split()[-1]
                elif metadata_line.startswith(">>> Batch_size"):
                    batch_size = metadata_line.split()[-1]
                elif metadata_line.startswith(">>> Alg"):
                    # Algorithm name is everything after ">>> Alg"
                    algorithm = metadata_line.split(">>> Alg")[1].strip()
                
                j += 1
            
            # Now parse results
            if j < len(lines) and lines[j].strip().startswith(">>> Res:"):
                j += 1  # Move past ">>> Res:" line
                result_pairs = []
                
                while j < len(lines) and not lines[j].strip().startswith(">>> End"):
                    result_line = lines[j].strip()
                    if result_line:
                        # Parse the two numbers
                        parts = result_line.split()
                        if len(parts) == 2:
                            try:
                                num1 = float(parts[0])
                                num2 = float(parts[1])
                                result_pairs.append([num1, num2])
                            except ValueError:
                                pass
                    j += 1
                
                # Extract graph name from input_file
                if input_file and current_baseline:
                    # Example: /data/datasets/graphs/live-journal/soc-LiveJournal1.base.1.adj
                    # Graph is the file name prefix (soc-LiveJournal1)
                    path_parts = input_file.split('/')
                    graph_file = path_parts[-1]
                    # Extract graph name (before .base)
                    graph_name_match = re.match(r'(.+?)\.base\.\d+\.adj', graph_file)
                    if graph_name_match:
                        graph_name = graph_name_match.group(1)
                    else:
                        graph_name = graph_file
                    
                    # Store results
                    if batch_size and algorithm:
                        results[current_baseline][batch_size][graph_name][algorithm] = result_pairs
                
                i = j
            else:
                i += 1
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
        f.write("baseline,batch_size,graph,algorithm,measurement_idx,value1,value2\n")
        
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
    log_file = "run_dzig_local.log"
    
    print(f"Parsing {log_file}...")
    results = parse_log_file(log_file)
    
    # Print summary
    print_results_summary(results)
    
    # Save to JSON
    save_results_json(results, "parsed_results.json")
    
    # Save to CSV
    save_results_csv(results, "parsed_results.csv")
    
    print("\n" + "=" * 80)
    print("Parsing complete!")
    print("=" * 80)
