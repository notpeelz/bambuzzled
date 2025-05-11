#pragma once

#include <string>
#include <functional>

namespace Slic3r {

class BBLProfile;
class BBLTask;
class BBLModelTask;
class BBLSliceInfo;
class BBLSubTask;

class BBLModelTask {
public:
  BBLModelTask();
  ~BBLModelTask() {}

  int job_id;
  int design_id;
  int profile_id;
  int instance_id;
  std::string task_id;
  std::string model_id;
  std::string model_name;
  std::string profile_name;
};

typedef std::function<void(BBLModelTask* subtask)> OnGetSubTaskFn;

}
