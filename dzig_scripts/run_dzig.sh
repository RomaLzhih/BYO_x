#!/bin/bash

graphs=("com-orkut_sym.bin" "soc-LiveJournal1_sym.bin" "twitter_sym.bin")
# graphs=("com-orkut_sym.bin" "soc-LiveJournal1_sym.bin" "twitter_sym.bin" "friendster_sym.bin")
# graphs=("com-orkut_sym.bin")
solvers=("run_ppcsr" "run_std_set")
# solvers=("run_ppcsr")
batch_size_seq=(1 10 100 1000 10000 100000 1000000)
# batch_size_seq=(1 10)
batch_num=10
rounds=4
algorithm=("bfs" "pagerank" "labelpropagation")
log="run_dzig_local.log"
# Ziyang's local path
graph_path_prefix="/anvil/scratch/x-zmen/graphs/bin/"
store_prefix="/anvil/scratch/x-zmen/graphs/byo/"
# Perlmutter's path
# graph_path_prefix="/pscratch/sd/r/raqib/dataset-byo/bin/"
# store_prefix="/pscratch/sd/r/raqib/dataset-byo/"
#echo "graph_path_prefix: ${graph_path_prefix}"

threads=128
: >${log}
for s in "${solvers[@]}"; do
    bazel build //benchmarks/run_structures:"${s}"
    if [ $? -ne 0 ]; then
        echo "Error: bazel build failed. Exiting script."
        exit 1
    fi

    for batch_size in "${batch_size_seq[@]}"; do
        for g in "${graphs[@]}"; do
            path=${graph_path_prefix}${g}
            batch_file="${store_prefix}${g%.bin}_batches/batch_${batch_size}.in"

            for a in "${algorithm[@]}"; do
                echo "--- scripts: Running ${s} on ${g} with batch size ${batch_size} and algorithm ${a}" | tee -a ${log}
                # Record the start time
                start_time=$(date +%s)

                PARLAY_NUM_THREADS=${threads} ./../bazel-bin/benchmarks/run_structures/${s} -alg ${a} -batch_num ${batch_num} -batch_size ${batch_size} -batch_file ${batch_file} -s -b -i 1 -rounds ${rounds} -src 10 ${path} 2>&1 | tee -a ${log}

                # Record the end time
                end_time=$(date +%s)

                # Calculate the elapsed time
                elapsed_time=$((end_time - start_time))
                echo "--- scripts: program finish, took ${elapsed_time}" | tee -a ${log}

            done
        done
    done
    # echo "-----------------------------------" | tee -a ${log}
done
