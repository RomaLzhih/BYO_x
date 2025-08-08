To compile a single solver `ppcsr`:
```{bash}
bazel build benchmarks/run_structures:run_ppcsr
```

To run a single solver `ppcsr` with batch input file `soc-Livejournal1_sym_batches/batch_1.in` and running algorithm `pagerank`:
```{bash}
bazel build benchmarks/run_structures:run_ppcsr && ./bazel-bin/benchmarks/run_structures/run_ppcsr -batch_file /anvil/scratch/x-zmen/graphs/byo/soc-LiveJournal1_sym_batches/batch_1.in -batch_size 1 -batch_num 10 -alg pagerank -s -b -i 1 -src 10 /anvil/scratch/x-zmen/graphs/bin/soc-LiveJournal1_sym.bin
```
To generate `10` batch files for `soc-LiveJournal1_sym.bin`, each with batch size `1000000`:
```{bash}
bazel build benchmarks/run_structures:run_edges_batch_generator

./bazel-bin/benchmarks/run_structures/run_edges_batch_generator -o /data/zmen002/graph/byo/livejournal_batches/batch_1000000.in -batch_size 1000000 -batch_num 10 -s -b /data/graphs/bin/soc-LiveJournal1_sym.bin 
```

To run the scripts:
```{bash}
cd dzig_scripts
./run_dzig.sh
```

To summarize the results:
```{bash}
cd dzig_scripts
./summarize.sh run_dzig_local.log > summarized.log
```
