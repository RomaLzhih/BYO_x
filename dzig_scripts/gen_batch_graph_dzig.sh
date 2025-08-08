#!/bin/bash

# Ziyang's local path
graph_path_prefix="/anvil/scratch/x-zmen/graphs/bin"
store_prefix="/anvil/scratch/x-zmen/graphs/byo"

# Perlmutter's path
# graph_path_prefix="/pscratch/sd/r/raqib/dataset-byo/bin"
# store_prefix="/pscratch/sd/r/raqib/dataset-byo"

graphs=("com-orkut_sym.bin" "soc-LiveJournal1_sym.bin" "twitter_sym.bin" "friendster_sym.bin")
# graphs=("com-orkut_sym.bin")
batch_size_seq=(1 10 100 1000 10000 100000 1000000)

bazel build //benchmarks/run_structures:run_edges_batch_generator
if [ $? -ne 0 ]; then
    echo "Error: bazel build failed. Exiting script."
    exit 1
fi

for g in "${graphs[@]}"; do
    store_path="${store_prefix}/${g%.bin}_batches"
    mkdir -p "${store_path}"
    rm -rf "${store_path:?}"/*
    echo "Generating batches for graph: ${g} in ${store_path}"
    for batch_size in "${batch_size_seq[@]}"; do
        batch_file="${store_path}/batch_${batch_size}.in"
        echo "Generating batch file: ${batch_file}"
        # needs to be the sym graph
        # if it is not bin format, please remove the -b tag
        ./../bazel-bin/benchmarks/run_structures/run_edges_batch_generator -o "${batch_file}" -batch_size "${batch_size}" -batch_num 10 -s -b "${graph_path_prefix}/${g}"
    done
done
