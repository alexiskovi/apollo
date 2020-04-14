#include "modules/common/time/time.h"
#include "modules/supervisor/sv_ipc_component.h"

namespace apollo {
namespace supervisor {

bool IPCSupervisor::Init() {
  return true;
}

bool IPCSupervisor::Proc() {
  AERROR << "triggered";
  return true;
}

}
}