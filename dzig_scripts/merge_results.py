#!/usr/bin/env python3
"""
Merge all CSV files in data/ and pivot algorithms into columns.
Output columns: baseline, graph, batch_size,
  then insert_time and algorithm_time for each algorithm in order:
  PageRank, LabelPropagation, wBFS_unweighted, BFS
"""

import os
import glob
import pandas as pd

ALGORITHM_ORDER = ["PageRank", "LabelPropagation", "wBFS_unweighted", "BFS"]
GRAPH_ORDER = ["com-orkut.ungraph", "cit-Patents", "soc-LiveJournal1", "twitter-unique-undir", "com-friendster"]

data_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
csv_files = [f for f in glob.glob(os.path.join(data_dir, "*.csv"))
             if os.path.basename(f) != "merged_results.csv"]

if not csv_files:
    print(f"No CSV files found in {data_dir}")
    exit(1)

df = pd.concat([pd.read_csv(f) for f in csv_files], ignore_index=True)

# Pivot to wide format keeping each measurement_idx
pivot = df.pivot(
    index=["baseline", "graph", "batch_size", "measurement_idx"],
    columns="algorithm",
    values=["insert_time", "algorithm_time"],
)

# Flatten column names: insert_time_PageRank, algorithm_time_PageRank, ...
pivot.columns = [f"{metric}_{alg}" for metric, alg in pivot.columns]
pivot = pivot.reset_index()

# Build ordered column list
ordered_cols = ["baseline", "graph", "batch_size", "measurement_idx"]
for alg in ALGORITHM_ORDER:
    for metric in ["insert_time", "algorithm_time"]:
        col = f"{metric}_{alg}"
        if col in pivot.columns:
            ordered_cols.append(col)

pivot = pivot[ordered_cols]

# Sort by graph order, then baseline, then batch_size
graph_order = {g: i for i, g in enumerate(GRAPH_ORDER)}
pivot["_graph_order"] = pivot["graph"].map(graph_order)
pivot = pivot.sort_values(["baseline", "batch_size", "_graph_order", "measurement_idx"]).drop(columns="_graph_order").reset_index(drop=True)

output_file = os.path.join(data_dir, "merged_results.csv")
pivot.to_csv(output_file, index=False)
print(f"Saved to {output_file}")
print(pivot.to_string(index=False))
