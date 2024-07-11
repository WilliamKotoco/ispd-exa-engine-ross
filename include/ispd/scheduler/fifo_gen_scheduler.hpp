#pragma once
#include <ross.h>
#include <vector>
#include <ispd/message/message.hpp>
#include <ispd/model_loader/scheduler_loader.hpp>
#include <ispd/scheduler/scheduler.hpp>

#include <queue>
namespace ispd::scheduler {

class FIFO final : public Scheduler {
private:
  FileInterpreter scheduler;
  std::queue<unsigned> machines;

public:
  void initScheduler(std::vector<ispd::services::slaves> &slaves,
                     std::string file_name) override {
    scheduler = readSchedulerFile(file_name);

    for (auto &i : slaves) {
      machines.push(i.position);
    }
  }

  void updateInformation(std::vector<ispd::services::slaves> &slaves, tw_bf *bf,
                         ispd_message *msg, tw_lp *lp) override {

    slaves.at(msg->machine_position).runningTasks--;

    slaves.at(msg->machine_position).runningMflops -= msg->task.m_ProcSize;

    if (slaves.at(msg->machine_position).runningTasks <=
            scheduler.restriction ||
        scheduler.restriction == -1)
      /// push the old machine back to queue
      machines.push(msg->machine_position);
    // ispd_error("Now the front is %u and the back is %u", machines.front(),
    // machines.back());
  }

  [[nodiscard]] tw_lpid
  forwardSchedule(std::vector<ispd::services::slaves> &slaves, tw_bf *bf,
                  ispd_message *msg, tw_lp *lp) override {

    unsigned next = machines.front();
    slaves.at(next).runningTasks++;
    slaves.at(next).runningMflops += msg->task.m_ProcSize;

    msg->machine_position = next;

    /// removes from queue if the machine now surpass the restriction or if
    /// there are no restrictions
    if (slaves.at(next).runningTasks > scheduler.restriction) {
      bf->c0 = 1;
      machines.pop();
    }

    return slaves.at(next).id;
  }

  void reverseUpdate(std::vector<ispd::services::slaves> &slaves, tw_bf *bf,
                     ispd_message *msg, tw_lp *lp) override {
    //    slaves.at(msg->machine_position).runningTasks++;
    //    slaves.at(msg->machine_position).runningMflops +=
    //    msg->task.m_ProcSize;
    //
    //    if (slaves.at(msg->machine_position).runningTasks >
    //    scheduler.restriction)
    //      machines.pop();
  }

  void reverseSchedule(std::vector<ispd::services::slaves> &slaves, tw_bf *bf,
                       ispd_message *msg, tw_lp *lp) override {

    //    unsigned previous = msg->machine_position;
    //    slaves.at(previous).runningTasks--;
    //    slaves.at(previous).runningMflops -= msg->task.m_ProcSize;
    //
    //    if (bf->c0 == 1) {
    //      bf->c0 = 0;
    //      machines.push(previous);
    //    }

    ispd_error("distributed execution not allowed for FIFO yet");
  }
};

}; // namespace ispd::scheduler
