#pragma once
#include <ross.h>
#include <vector>
#include <ispd/message/message.hpp>
#include <ispd/model_loader/scheduler_loader.hpp>
#include <ispd/scheduler/scheduler.hpp>
#include <tinyexpr/tinyexpr.h>
#include <set>

namespace ispd::scheduler {

struct compare_machine_inc {
  bool operator()(const ispd::services::slaves &lhs,
                  const ispd::services::slaves &rhs) const {
    return lhs.priority > rhs.priority;
  }
};

class Increasing final : public Scheduler {
private:
  FileInterpreter scheduler;
  std::set<ispd::services::slaves, compare_machine_inc>
      queue;     /// priority queue
  te_expr *expr; /// expression to evaluate the formula

  double eval(ispd::services::slaves &slave, double TPS_, double TCS_,
              double TOFF_) {
    int cpuCores = slave.cpuCoreCount;
    double cpuPP = slave.powerPerCore;
    int gpuCores = slave.gpuCoreCount;
    double gpuPP = slave.gpuPowerPerCore;
    double TPS = TPS_;
    double TCS = TCS_;
    double TOFF = TOFF_;
    double RMFE = slave.runningMflops;

    te_variable vars[] = {{"cpuCores", &cpuCores}, {"cpuPP", &cpuPP},
                          {"gpuCores", &gpuCores}, {"gpuPP", &gpuPP},
                          {"TPS", &TPS},           {"TCS", &TCS},
                          {"TOFF", &TOFF},         {"RMFE", &RMFE}};
    int err;
    /// if there are no expression, creates the expression from the scheduler
    /// formula and evaluates.
    if (!this->expr) {
      const char *c = scheduler.formula.c_str();
      this->expr = te_compile(c, vars, 8, &err);

      if (this->expr) {

        return te_eval(this->expr);
      } else {
        ispd_error("error at %d with formula ", err, scheduler.formula.c_str());
      }

      /// expression were already created, return the evaluation using it.
    } else
      return te_eval(this->expr);

    return 0;
  }

public:
  void initScheduler(std::vector<ispd::services::slaves> &slaves,
                     std::string file_name) override {

    scheduler = readSchedulerFile(file_name);
    for (auto &i : slaves) {
      i.priority = eval(
          i, 0, 0, 0); /// the first evaluation does not use task information
      queue.insert(i);
    }
  }

  void updateInformation(std::vector<ispd::services::slaves> &slaves, tw_bf *bf,
                         ispd_message *msg, tw_lp *lp) override {
    slaves.at(msg->machine_position).runningTasks--;
    slaves.at(msg->machine_position).runningMflops -= msg->task.m_ProcSize;
  }

  [[nodiscard]] tw_lpid
  forwardSchedule(std::vector<ispd::services::slaves> &slaves, tw_bf *bf,
                  ispd_message *msg, tw_lp *lp) override {

    int position = queue.cbegin()->position;

    tw_lpid machine = slaves.at(position).id;

    //      ispd_error("%lu ", machine);
    slaves.at(position).runningTasks++;
    slaves.at(position).runningMflops += msg->task.m_ProcSize;
    /// removes from queue if there are more running tasks than the restriction
    /// allows
    if (slaves.at(position).runningTasks >= scheduler.restriction &&
        scheduler.restriction != -1) {

      queue.erase(queue.cbegin());
    }

    /// recalculate priority and adds back in the queue if there is available
    /// space based on the arrived machine'
    if (slaves.at(msg->machine_position).runningTasks < scheduler.restriction &&
        scheduler.restriction != -1 && msg->type == message_type::FEEDBACK) {
      ispd_debug("Running tasks: %d ",
                 slaves.at(msg->machine_position).runningTasks);

      slaves.at(msg->machine_position).priority =
          eval(slaves.at(msg->machine_position), msg->task.m_ProcSize,
               msg->task.m_CommSize, msg->task.m_Offload);

      queue.insert(slaves.at(msg->machine_position));
      ispd_debug("Machine size after insertion %u", queue.size());
    }

    msg->machine_position = position;

    return machine;
  }

  void reverseUpdate(std::vector<ispd::services::slaves> &slaves, tw_bf *bf,
                     ispd_message *msg, tw_lp *lp) override {
    return;
  }

  void reverseSchedule(std::vector<ispd::services::slaves> &slaves, tw_bf *bf,
                       ispd_message *msg, tw_lp *lp) override {
    return;
  }
};

}; // namespace ispd::scheduler