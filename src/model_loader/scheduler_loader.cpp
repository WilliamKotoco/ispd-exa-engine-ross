#include <ispd/model_loader/scheduler_loader.hpp>

struct FileInterpreter readSchedulerFile(std::string path) {
  struct FileInterpreter fileInterpreter;
  ispd_debug(path);
  std::ifstream scheduler_file(path);
  std::string line;

  if (!scheduler_file.is_open())
    ispd_error("Error: could not open scheduler's file %s.", path.c_str());

  while (std::getline(scheduler_file, line)) {
    std::string token;
    std::istringstream iss(line);

    iss >> token;

    if (token == "SCHEDULER") {
      iss >> fileInterpreter.name;
    } else if (token == "RESTRICT") {
      std::string restriction;
      iss >> restriction;
      if (restriction == "NO")
        fileInterpreter.restriction = -1;
      else
        fileInterpreter.restriction = std::stoi(restriction);
    }

    else if (token == "DYNAMIC" || token == "STATIC")
      fileInterpreter.isDynamic = (token == "DYNAMIC");

    else if (token == "FORMULA") {
      std::string type;
      iss >> type;

      if(type == "FIFO")
      {
        fileInterpreter.type = FIFO;
      }
      else if (type == "RANDOM")
      {
        fileInterpreter.type = RANDOM;
      }
      else if (type == "INCREASING")
      {
        fileInterpreter.type = INCREASING;
        fileInterpreter.formula = line.erase(0, 19);
        fileInterpreter.formula.append((" + 0 * (cpuCores + cpuPP + gpuCores + gpuPP + TPS + TCS + TOFF + RMFE)"));

      }
      else
      {
        fileInterpreter.type = DECREASING;
        fileInterpreter.formula = line.erase(0, 18);
        fileInterpreter.formula.append((" + 0 * (cpuCores + cpuPP + gpuCores + gpuPP + TPS + TCS + TOFF + RMFE)"));


      }

    }
  }




  ispd_debug("Custom scheduler loaded: \n "
             "Name :%s \n"
             "Type: %d \n"
             "Dynamic: %d \n"
             "Restriction %d \n"
             "Formula: %s",
             fileInterpreter.name.c_str(), fileInterpreter.type, fileInterpreter.isDynamic,
             fileInterpreter.restriction, fileInterpreter.formula.c_str());
  return fileInterpreter;
}