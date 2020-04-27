#include "modules/supervisor/software/gnss_supervisor.h"

DEFINE_double(gnss_check_period, 0.2,
            "Interval in seconds.");
DEFINE_double(gnss_timeout_sec, 2.0,
            "Timeout in seconds.");            
DEFINE_string(sv_gnss_conf_file, "/apollo/modules/supervisor/conf/gnss_conf.pb.txt",
            "Path to GNSS conf file");


namespace apollo {
namespace supervisor {

GNSSSupervisor::GNSSSupervisor() 
    : SupervisorRunner("GNSSSupervisor", FLAGS_gnss_check_period) {
  sv_sub_node_ = apollo::cyber::CreateNode("sv_gnss_sub_node");
  best_pose_reader_ = sv_sub_node_->CreateReader<apollo::drivers::gnss::GnssBestPose>("/apollo/sensor/gnss/best_pose", nullptr);
  ACHECK(best_pose_reader_ != nullptr);
  ACHECK(
    cyber::common::GetProtoFromFile(FLAGS_sv_gnss_conf_file, &gnss_conf_))
    << "Unable to load gnss conf file: " + FLAGS_sv_gnss_conf_file;
}

void GNSSSupervisor::RunOnce(const double current_time) {
  status_ = 0;

  best_pose_reader_->Observe();
  const auto &best_pose_msg = best_pose_reader_->GetLatestObserved();

  if(best_pose_msg == nullptr) {
    AERROR << "GNSS message is not ready";
    return void();
  }
  // Check timeout
  if(current_time - best_pose_msg->header().timestamp_sec() > FLAGS_gnss_timeout_sec){
    status_ = gnss_conf_.timeout_priority();
    debug_msg_ = "GNSS message timeout";
  }
  // Check differential age
  if(best_pose_msg->differential_age() > gnss_conf_.max_differential_age()){
    status_ = gnss_conf_.differential_age_timeout_priority();
    debug_msg_ = "GNSS differential age timeout";
  }
  // Check solution status
  if(best_pose_msg->sol_status() != apollo::drivers::gnss::SolutionStatus::SOL_COMPUTED){
    status_ = gnss_conf_.bad_sol_status_priority();
    debug_msg_ = "GNSS invalid solution status";
  }
  // Check sol type
  
}

void GNSSSupervisor::GetStatus(int* status, std::string* debug_msg) {
  *status = status_;
  *debug_msg = debug_msg_;
}

}
}