#ifndef ISPD_EXA_SLAVES_HPP
#define ISPD_EXA_SLAVES_HPP

#include <ross.h>
namespace ispd {
namespace services {
struct slaves {
  tw_lpid id;
  int position;
  unsigned cpuCoreCount;
  double powerPerCore;
  unsigned gpuCoreCount;
  double gpuPowerPerCore;
  int runningTasks;
  double runningMflops;

  float priority; /// numerical value that represents the priority over other
                  /// machines

  bool operator==(const slaves &other) const {
    return id == other.id; // Compare based on id
  }
};

}; // namespace services

}; // namespace ispd
#endif
