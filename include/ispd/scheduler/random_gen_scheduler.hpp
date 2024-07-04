#pragma once
#include <ross.h>
#include <vector>
#include <ispd/message/message.hpp>
#include <ispd/model_loader/scheduler_loader.hpp>
#include <ispd/scheduler/scheduler.hpp>

namespace ispd::scheduler {

class Random final : public Scheduler {
private:
  FileInterpreter scheduler;

public:
  void initScheduler(std::vector<ispd::services::slaves> &slaves,
                     std::string file_name) override {
    scheduler = readSchedulerFile(file_name);
  }

  void updateInformation(std::vector<ispd::services::slaves> &slaves, tw_bf *bf,
                         ispd_message *msg, tw_lp *lp) override {

    /// if it is a feedback message, it is necessary to update
    /// the machine information
    slaves.at(msg->machine_position).runningTasks--;
  }

  void reverseUpdate(std::vector<ispd::services::slaves> &slaves, tw_bf *bf,
                     ispd_message *msg, tw_lp *lp) override {
    slaves.at(msg->machine_position).runningTasks++;
    return;
  }
  [[nodiscard]] tw_lpid
  forwardSchedule(std::vector<ispd::services::slaves> &slaves, tw_bf *bf,
                  ispd_message *msg, tw_lp *lp) override {

    long pos;

    do {
      /// generate a random position within the boundaries of the vector
      pos = tw_rand_integer(lp->rng, 0, slaves.size() - 1);
    } while (slaves.at(pos).runningTasks > scheduler.restriction &&
             scheduler.restriction >= 0);

    msg->machine_position = pos;

    slaves.at(pos).runningTasks++;

    return slaves.at(pos).id;
  }

  void reverseSchedule(std::vector<ispd::services::slaves> &slaves, tw_bf *bf,
                       ispd_message *msg, tw_lp *lp) override {

    tw_rand_reverse_unif(lp->rng);
  }
};

}; // namespace ispd::scheduler
