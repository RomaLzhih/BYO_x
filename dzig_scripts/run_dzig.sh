#!/bin/bash

# graphs=("com-orkut_sym.bin" "soc-LiveJournal1_sym.bin" "twitter_sym.bin" "friendster_sym.bin")
graphs=("com-orkut_sym.bin")
# solvers=("run_std_set" "run_vector_vector" "run_ppcsr")
solvers=("run_std_set" "run_ppcsr")
batch_size_seq=(1 10 100 1000 10000 100000 1000000)
batch_num=10
algorithm=("bfs" "pagerank")
log="run_dzig_local.log"
graph_path_prefix="/data/graphs/bin/"

: >${log}
for s in "${solvers[@]}"; do
  bazel build //benchmarks/run_structures:"${s}"
  if [ $? -ne 0 ]; then
    echo "Error: bazel build failed. Exiting script."
    exit 1
  fi

  for g in "${graphs[@]}"; do
    path=${graph_path_prefix}${g}

    for batch_size in "${batch_size_seq[@]}"; do
      # get the batch file path
      batch_file="${g%.bin}_batches/batch_${batch_size}.in"

      for a in "${algorithm[@]}"; do
        echo ">>>Running ${s} on ${g} with batch size ${batch_size} and algorithm ${a}" | tee -a ${log}

        ./../bazel-bin/benchmarks/run_structures/${s} -alg ${a} -batch_num ${batch_num} -batch_size ${batch_size} -batch_file ${batch_file} -s -b -i 1 -src 10 ${path} 2>&1 | tee -a ${log}

        echo ">>>Program Finish." | tee -a ${log}

      done
    done
  done
  echo "-----------------------------------" | tee -a ${log}
done
