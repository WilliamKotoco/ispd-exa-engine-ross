///\file scheduler_loader.hpp
///
/// \brief Interpreter of scheduler's file generated using the scheduler
/// generator.
///
/// Stores the file's information in a struct for easy retrieval of information
/// when building the scheduler.
#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ispd/log/log.hpp>

enum sched_type { INCREASING, DECREASING, FIFO, RANDOM };

/// \struct FileInterpreter
///
/// \brief Stores scheduler's file information.
struct FileInterpreter {
  std::string name;
  std::string formula;
  int restriction;
  bool isDynamic;
  sched_type type;
};

struct FileInterpreter readSchedulerFile(std::string path);