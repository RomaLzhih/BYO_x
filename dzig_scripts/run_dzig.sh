#!/bin/bash

#SBATCH --nodes=1               # Total # of nodes (must be 1 for OpenMP job)
#SBATCH --ntasks-per-node=1     # Total # of MPI tasks per node
#SBATCH --cpus-per-task=128      # cpu-cores per task (default value is 1, >1 for multi-threaded tasks)
#SBATCH --time=8:00:00          # Total run time limit (hh:mm:ss)
#SBATCH -J dzig            # Job name
#SBATCH -o dzig.o%j            # Name of stdout output file
#SBATCH -e dzig.e%j            # Name of stderr error file
#SBATCH -p wholenode            # Queue (partition) name

solvers=("run_ppcsr" "run_std_set" "run_pcsr" "run_absl_btree_set" "run_absl_flat_hash_set" "cpam" "run_sstgraph" "run_dhb" "run_terrace")
graphs=("soc-LiveJournal1")
# batch_size_seq=(1 10 100 1000 10000 100000 1000000)
batch_size_seq=(1)
batch_num=10
rounds=4
algorithm=("bfs" "pagerank" "labelpropagation" "wbfs")
log="run_dzig_local.log"
graph_path_prefix="/data/datasets/graphs/"

# Map from graph names to their prefixes (directory names)
declare -A graph_prefix_map
graph_prefix_map["soc-LiveJournal1"]="live-journal"
# Add more mappings as needed:
# graph_prefix_map["another-graph"]="another-dir"
scripts_path="$PWD"
project_path=$(dirname "$scripts_path")

: >${log}
for s in "${solvers[@]}"; do
    bazel build //benchmarks/run_structures:"${s}"

    for batch_size in "${batch_size_seq[@]}"; do
        for g in "${graphs[@]}"; do
            # Get the directory prefix for the current graph
            prefix=${graph_prefix_map[$g]}

            # Construct path:  /anvil/projects/x-cis250123/live-journal/soc-LiveJournal1.base.1.adj
            path="${graph_path_prefix}${prefix}/${g}.base.${batch_size}.adj"

            # Construct batch_file: /anvil/projects/x-cis250123/live-journal/soc-LiveJournal1.dynamic.{batch_size}.el
            batch_file="${graph_path_prefix}${prefix}/${g}.dynamic.${batch_size}.el"

            for a in "${algorithm[@]}"; do
                echo "--- scripts:  Running ${s} on ${g} (prefix: ${prefix}) with batch size ${batch_size} and algorithm ${a}" | tee -a ${log}
                # Record the start time
                start_time=$(date +%s)

                ${project_path}/bazel-bin/benchmarks/run_structures/${s} -alg ${a} -batch_num ${batch_num} -batch_size ${batch_size} -batch_file ${batch_file} -s -i 1 -rounds ${rounds} -src 10 ${path} 2>&1 | tee -a ${log}

                # Record the end time
                end_time=$(date +%s)

                # Calculate the elapsed time
                elapsed_time=$((end_time - start_time))
                echo "--- scripts:  program finish, took ${elapsed_time}" | tee -a ${log}

            done
        done
    done
    echo "-----------------------------------" | tee -a ${log}
done
