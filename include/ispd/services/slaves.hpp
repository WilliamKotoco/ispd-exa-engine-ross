#ifndef ISPD_EXA_SLAVES_HPP
#define ISPD_EXA_SLAVES_HPP

#include <ross.h>
namespace ispd {
namespace services {
struct slaves {
  tw_lpid id;
  unsigned cpuCoreCount;
  double powerPerCore;
  unsigned gpuCoreCount;
  double gpuPowerPerCore;
  unsigned runningTasks;
  double runningMflops;

  float priority; /// numerical value that represents the priority over other
                  /// machines
};

}; // namespace services

}; // namespace ispd
#endif
