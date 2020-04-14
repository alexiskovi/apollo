#pragma once

#include <memory>
#include <vector>

#include "cyber/component/timer_component.h"
#include "cyber/cyber.h"

namespace apollo {
namespace supervisor {

class IPCSupervisor : public apollo::cyber::TimerComponent {
 public:
  bool Init() override;
  bool Proc() override;
};

CYBER_REGISTER_COMPONENT(IPCSupervisor)

}
}
