#!/bin/bash

solvers=("run_ppcsr" "run_std_set")
graphs=("soc-LiveJournal1")
batch_size_seq=(1 10 100 1000 10000 100000 1000000)
batch_num=10
rounds=4
algorithm=("bfs" "pagerank" "labelpropagation" "wbfs")
log="run_dzig_local.log"
graph_path_prefix="/anvil/projects/x-cis250123/"

# Map from graph names to their prefixes (directory names)
declare -A graph_prefix_map
graph_prefix_map["soc-LiveJournal1"]="live-journal"
# Add more mappings as needed:
# graph_prefix_map["another-graph"]="another-dir"

threads=128
: >${log}
for s in "${solvers[@]}"; do
    bazel build //benchmarks/run_structures:"${s}"
    if [ $? -ne 0 ]; then
        echo "Error: bazel build failed.  Exiting script."
        exit 1
    fi

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

                ./../bazel-bin/benchmarks/run_structures/${s} -alg ${a} -batch_num ${batch_num} -batch_size ${batch_size} -batch_file ${batch_file} -s -i 1 -rounds ${rounds} -src 10 ${path} 2>&1 | tee -a ${log}

                # Record the end time
                end_time=$(date +%s)

                # Calculate the elapsed time
                elapsed_time=$((end_time - start_time))
                echo "--- scripts:  program finish, took ${elapsed_time}" | tee -a ${log}

            done
        done
    done
    # echo "-----------------------------------" | tee -a ${log}
done
