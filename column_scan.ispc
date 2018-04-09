#define KEY_TYPE unsigned int32

export void spmd_column_scan(uniform KEY_TYPE column[],
                             uniform KEY_TYPE result[],
                             uniform KEY_TYPE lower_bound,
                             uniform KEY_TYPE upper_bound,
                             uniform KEY_TYPE count) {
  foreach(index = 0 ... count) {
    if (lower_bound <= column[index] && upper_bound >= column[index]) {
      result[index] = 1;
    } else {
      result[index] = 0;
    }
  }
}

export void spmd_column_scan_bf(uniform KEY_TYPE column[],
                                uniform KEY_TYPE result[],
                                uniform KEY_TYPE lower_bound,
                                uniform KEY_TYPE upper_bound,
                                uniform KEY_TYPE count) {
  foreach(index = 0 ... count) {
    result[index] = (lower_bound <= column[index] && upper_bound >= column[index]);
  }
}
