#!/bin/bash

prefix="/data/graphs/bin/"
graphs=("com-orkut_sym.bin" "soc-LiveJournal1_sym.bin" "twitter_sym.bin" "friendster_sym.bin")
# solvers=("run_std_set" "run_vector_vector" "run_ppcsr")
solvers=("run_std_set")
log="run_dzig_local.log"

: >${log}
for s in "${solvers[@]}"; do
  bazel build //benchmarks/run_structures:${s}
  echo "Running ${s}" | tee -a ${log}
  for g in "${graphs[@]}"; do
    echo "${g}" | tee -a ${log}
    path=${prefix}${g}
    ./../bazel-bin/benchmarks/run_structures/${s} -s -b -i 1 -src 10 ${path} 2>&1 | tee -a ${log}
    echo "*****" | tee -a ${log}
  done
  echo "-----------------------------------" | tee -a ${log}
done
