#!/bin/bash

# graphs=("com-orkut_sym.bin" "soc-LiveJournal1_sym.bin" "twitter_sym.bin" "friendster_sym.bin")
graphs=("com-orkut_sym.bin")
# solvers=("run_std_set" "run_vector_vector" "run_ppcsr")
solvers=("run_ppcsr")
#batch_size_seq=(1 10 100 1000 10000 100000 1000000)
batch_size_seq=(1 10)
batch_num=10
algorithm=("bfs" "pagerank")
log="run_dzig_local.log"
# Ziyang's local path
graph_path_prefix="/data/graphs/bin/"
store_prefix="/data/zmen002/graph/byo/"
# Perlmutter's path
# graph_path_prefix="/pscratch/sd/r/raqib/dataset-byo/bin/"
# store_prefix="/pscratch/sd/r/raqib/dataset-byo/"
#echo "graph_path_prefix: ${graph_path_prefix}"

threads=192
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

        PARLAY_NUM_THREADS=$threads ./../bazel-bin/benchmarks/run_structures/${s} -alg ${a} -batch_num ${batch_num} -batch_size ${batch_size} -batch_file ${batch_file} -s -b -i 1 -src 10 ${path} 2>&1 | tee -a ${log}

        # echo ">>>Program Finish." | tee -a ${log}

      done
    done
  done
  # echo "-----------------------------------" | tee -a ${log}
done
