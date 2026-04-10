#pragma once

#include <cassert>
#include <cstdlib>
#include <map>

#include "ApproximateDensestSubgraph/ApproxPeelingBKV12/DensestSubgraph.h"
#include "ApproximateDensestSubgraph/GreedyCharikar/DensestSubgraph.h"
#include "BFS/NonDeterministicBFS/BFS.h"
#include "CoSimRank/CoSimRank.h"
#include "Connectivity/BFSCC/Connectivity.h"
#include "Connectivity/LabelPropagation/Connectivity.h"
#include "Connectivity/SimpleUnionAsync/Connectivity.h"
#include "Connectivity/WorkEfficientSDB14/Connectivity.h"
#include "IntegralWeightSSSP/JulienneDBS17/wBFS_unweighted.h"
// #include "DegeneracyOrder/GoodrichPszona11/DegeneracyOrder.h"
#include "GraphColoring/Hasenplaugh14/GraphColoring.h"
#include "KCore/JulienneDBS17/KCore.h"
#include "LowDiameterDecomposition/MPX13/LowDiameterDecomposition.h"
#include "Map/Base/Map.h"
#include "Map/RemoteRead/Map.h"
#include "MaximalIndependentSet/RandomGreedy/MaximalIndependentSet.h"
#include "PageRank/PageRank.h"
#include "SSBetweenessCentrality/Brandes/SSBetweennessCentrality.h"
#include "Spanner/MPXV15/Spanner.h"
#include "TriangleCounting/ShunTangwongsan15/Triangle.h"

namespace gbbs {

template <class Graph>
double Map_runner(Graph const& G, size_t rounds) {
  std::cout << "### Application: Base Map" << std::endl;
  std::cout << "### ------------------------------------" << std::endl;
  std::cout << "### ------------------------------------" << std::endl;

  double total_time = 0.0;
  for (size_t r = 0; r <= rounds; r++) {
    timer t;
    t.start();
    auto sum = Test_Map(G);
    double tt = t.stop();
    std::cout << "### Running Time: " << tt << std::endl;
    if (r == 0) {
      std::cout << "Edgesum was " << sum << "\n";
    } else {
      total_time += tt;
    }
  }
  auto time_per_iter = total_time / rounds;
  std::cout << "# time per iter: " << time_per_iter << "\n";
  return time_per_iter;
}

template <class Graph, size_t read_size>
double Map_Remote_runner(Graph const& G, size_t rounds) {
  std::cout << "### Application: Map with Remote: " << read_size << std::endl;
  std::cout << "### ------------------------------------" << std::endl;
  std::cout << "### ------------------------------------" << std::endl;

  double total_time = 0.0;
  double tt = 0;
  parlay::random rand = random();
  size_t n = G.N();
  for (size_t r = 0; r <= rounds; r++) {
    if constexpr (read_size == 0) {
      std::vector<bool> remote_data(n);
      remote_data[rand.rand() % n] = true;
      timer t;
      t.start();
      auto sum = Test_Map_Remote(G, remote_data);
      tt = t.stop();
      std::cout << "Edgesum was " << sum << "\n";
    }
    if constexpr (read_size == 1) {
      std::vector<char> remote_data(n);
      remote_data[rand.rand() % n] = true;
      timer t;
      t.start();
      auto sum = Test_Map_Remote(G, remote_data);
      tt = t.stop();
      std::cout << "Edgesum was " << sum << "\n";
    }
    if constexpr (read_size == 2) {
      std::vector<uint16_t> remote_data(n);
      remote_data[rand.rand() % n] = true;
      timer t;
      t.start();
      auto sum = Test_Map_Remote(G, remote_data);
      tt = t.stop();
      std::cout << "Edgesum was " << sum << "\n";
    }
    if constexpr (read_size == 4) {
      std::vector<uint32_t> remote_data(n);
      remote_data[rand.rand() % n] = true;
      timer t;
      t.start();
      auto sum = Test_Map_Remote(G, remote_data);
      tt = t.stop();
      std::cout << "Edgesum was " << sum << "\n";
    }
    if constexpr (read_size == 8) {
      std::vector<uint64_t> remote_data(n);
      remote_data[rand.rand() % n] = true;
      timer t;
      t.start();
      auto sum = Test_Map_Remote(G, remote_data);
      tt = t.stop();
      std::cout << "Edgesum was " << sum << "\n";
    }
    if constexpr (read_size > 8) {
      std::vector<wide_int<size_t, read_size>> remote_data(n);
      remote_data[rand.rand() % n] = true;
      timer t;
      t.start();
      auto sum = Test_Map_Remote(G, remote_data);
      tt = t.stop();
      std::cout << "Edgesum was " << sum << "\n";
    }
    std::cout << "### Running Time: " << tt << std::endl;
    if (r == 0) {
    } else {
      total_time += tt;
    }
  }
  auto time_per_iter = total_time / rounds;
  std::cout << "# time per iter: " << time_per_iter << "\n";
  return time_per_iter;
}

template <class Graph>
double BFS_runner(Graph const& G, uintE src, size_t rounds, bool dump) {
  // std::cout << "### Application: BFS" << std::endl;
  // std::cout << "### Params: -src = " << src << std::endl;
  // std::cout << "### ------------------------------------" << std::endl;
  // std::cout << "### ------------------------------------" << std::endl;

  double total_time = 0.0;
  timer t;
  t.start();
  auto parents = BFS(G, src);
  double tt = t.stop();
  // std::cout << "### Running Time: " << tt << std::endl;
  if (dump) {
    // useful for debugging
    std::vector<uintE> depths(G.N(), UINT_E_MAX);
    for (size_t j = 0; j < G.N(); j++) {
      uintE current_depth = 0;
      uintE current_parent = j;
      if (parents[j] == UINT_E_MAX) {
        continue;
      }
      while (current_parent != parents[current_parent]) {
        current_depth += 1;
        current_parent = parents[current_parent];
      }
      depths[j] = current_depth;
    }
    std::ofstream myfile;
    myfile.open("bfs.out");
    for (unsigned int i = 0; i < G.N(); i++) {
      myfile << depths[i] << std::endl;
    }
    myfile.close();
  }
  // std::cout << "# time per iter: " << time_per_iter << "\n";
  return tt;
}

template <class Graph>
double WBFS_unweighted_runner(Graph const& G, uintE src, size_t rounds,
                              size_t num_buckets, bool no_blocked,
                              bool largemem, bool dump) {
  // std::cout << "### Application: wBFS (Weighted Breadth-First Search)"
  //           << std::endl;
  // std::cout << "### Threads: " << num_workers() << std::endl;
  // std::cout << "### n: " << G.N() << std::endl;
  // std::cout << "### m: " << G.M() << std::endl;
  // std::cout << "### Params: -src = " << src
  //           << " -nb (num_buckets) = " << num_buckets << std::endl;
  // std::cout << "### ------------------------------------" << std::endl;

  if (num_buckets != (((uintE)1) << parlay::log2_up(num_buckets))) {
    std::cout << "Please specify a number of buckets that is a power of two"
              << "\n";
    exit(-1);
  }

  double total_time = 0.0;
  timer t;
  t.start();
  auto dists = wBFS_unweighted(G, src, num_buckets, largemem, no_blocked);
  double tt = t.stop();
  // std::cout << "### Running Time: " << tt << std::endl;
  if (dump) {
    // useful for debugging
    std::ofstream myfile;
    myfile.open("wBFS_unweighted.out");
    for (unsigned int i = 0; i < G.N(); i++) {
      myfile << dists[i] << std::endl;
    }
    myfile.close();
  }
  // std::cout << "# time per iter: " << time_per_iter << "\n";
  return tt;
}

template <class Graph>
double WorkEfficientDensestSubgraph_runner(Graph const& G, double eps,
                                           size_t rounds) {
  std::cout << "### Application: WorkEfficientDensestSubgraph" << std::endl;
  std::cout << "### Params: -eps = " << eps << std::endl;
  std::cout << "### ------------------------------------" << std::endl;

  double total_time = 0.0;
  for (size_t r = 0; r <= rounds; r++) {
    timer t;
    t.start();
    WorkEfficientDensestSubgraph(G, eps);
    double tt = t.stop();
    if (r > 0) {
      total_time += tt;
    }
  }

  auto time_per_iter = total_time / rounds;
  std::cout << "# time per iter: " << time_per_iter << "\n";
  return time_per_iter;
}

template <class Graph>
double CharikarAppxDensestSubgraph_runner(Graph const& G, size_t rounds) {
  std::cout << "### Application: CharikarAppxDensestSubgraph" << std::endl;
  std::cout << "### ------------------------------------" << std::endl;

  double total_time = 0.0;
  for (size_t r = 0; r <= rounds; r++) {
    timer t;
    t.start();
    CharikarAppxDensestSubgraph(G);
    double tt = t.stop();
    if (r > 0) {
      total_time += tt;
    }
  }

  auto time_per_iter = total_time / rounds;
  std::cout << "# time per iter: " << time_per_iter << "\n";
  return time_per_iter;
}

template <class Graph>
double CoSimRank_runner(Graph const& G, double eps, size_t iters, double c,
                        double u, double v, bool em, size_t rounds) {
  std::cout << "### Application: CoSimRank" << std::endl;
  std::cout << "### Params: -eps = " << eps << std::endl;
  std::cout << "### ------------------------------------" << std::endl;

  timer t;

  double total_time = 0.0;
  for (size_t r = 0; r <= rounds; r++) {
    t.start();
    if (em)
      CoSimRank_edgeMap(G, u, v, eps, c, iters);
    else
      CoSimRank(G, u, v, eps, c, iters);
    double tt = t.stop();

    std::cout << "### Running Time: " << tt << std::endl;
    if (r > 0) {
      total_time += tt;
    }
  }

  auto time_per_iter = total_time / rounds;
  std::cout << "# time per iter: " << time_per_iter << "\n";
  return time_per_iter;
}

template <class Graph>
double BFSCC_runner(Graph const& G, size_t rounds, bool dump) {
  std::cout << "### Application: BFSCC (Connectivity)" << std::endl;
  std::cout << "### ------------------------------------" << std::endl;

  double total_time = 0.0;
  for (size_t r = 0; r <= rounds; r++) {
    timer t;
    t.start();
    auto components = bfs_cc::CC(G);
    double tt = t.stop();
    std::cout << "### Running Time: " << tt << std::endl;
    if (r == 0) {
      if (dump) {
        std::ofstream myfile;
        myfile.open("bfscc.out");
        for (size_t i = 0; i < G.N(); i++) {
          myfile << components[i] << std::endl;
        }
        myfile.close();
      }
    } else {
      total_time += tt;
    }
  }
  auto time_per_iter = total_time / rounds;
  std::cout << "# time per iter: " << time_per_iter << "\n";
  return time_per_iter;
}

template <class Graph>
double LabelPropCC_runner(Graph const& G, size_t rounds, bool dump,
                          bool permute) {
  // std::cout << "### Application: LabelPropCC (Connectivity)" << std::endl;
  // std::cout << "### ------------------------------------" << std::endl;

  double total_time = 0.0;
  timer t;
  t.start();
  auto components = (permute) ? labelprop_cc::CC</*use_permutation=*/true>(G)
                              : labelprop_cc::CC</*use_permutation=*/false>(G);
  double tt = t.stop();
  // std::cout << "### Running Time: " << tt << std::endl;
  if (dump) {
    std::ofstream myfile;
    myfile.open("label_propcc.out");
    for (size_t i = 0; i < G.N(); i++) {
      myfile << components[i] << std::endl;
    }
    myfile.close();
  }
  return tt;
}

template <class Graph>
double SimpleUnionCC_runner(Graph const& G, size_t rounds, bool dump) {
  std::cout << "### Application: SimpleUnionCC (Connectivity)" << std::endl;
  std::cout << "### ------------------------------------" << std::endl;

  double total_time = 0.0;
  for (size_t r = 0; r <= rounds; r++) {
    timer t;
    t.start();
    auto components = gbbs::simple_union_find::SimpleUnionAsync(G);
    double tt = t.stop();
    std::cout << "### Running Time: " << tt << std::endl;
    if (r == 0) {
      if (dump) {
        std::ofstream myfile;
        myfile.open("simple_unioncc.out");
        for (size_t i = 0; i < G.N(); i++) {
          myfile << components[i] << std::endl;
        }
        myfile.close();
      }
    } else {
      total_time += tt;
    }
  }
  auto time_per_iter = total_time / rounds;
  std::cout << "# time per iter: " << time_per_iter << "\n";
  return time_per_iter;
}

template <class Graph>
double WorkEfficientCC_runner(Graph const& G, size_t rounds, bool dump,
                              bool permute, double beta) {
  std::cout << "### Application: WorkEfficientCC (Connectivity)" << std::endl;
  std::cout << "### ------------------------------------" << std::endl;

  double total_time = 0.0;
  for (size_t r = 0; r <= rounds; r++) {
    timer t;
    t.start();
    auto components = workefficient_cc::CC(G, beta, false, permute);
    double tt = t.stop();
    std::cout << "### Running Time: " << tt << std::endl;
    if (r == 0) {
      if (dump) {
        std::ofstream myfile;
        myfile.open("WorkEfficientcc.out");
        for (size_t i = 0; i < G.N(); i++) {
          myfile << components[i] << std::endl;
        }
        myfile.close();
      }
    } else {
      total_time += tt;
    }
  }
  auto time_per_iter = total_time / rounds;
  std::cout << "# time per iter: " << time_per_iter << "\n";
  return time_per_iter;
}

// template <class Graph>
// double DegeneracyOrder_runner(const Graph &G, size_t rounds, bool dump,
//                               double eps) {
//   std::cout << "### Application: DegeneracyOrder" << std::endl;
//   std::cout << "### ------------------------------------" << std::endl;

//   double total_time = 0.0;
//   for (size_t r = 0; r <= rounds; r++) {
//     timer t;
//     t.start();
//     auto order = goodrichpszona_degen::DegeneracyOrder(G, eps);
//     double tt = t.stop();
//     std::cout << "### Running Time: " << tt << std::endl;
//     if (r == 0) {
//       if (dump) {
//         std::ofstream myfile;
//         myfile.open("DegeneracyOrder.out");
//         for (size_t i = 0; i < order.size(); i++) {
//           myfile << order[i] << std::endl;
//         }
//         myfile.close();
//       }
//     } else {
//       total_time += tt;
//     }
//   }
//   auto time_per_iter = total_time / rounds;
//   std::cout << "# time per iter: " << time_per_iter << "\n";
//   return time_per_iter;
// }

template <class Graph>
double Coloring_runner(Graph const& G, size_t rounds, bool dump, bool LF,
                       bool verify) {
  std::cout << "### Application: Coloring" << std::endl;
  std::cout << "### ------------------------------------" << std::endl;

  double total_time = 0.0;
  for (size_t r = 0; r <= rounds; r++) {
    timer t;
    t.start();
    auto colors = Coloring(G, LF);
    double tt = t.stop();
    std::cout << "### Running Time: " << tt << std::endl;
    if (r == 0) {
      if (verify) {
        verify_coloring(G, colors);
      }
      if (dump) {
        std::ofstream myfile;
        myfile.open("Coloring.out");
        for (size_t i = 0; i < colors.size(); i++) {
          myfile << colors[i] << std::endl;
        }
        myfile.close();
      }
    } else {
      total_time += tt;
    }
  }
  auto time_per_iter = total_time / rounds;
  std::cout << "# time per iter: " << time_per_iter << "\n";
  return time_per_iter;
}

template <class Graph>
double KCore_runner(Graph const& G, size_t rounds, bool dump, bool fa,
                    size_t num_buckets) {
  std::cout << "### Application: KCore" << std::endl;
  std::cout << "### ------------------------------------" << std::endl;

  double total_time = 0.0;
  for (size_t r = 0; r <= rounds; r++) {
    timer t;
    t.start();
    auto cores = (fa) ? KCore_FA(G, num_buckets) : KCore(G, num_buckets);
    double tt = t.stop();
    std::cout << "### Running Time: " << tt << std::endl;
    if (r == 0) {
      if (dump) {
        std::ofstream myfile;
        myfile.open("KCore.out");
        for (size_t i = 0; i < cores.size(); i++) {
          myfile << cores[i] << std::endl;
        }
        myfile.close();
      }
    } else {
      total_time += tt;
    }
  }
  auto time_per_iter = total_time / rounds;
  std::cout << "# time per iter: " << time_per_iter << "\n";
  return time_per_iter;
}

template <class Graph>
double LDD_runner(Graph const& G, size_t rounds, bool dump, double beta,
                  bool permute) {
  std::cout << "### Application: LDD" << std::endl;
  std::cout << "### ------------------------------------" << std::endl;

  double total_time = 0.0;
  for (size_t r = 0; r <= rounds; r++) {
    timer t;
    t.start();
    auto ldd = LDD(G, beta, permute);
    double tt = t.stop();
    std::cout << "### Running Time: " << tt << std::endl;
    if (r == 0) {
      if (dump) {
        std::ofstream myfile;
        myfile.open("ldd.out");
        for (size_t i = 0; i < ldd.size(); i++) {
          myfile << ldd[i] << std::endl;
        }
        myfile.close();
      }
    } else {
      total_time += tt;
    }
  }
  auto time_per_iter = total_time / rounds;
  std::cout << "# time per iter: " << time_per_iter << "\n";
  return time_per_iter;
}

template <class Graph>
double MIS_runner(Graph const& G, size_t rounds, bool dump, bool spec_for,
                  bool verify) {
  std::cout << "### Application: MIS" << std::endl;
  std::cout << "### ------------------------------------" << std::endl;

  double total_time = 0.0;
  // Code below looks duplicated; this is because the return types of specfor
  // and rootset are different
  if (spec_for) {
    for (size_t r = 0; r <= rounds; r++) {
      timer t;
      t.start();
      auto MaximalIndependentSet =
          MaximalIndependentSet_spec_for::MaximalIndependentSet(G);
      double tt = t.stop();
      std::cout << "### Running Time: " << tt << std::endl;
      if (r == 0) {
        if (dump) {
          std::ofstream myfile;
          myfile.open("mis.out");
          for (size_t i = 0; i < MaximalIndependentSet.size(); i++) {
            myfile << MaximalIndependentSet[i] << std::endl;
          }
          myfile.close();
        }
        if (verify) {
          auto size_f = [&](size_t i) {
            return (MaximalIndependentSet[i] == 1);
          };
          auto size_imap = parlay::delayed_seq<size_t>(G.N(), size_f);
          verify_MaximalIndependentSet(G, size_imap);
        }
      } else {
        total_time += tt;
      }
    }
  } else {
    for (size_t r = 0; r <= rounds; r++) {
      timer t;
      t.start();
      auto MaximalIndependentSet =
          MaximalIndependentSet_rootset::MaximalIndependentSet(G);
      double tt = t.stop();
      std::cout << "### Running Time: " << tt << std::endl;
      if (r == 0) {
        if (dump) {
          std::ofstream myfile;
          myfile.open("mis.out");
          for (size_t i = 0; i < MaximalIndependentSet.size(); i++) {
            myfile << MaximalIndependentSet[i] << std::endl;
          }
          myfile.close();
        }
        if (verify) {
          auto size_f = [&](size_t i) { return MaximalIndependentSet[i]; };
          auto size_imap = parlay::delayed_seq<size_t>(G.N(), size_f);
          verify_MaximalIndependentSet(G, size_imap);
        }
      } else {
        total_time += tt;
      }
    }
  }
  auto time_per_iter = total_time / rounds;
  std::cout << "# time per iter: " << time_per_iter << "\n";
  return time_per_iter;
}

template <class Graph>
double PageRank_runner(Graph const& G, size_t rounds, bool dump, size_t iters,
                       bool em, bool delta, double eps, double leps) {
  // std::cout << "### Application: PageRank" << std::endl;
  // std::cout << "### ------------------------------------" << std::endl;

  double total_time = 0.0;
  timer t;
  t.start();
  auto ret = (em)      ? PageRank_edgeMap(G, eps, iters)
             : (delta) ? delta::PageRankDelta(G, eps, leps, iters)
                       : PageRank(G, eps, iters);
  double tt = t.stop();
  if (dump) {
    std::ofstream myfile;
    myfile.open("pr.out");
    for (size_t i = 0; i < ret.size(); i++) {
      myfile << ret[i] << std::endl;
    }
    myfile.close();
  }
  return tt;
}

template <class Graph>
double SSBetweennessCentrality_runner(Graph const& G, size_t rounds, bool dump,
                                      uintE src, bool fa, bool ligra) {
  std::cout << "### Application: SSBetweennessCentrality" << std::endl;
  std::cout << "### ------------------------------------" << std::endl;

  double total_time = 0.0;
  for (size_t r = 0; r <= rounds; r++) {
    timer t;
    t.start();
    auto scores = (fa)      ? bc::SSBetweennessCentrality_EM(G, src)
                  : (ligra) ? bc::SSBetweennessCentrality(G, src)
                            : bc_bfs::SSBetweennessCentrality_BFS(G, src);
    double tt = t.stop();
    std::cout << "### Running Time: " << tt << std::endl;
    if (r == 0) {
      if (dump) {
        std::ofstream myfile;
        myfile.open("between.out");
        for (size_t i = 0; i < scores.size(); i++) {
          myfile << scores[i] << std::endl;
        }
        myfile.close();
      }
    } else {
      total_time += tt;
    }
  }
  auto time_per_iter = total_time / rounds;
  std::cout << "# time per iter: " << time_per_iter << "\n";
  return time_per_iter;
}

template <class Graph>
double Spanner_runner(Graph const& G, size_t rounds, bool dump, size_t k) {
  std::cout << "### Application: Spanner" << std::endl;
  std::cout << "### ------------------------------------" << std::endl;
  size_t n = G.N();
  double beta = log(n) / (2 * k);

  double total_time = 0.0;
  for (size_t r = 0; r <= rounds; r++) {
    timer t;
    t.start();
    auto spanner_ = spanner::Spanner(G, beta);
    double tt = t.stop();
    std::cout << "### Running Time: " << tt << std::endl;
    if (r == 0) {
      if (dump) {
        std::ofstream myfile;
        myfile.open("spanner.out");
        for (size_t i = 0; i < spanner_.size(); i++) {
          myfile << spanner_[i].first << ", " << spanner_[i].second
                 << std::endl;
        }
        myfile.close();
      }
    } else {
      total_time += tt;
    }
  }
  auto time_per_iter = total_time / rounds;
  std::cout << "# time per iter: " << time_per_iter << "\n";
  return time_per_iter;
}

template <class Graph>
double TriangleCount_runner(Graph const& G, size_t rounds) {
  std::cout << "### Application: Triangle_degree_ordering" << std::endl;
  std::cout << "### ------------------------------------" << std::endl;

  double total_time = 0.0;
  auto f = [&](uintE u, uintE v, uintE w) {};

  for (size_t r = 0; r <= rounds; r++) {
    timer t;
    t.start();
    auto count = Triangle_degree_ordering(G, f);
    double tt = t.stop();
    std::cout << "### Running Time: " << tt << " count = " << count
              << std::endl;
    if (r > 0) {
      total_time += tt;
    }
  }
  auto time_per_iter = total_time / rounds;
  std::cout << "# time per iter: " << time_per_iter << "\n";
  return time_per_iter;
}

namespace batch_insert_helpers {
// from numerical recipes
inline uint64_t hash64(uint64_t u) {
  uint64_t v = u * 3935559000370003845ul + 2691343689449507681ul;
  v ^= v >> 21;
  v ^= v << 37;
  v ^= v >> 4;
  v *= 4768777513237032717ul;
  v ^= v << 20;
  v ^= v >> 41;
  v ^= v << 5;
  return v;
}

// A cheap version of an inteface that should be improved
// Allows forking a state into multiple states
struct random {
 public:
  random(size_t seed) : state(seed) {};
  random() : state(0) {};
  random fork(uint64_t i) const { return random(hash64(hash64(i + state))); }
  random next() const { return fork(0); }
  size_t ith_rand(uint64_t i) const { return hash64(i + state); }
  size_t operator[](size_t i) const { return ith_rand(i); }
  size_t rand() { return ith_rand(0); }

 private:
  uint64_t state = 0;
};

// a 32-bit hash function
inline uint32_t hash32(uint32_t a) {
  a = (a + 0x7ed55d16) + (a << 12);
  a = (a ^ 0xc761c23c) ^ (a >> 19);
  a = (a + 0x165667b1) + (a << 5);
  a = (a + 0xd3a2646c) ^ (a << 9);
  a = (a + 0xfd7046c5) + (a << 3);
  a = (a ^ 0xb55a4f09) ^ (a >> 16);
  return a;
}

template <class intT>
struct rMat {
  double a, ab, abc;
  intT n;
  intT h;
  rMat(intT _n, intT _seed, double _a, double _b, double _c) {
    n = _n;
    a = _a;
    ab = _a + _b;
    abc = _a + _b + _c;
    h = hash32((intT)_seed);
    if (abc > 1) {
      std::cout << "in rMat: a + b + c add to more than 1\n";
      abort();
    }
    if ((1UL << parlay::log2_up(n)) != n) {
      std::cout << "in rMat: n not a power of 2";
      abort();
    }
  }

  double hashDouble(intT i) {
    return ((double)(hash32((intT)i)) /
            ((double)std::numeric_limits<int32_t>::max()));
  }

  std::pair<intT, intT> rMatRec(intT nn, intT randStart, intT randStride) {
    if (nn == 1)
      return std::make_pair<intT, intT>(0, 0);
    else {
      std::pair<intT, intT> x =
          rMatRec(nn / 2, randStart + randStride, randStride);
      double r = hashDouble(randStart);
      if (r < a)
        return x;
      else if (r < ab)
        return std::make_pair(x.first, x.second + nn / 2);
      else if (r < abc)
        return std::make_pair(x.first + nn / 2, x.second);
      else
        return std::make_pair(x.first + nn / 2, x.second + nn / 2);
    }
  }

  std::pair<intT, intT> operator()(intT i) {
    intT randStart = hash32((intT)(2 * i) * h);
    intT randStride = hash32((intT)(2 * i + 1) * h);
    return rMatRec(n, randStart, randStride);
  }
};
}  // namespace batch_insert_helpers

template <class Graph>
void Batch_insert_runner(std::map<std::string, double>& time_map, Graph& G,
                         size_t rounds, size_t max_batch, bool dump) {
  size_t n = G.N();

  batch_insert_helpers::random r;

  using pair_vertex = std::tuple<uintE, uintE>;
  size_t batch_size = 1;

  while (batch_size <= max_batch) {
    std::cout << "batch size: " << batch_size << "\n";
    double insert_time = 0;
    double remove_time = 0;
    double sort_time = 0;

    for (size_t ts = 0; ts <= rounds; ts++) {
      auto updates = parlay::sequence<pair_vertex>(batch_size);

      double a = 0.5;
      double b = 0.1;
      double c = 0.1;
      size_t nn = 1 << (parlay::log2_up(n) - 1);
      auto rmat = batch_insert_helpers::rMat<uintE>(nn, r.ith_rand(0), a, b, c);

      parallel_for(0, updates.size(), [&](size_t i) { updates[i] = rmat(i); });

      {
        timer st;
        timer sort_timer;
        double batch_sort_time = 0;
        st.start();
        G.insert_batch(updates.data(), updates.size());
        std::cout << G.M() << " edges after inserts\n";
        std::cout << G.edges().size() << " another way to count edges\n";

        double batch_time = st.stop();

        if (ts > 0) {
          insert_time += batch_time;
          sort_time += batch_sort_time;
        } else {
          if (dump) {
            G.write_adj(std::string("graph_after_batch_") +
                        std::to_string(batch_size) + std::string(".adj"));
          }
        }
        std::cout << "done inserts in " << batch_time << "\n";
        std::cout << "sorts took " << batch_sort_time << "\n";
      }
      std::random_device rd;
      std::mt19937 gen{rd()};
      std::ranges::shuffle(updates, gen);

      {
        timer st;
        st.start();
        G.remove_batch(updates.data(), updates.size());
        double batch_time = st.stop();

        if (ts > 0) {
          remove_time += batch_time;
        }
        std::cout << "done deletes in " << batch_time << "\n";
      }
    }

    std::string insert_key =
        std::string("batch_insert_") + std::to_string(batch_size);
    std::string remove_key =
        std::string("batch_remove_") + std::to_string(batch_size);
    std::string sort_key =
        std::string("batch_sort_") + std::to_string(batch_size);
    time_map[insert_key] = insert_time / rounds;
    time_map[remove_key] = remove_time / rounds;
    time_map[sort_key] = sort_time / rounds;
    batch_size *= 10;
  }
}

class run_all_options {
 public:
  uintE src = 0;
  size_t rounds = 3;
  bool dump = false;
  bool verify = false;
  double DensestSubgraph_eps = 0.001;
  double CoSimRank_eps = 0.000001;
  size_t CoSimRank_iters = 100;
  double c = .85;
  double u = 0;
  double v = 1;
  bool em = false;
  bool label_prop_permute = false;
  bool wecc_permute = false;
  double we_cc_beta = .2;
  double degen_eps = .1;
  bool coloringLF = false;
  size_t k_core_num_buckets = 16;
  bool k_core_fa = false;
  double ldd_beta = .2;
  bool ldd_permute = false;
  bool mis_spec_for = false;
  bool pagerank_em = false;
  bool pagerank_delta = false;
  double pagerank_eps = 1e-4;
  double pagerank_leps = .01;
  size_t pagerank_iters = 20;
  bool ssbetween_fa = false;
  bool ssbetween_ligra = false;
  size_t spanner_k = 4;
  size_t max_batch = 1000000UL;
  bool inserts = false;

  // for dzig incre alg
  char* input_file = nullptr;
  char* batch_file = nullptr;
  size_t batch_size = 1;
  size_t batch_num = 10;
  bool remove_batches_edges = true;
  std::string alg = "";
  size_t wbfs_num_buckets = 32;
  bool wbfs_no_blocked = false;
  bool wbfs_largemem = false;
};

template <class Graph>
void run_bfs(Graph const& G, run_all_options const& options) {
  static constexpr bool symmetric = Graph::symmetric;
  std::cout << "### Threads: " << num_workers() << std::endl;
  std::cout << "### n: " << G.N() << std::endl;
  std::cout << "### m: " << G.M() << std::endl;
  if (options.dump) {
    std::cout << "writing output arrays to files\n";
  }
  std::map<std::string, double> time_map;
  if (!options.inserts) {
    time_map["BFS"] = BFS_runner(G, options.src, options.rounds, options.dump);

    time_map["PageRank"] =
        PageRank_runner(G, options.rounds, options.dump, options.pagerank_iters,
                        options.pagerank_em, options.pagerank_delta,
                        options.pagerank_eps, options.pagerank_leps);
  }
  if (options.inserts) {
    auto edges = G.edges();
    for (int i = 0; i < 10; i++) {
      std::cout << std::get<0>(edges[i]) << ", " << std::get<1>(edges[i])
                << "\n";
    }
    if constexpr (Graph::support_insert_batch) {
      // Batch_insert_runner(time_map, const_cast<Graph &>(G), options.rounds,
      //                     options.max_batch, options.dump);
      parlay::random_shuffle(edges);
      G.remove_batch(edges.data(), edges.size());
    }
  }
  for (auto const& [alg, time_per_iter] : time_map) {
    std::cout << "BYO: => " << alg << ", " << time_per_iter << "\n";
  }
}

template <class Graph, class EdgeArray, typename Func>
void run_incre_alg(Graph& G, EdgeArray& batch_edges,
                   run_all_options const& options, Func&& func) {
  size_t insert_sz = options.batch_num * options.batch_size;

  if (insert_sz != batch_edges.size()) {
    std::cout << "skipping, total batch size is not equal to number of edges\n";
    return;
  }

  parlay::sequence<double> insert_time_seq(options.batch_num, 0.0);
  parlay::sequence<double> alg_time_seq(options.batch_num, 0.0);
  for (int i = 0; i < options.rounds; i++) {
    Graph dynamic_graph(G);
    // Graph dynamic_graph = G;

    if (options.remove_batches_edges) {
      // std::cout << "removing " << insert_sz << " edges from the graph\n";
      dynamic_graph.remove_batch(batch_edges.data(), insert_sz);
    }

    // begin insert batch
    for (int j = 0; j < options.batch_num; j++) {
      timer t;
      t.start();
      dynamic_graph.insert_batch(batch_edges.data() + j * options.batch_size,
                                 options.batch_size);
      // printf("new graph has %zu edges\n", dynamic_graph.edges().size());
      insert_time_seq[j] += t.stop();
      alg_time_seq[j] += func(dynamic_graph);
    }
  }

  std::cout << ">>> Res: " << std::endl;
  for (int j = 0; j < options.batch_num; j++) {
    printf("%.6f %.6f\n", insert_time_seq[j] / options.rounds,
           alg_time_seq[j] / options.rounds);
  }
  std::cout << std::flush;
  return;
}

template <class Graph>
class BatchEdgesIO {
 public:
  static void GenerateBatchEdges(Graph& G, run_all_options const& options,
                                 std::string output_path, size_t batch_size,
                                 size_t batch_num) {
    auto edges = G.edges();
    auto batch_edges = parlay::tabulate(edges.size(), [&](size_t i) {
      return std::make_tuple(std::get<0>(edges[i]), std::get<1>(edges[i]));
    });
    batch_edges = parlay::random_shuffle(batch_edges);
    std::cout << edges.size() << " edges in the graph\n";

    // Write the last batch_size * batch_num edges to output_path
    size_t total_batch_edges =
        static_cast<size_t>(batch_size) * static_cast<size_t>(batch_num);
    if (total_batch_edges > batch_edges.size()) {
      std::cout << "Warning: requested " << total_batch_edges
                << " edges but only " << batch_edges.size()
                << " edges available\n";
      total_batch_edges = batch_edges.size();
    }

    std::ofstream output_file(output_path);
    if (!output_file.is_open()) {
      std::cout << "Error: could not open output file " << output_path << "\n";
      return;
    }
    // output_file << total_batch_edges << "\n";

    size_t start_idx = batch_edges.size() - total_batch_edges;
    for (size_t i = start_idx; i < batch_edges.size(); i++) {
      output_file << std::get<0>(batch_edges[i]) << " "
                  << std::get<1>(batch_edges[i]) << "\n";
    }

    output_file.close();
    std::cout << "Wrote " << total_batch_edges << " edges to " << output_path
              << "\n";
  }

  template <class Seq>
  static parlay::sequence<char*> stringToWords(Seq& Str) {
    auto is_space = [](char c) {
      switch (c) {
        case '\r':
        case '\t':
        case '\n':
        case 0:
        case ' ':
          return true;
        default:
          return false;
      }
    };

    size_t n = Str.size();
    parlay::parallel_for(0, n, [&](long i) {
      if (is_space(Str[i])) Str[i] = 0;
    });
    // mark start of words
    auto FL = parlay::tabulate(n, [&](long i) -> bool {
      return (i == 0) ? Str[0] : Str[i] && !Str[i - 1];
    });
    // offset for each start of word
    auto Offsets = parlay::pack_index<long>(FL);
    // pointer to each start of word
    auto SA = parlay::tabulate(Offsets.size(), [&](long j) -> char* {
      return Str.begin() + Offsets[j];
    });

    return SA;
  }

  static parlay::sequence<std::tuple<uintE, uintE>> ReadBatchEdges(
      std::string const input_path, size_t batch_size, size_t batch_num) {
    std::ifstream input_file(input_path);
    if (!input_file.is_open()) {
      std::cerr << "Error: could not open input file " << input_path << "\n";
      return {};
    }

    // input_file >> total_batch_edges;
    parlay::sequence<char> S =
        gbbs::gbbs_io::readStringFromFile(input_path.c_str());
    parlay::sequence<char*> W = stringToWords(S);
    size_t total_batch_edges = batch_size * batch_num;
    if (total_batch_edges != W.size() / 2) {
      std::cout << total_batch_edges << " " << W.size() << std::endl;
      throw std::runtime_error(
          "The number of edges in a file does not match the given size");
      exit(1);
    }
    auto pts = W.cut(0, W.size());

    parlay::sequence<std::tuple<uintE, uintE>> edges(total_batch_edges);
    parlay::parallel_for(0, total_batch_edges, [&](size_t i) {
      uintE u = std::stoul(pts[i * 2]);
      uintE v = std::stoul(pts[i * 2 + 1]);
      edges[i] = std::make_tuple(u, v);
    });

    // for (size_t i = 0; i < total_batch_edges; i++) {
    // for (size_t i = 0; i < 10; i++) {
    //   std::cout << "Read edge: " << std::get<0>(edges[i]) << ", "
    //             << std::get<1>(edges[i]) << "\n";
    // }

    input_file.close();
    return edges;
  }
};

template <class Graph>
void batch_alg_wrapper(Graph const& G, run_all_options const& options) {
  static constexpr bool symmetric = Graph::symmetric;
  std::cout << "### Threads: " << num_workers() << std::endl;
  std::cout << "### n: " << G.N() << std::endl;
  std::cout << "### m: " << G.M() << std::endl;
  if (options.dump) {
    std::cout << "writing output arrays to files\n";
  }
  if constexpr (!Graph::support_insert_batch) {
    std::cout << "Graph does not support batch insertions, skipping.\n";
    return;
  }

  puts(">>> Begin -----------------------------");
  std::cout << ">>> Input_file " << std::string(options.input_file) << "\n";
  std::cout << ">>> Batch_file " << std::string(options.batch_file) << "\n";
  std::cout << ">>> Batch_num " << options.batch_num << "\n";
  std::cout << ">>> Batch_size " << options.batch_size << "\n";

  auto batch_edges = gbbs::BatchEdgesIO<decltype(G)>::ReadBatchEdges(
      options.batch_file, options.batch_size, options.batch_num);

  if (options.alg == "bfs") {
    std::cout << ">>> Alg BFS \n";
    run_incre_alg(const_cast<Graph&>(G), batch_edges, options,
                  [&](Graph const& dynamic_graph) {
                    return BFS_runner(dynamic_graph, options.src,
                                      options.rounds, options.dump);
                  });
  } else if (options.alg == "pagerank") {
    std::cout << ">>> Alg PageRank \n";
    run_incre_alg(const_cast<Graph&>(G), batch_edges, options,
                  [&](Graph const& dynamic_graph) {
                    return PageRank_runner(
                        dynamic_graph, options.rounds, options.dump,
                        options.pagerank_iters, options.pagerank_em,
                        options.pagerank_delta, options.pagerank_eps,
                        options.pagerank_leps);
                  });
  } else if (options.alg == "labelpropagation") {
    std::cout << ">>> Alg LabelPropagation \n";
    run_incre_alg(const_cast<Graph&>(G), batch_edges, options,
                  [&](Graph const& dynamic_graph) {
                    return LabelPropCC_runner(dynamic_graph, options.rounds,
                                              options.dump,
                                              options.label_prop_permute);
                  });
  } else if (options.alg == "wbfs") {
    std::cout << ">>> Alg wBFS_unweighted \n";
    run_incre_alg(const_cast<Graph&>(G), batch_edges, options,
                  [&](Graph const& dynamic_graph) {
                    return WBFS_unweighted_runner(
                        dynamic_graph, options.src, options.rounds,
                        options.wbfs_num_buckets, options.wbfs_no_blocked,
                        options.wbfs_largemem, options.dump);
                  });
  } else if (options.alg == "all") {  // NOTE: run all algorithms
    std::cout << ">>> Alg BFS \n";
    run_incre_alg(const_cast<Graph&>(G), batch_edges, options,
                  [&](Graph const& dynamic_graph) {
                    return BFS_runner(dynamic_graph, options.src,
                                      options.rounds, options.dump);
                  });

    std::cout << ">>> Alg PageRank \n";
    run_incre_alg(const_cast<Graph&>(G), batch_edges, options,
                  [&](Graph const& dynamic_graph) {
                    return PageRank_runner(
                        dynamic_graph, options.rounds, options.dump,
                        options.pagerank_iters, options.pagerank_em,
                        options.pagerank_delta, options.pagerank_eps,
                        options.pagerank_leps);
                  });

    std::cout << ">>> Alg LabelPropagation \n";
    run_incre_alg(const_cast<Graph&>(G), batch_edges, options,
                  [&](Graph const& dynamic_graph) {
                    return LabelPropCC_runner(dynamic_graph, options.rounds,
                                              options.dump,
                                              options.label_prop_permute);
                  });

    std::cout << ">>> Alg wBFS_unweighted \n";
    run_incre_alg(const_cast<Graph&>(G), batch_edges, options,
                  [&](Graph const& dynamic_graph) {
                    return WBFS_unweighted_runner(
                        dynamic_graph, options.src, options.rounds,
                        options.wbfs_num_buckets, options.wbfs_no_blocked,
                        options.wbfs_largemem, options.dump);
                  });
  } else {
    std::cout << ">>> Unknown alg: " << options.alg << "\n";
  }
  puts(">>> End -----------------------------");
}

template <class Graph>
void run_all(Graph const& G, run_all_options const& options) {
  static constexpr bool symmetric = Graph::symmetric;
  std::cout << "### Threads: " << num_workers() << std::endl;
  std::cout << "### n: " << G.N() << std::endl;
  std::cout << "### m: " << G.M() << std::endl;
  if (options.dump) {
    std::cout << "writing output arrays to files\n";
  }
  std::map<std::string, double> time_map;
  if (!options.inserts) {
    if constexpr (symmetric) {
      time_map["Map"] = Map_runner(G, options.rounds);
      time_map["Map_withRemote_0"] =
          Map_Remote_runner<Graph, 0>(G, options.rounds);
      time_map["Map_withRemote_1"] =
          Map_Remote_runner<Graph, 1>(G, options.rounds);
      time_map["Map_withRemote_2"] =
          Map_Remote_runner<Graph, 2>(G, options.rounds);
      time_map["Map_withRemote_4"] =
          Map_Remote_runner<Graph, 4>(G, options.rounds);
      time_map["Map_withRemote_8"] =
          Map_Remote_runner<Graph, 8>(G, options.rounds);
      time_map["Map_withRemote_16"] =
          Map_Remote_runner<Graph, 16>(G, options.rounds);
      time_map["Map_withRemote_32"] =
          Map_Remote_runner<Graph, 32>(G, options.rounds);
      time_map["Map_withRemote_64"] =
          Map_Remote_runner<Graph, 64>(G, options.rounds);
    }
    time_map["BFS"] = BFS_runner(G, options.src, options.rounds, options.dump);
    if constexpr (symmetric) {
      time_map["WorkEfficientDensestSubgraph"] =
          WorkEfficientDensestSubgraph_runner(G, options.DensestSubgraph_eps,
                                              options.rounds);
      // time_map["CharikarAppxDensestSubgraph"] =
      //     CharikarAppxDensestSubgraph_runner(G, options.rounds);
    }
    // time_map["CoSimRank"] = CoSimRank_runner(
    //     G, options.CoSimRank_eps, options.CoSimRank_iters, options.c,
    //     options.u, options.v, options.em, options.rounds);
    // if constexpr (symmetric) {
    //   time_map["BFSCC"] = BFSCC_runner(G, options.rounds, options.dump);
    // }
    // time_map["LabelPropCC"] = LabelPropCC_runner(G, options.rounds,
    // options.dump,
    //                                              options.label_prop_permute);
    time_map["SimpleUnionCC"] =
        SimpleUnionCC_runner(G, options.rounds, options.dump);

    // if constexpr (symmetric) {
    //   time_map["WorkEfficientCC"] =
    //       WorkEfficientCC_runner(G, options.rounds, options.dump,
    //                              options.wecc_permute, options.we_cc_beta);
    //   // time_map["DegeneracyOrder"] = DegeneracyOrder_runner(
    //   //     G, options.rounds, options.dump, options.degen_eps);
    // }

    if constexpr (symmetric) {
      time_map["Coloring"] = Coloring_runner(
          G, options.rounds, options.dump, options.coloringLF, options.verify);
      time_map["KCore"] =
          KCore_runner(G, options.rounds, options.dump, options.k_core_fa,
                       options.k_core_num_buckets);
      time_map["LDD"] = LDD_runner(G, options.rounds, options.dump,
                                   options.ldd_beta, options.ldd_permute);

      time_map["MIS"] = MIS_runner(G, options.rounds, options.dump,
                                   options.mis_spec_for, options.verify);
    }

    time_map["PageRank"] =
        PageRank_runner(G, options.rounds, options.dump, options.pagerank_iters,
                        options.pagerank_em, options.pagerank_delta,
                        options.pagerank_eps, options.pagerank_leps);

    time_map["SSBetweennessCentrality"] = SSBetweennessCentrality_runner(
        G, options.rounds, options.dump, options.src, options.ssbetween_fa,
        options.ssbetween_ligra);

    if constexpr (symmetric) {
      time_map["Spanner"] =
          Spanner_runner(G, options.rounds, options.dump, options.spanner_k);
    }
    if (G.M() < 1000000000) {
      time_map["TriangleCount"] = TriangleCount_runner(G, options.rounds);
    }
  }

  if (options.inserts) {
    // MODIFIES THE GRAPH
    // run last so we run everything else on the same graph
    // we modify the graph to avoid having to do a copy
    if constexpr (Graph::support_insert_batch) {
      Batch_insert_runner(time_map, const_cast<Graph&>(G), options.rounds,
                          options.max_batch, options.dump);
    }
  }

  for (auto const& [alg, time_per_iter] : time_map) {
    std::cout << "# # # " << alg << ", " << time_per_iter << "\n";
  }
}

}  // namespace gbbs
