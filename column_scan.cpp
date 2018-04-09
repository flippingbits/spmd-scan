#include <sys/time.h>

#include <algorithm>
#include <iostream>
#include <limits>

#include "column_scan_ispc.h"

static double gettime(void) {
  struct timeval now_tv;
  gettimeofday (&now_tv,NULL);
  return ((double)now_tv.tv_sec) + ((double)now_tv.tv_usec) / 1000000.0;
}

// column scan implementation with branches
void column_scan(uint32_t* column, uint32_t* result, uint32_t lower_bound, uint32_t upper_bound, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (column[i] >= lower_bound && column[i] <= upper_bound) {
      result[i] = 1;
    } else {
      result[i] = 0;
    }
  }
}

// column scan implementation without branches
void column_scan_bf(uint32_t* column, uint32_t* result, uint32_t lower_bound, uint32_t upper_bound, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    result[i] = (column[i] >= lower_bound && column[i] <= upper_bound);
  }
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " num_elements selectivity" << std::endl;
    exit(0);
  }

  std::srand(0);

  size_t n = std::atoi(argv[1]);
  float selectivity = std::atof(argv[2]);
  uint32_t* column_data  = new uint32_t[n];
  uint32_t* results = new uint32_t[n];

  std::cout << "Keys: " << n << " Query Selectivity: " << selectivity * 100 << "%" << std::endl;

  // fill column with random integers
  for (size_t i = 0; i < n; ++i)
    column_data[i] = std::rand();

  // generate query objects
  size_t num_queries = 10;
  uint32_t* lower_bounds = new uint32_t[num_queries];
  uint32_t* upper_bounds = new uint32_t[num_queries];
  for (size_t i = 0; i < num_queries; ++i) {
    // select lower bound
    lower_bounds[i] = column_data[rand() %n];
    // determine upper bound depending on selectivity
    upper_bounds[i] = lower_bounds[i] + selectivity * std::numeric_limits<int>::max();
  }

  double start = gettime();
  for (size_t i = 0; i < num_queries; ++i) {
    column_scan(column_data, results, lower_bounds[i], upper_bounds[i], n);
  }
  double throughput_scalar = (num_queries / (gettime() - start));
  std::cout << "Throughput (column scan/branching): " << throughput_scalar << " ops/s." << std::endl;

  start = gettime();
  for (size_t i = 0; i < num_queries; ++i) {
    column_scan_bf(column_data, results, lower_bounds[i], upper_bounds[i], n);
  }
  double throughput_scalar_bf = (num_queries / (gettime() - start));
  std::cout << "Throughput (column scan/branch-free): " << throughput_scalar_bf << " ops/s." << std::endl;

  start = gettime();
  for (size_t i = 0; i < num_queries; ++i) {
    ispc::spmd_column_scan(column_data, results, lower_bounds[i], upper_bounds[i], n);
  }
  double throughput_ispc = (num_queries / (gettime() - start));
  std::cout << "Throughput (SPMD column scan/branching): " << throughput_ispc << " ops/s." << std::endl;

  start = gettime();
  for (size_t i = 0; i < num_queries; ++i) {
    ispc::spmd_column_scan_bf(column_data, results, lower_bounds[i], upper_bounds[i], n);
  }
  double throughput_ispc_bf = (num_queries / (gettime() - start));
  std::cout << "Throughput (SPMD column scan/branch-free): " << throughput_ispc_bf << " ops/s." << std::endl;

  std::cout << "Speedup branching: " << throughput_ispc / throughput_scalar << std::endl;
  std::cout << "Speedup branch-free: " << throughput_ispc_bf / throughput_scalar_bf << std::endl;

  delete[] column_data;
  delete[] results;
  delete[] lower_bounds;
  delete[] upper_bounds;

  return 0;
}
