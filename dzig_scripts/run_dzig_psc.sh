#!/bin/bash

prefix="/ocean/projects/cis250082/"
graphs=("tw.adj" "fs.adj")
solvers=("run_std_set" "run_vector_vector" "run_ppcsr")
log="run_dzig.log"

: >${log}
for s in "${solvers[@]}"; do
  echo "Running ${s}" | tee -a ${log}
  for g in "${graphs[@]}"; do
    echo "${g}" | tee -a ${log}
    path=prefix${g}
    ./../bazel-bin/benchmarks/run_structures/${s} -s -src 10 ${path} 2>&1 | tee -a ${log}
    echo "*****" | tee -a ${log}
  done
  echo "-----------------------------------" | tee -a ${log}
done
