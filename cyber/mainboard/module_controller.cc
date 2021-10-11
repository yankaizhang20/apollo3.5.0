/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#include "cyber/mainboard/module_controller.h"

#include <utility>

#include "cyber/common/environment.h"
#include "cyber/common/file.h"
#include "cyber/component/component_base.h"

namespace apollo {
namespace cyber {
namespace mainboard {

ModuleController::ModuleController(const ModuleArgument& args) { args_ = args; }

ModuleController::~ModuleController() {}

bool ModuleController::Init() { return LoadAll(); }

void ModuleController::Clear() {
        for (auto& component : component_list_) {
                component->Shutdown();
        }
        component_list_.clear(); // keep alive
        class_loader_manager_.UnloadAllLibrary();
}

bool ModuleController::LoadAll() {
        //@zyk：获取环境变量CYBER_PATH的值，如果没有则为/apollo/cyber
        const std::string work_root = common::WorkRoot();
        const std::string current_path = common::GetCurrentPath();
        //@zyk:dag_root_path==/apollo/cyber/dag
        const std::string dag_root_path = common::GetAbsolutePath(work_root, "dag");
        //@zyk:处理命令行参数-d(dag_conf)
        for (auto& dag_conf : args_.GetDAGConfList()) {
                std::string module_path = "";
                //@zyk：获得dag文件的绝对路径
                if (dag_conf == common::GetFileName(dag_conf)) {
                        // case dag conf argument var is a filename
                        module_path = common::GetAbsolutePath(dag_root_path, dag_conf);
                } else if (dag_conf[0] == '/') {
                        // case dag conf argument var is an absolute path
                        module_path = dag_conf;
                } else {
                        // case dag conf argument var is a relative path
                        module_path = common::GetAbsolutePath(current_path, dag_conf);
                        if (!common::PathExists(module_path)) {
                                module_path = common::GetAbsolutePath(work_root, dag_conf);
                        }
                } //@zyk:module_path="/apollo/modules/planning/dag/planning.dag"
                AINFO << "Start initialize dag: " << module_path;
                if (!LoadModule(module_path)) {
                        AERROR << "Failed to load module: " << module_path;
                        return false;
                }
        }
        return true;
}
/* planning.dag
module_config {
  module_library : "/apollo/bazel-bin/modules/planning/libplanning_component.so"
  components {
    class_name : "PlanningComponent"
    config {
      name: "planning"
      flag_file_path:  "/apollo/modules/planning/conf/planning.conf"
      readers: [
        {
          channel: "/apollo/prediction"
        },
        {
          channel: "/apollo/canbus/chassis"
          qos_profile: {
              depth : 15
          }
          pending_queue_size: 50
        },
        {
          channel: "/apollo/localization/pose"
          qos_profile: {
              depth : 15
          }
          pending_queue_size: 50
        }
      ]
    }
  }
}
*/
bool ModuleController::LoadModule(const DagConfig& dag_config) {
        const std::string work_root = common::WorkRoot();

        for (auto module_config : dag_config.module_config()) {
                std::string load_path;
                if (module_config.module_library().front() == '/') {
                        load_path = module_config.module_library();
                } else {
                        load_path = common::GetAbsolutePath(work_root, module_config.module_library());
                }

                if (!common::PathExists(load_path)) {
                        AERROR << "Path not exist: " << load_path;
                        return false;
                }
                //@zyk:LoadLibrary("/apollo/bazel-bin/modules/planning/libplanning_component.so")
                //@TODO:关于具体如何加载库还需研究
                class_loader_manager_.LoadLibrary(load_path);
                //@zyk:创建PlanningComponent类
                for (auto& component : module_config.components()) {
                        //@zyk:class_name=="PlanningComponent"
                        const std::string& class_name = component.class_name();
                        //TODO:用工厂模式创建组件对象PlanningComponent的具体过程有待研究
                        std::shared_ptr<ComponentBase> base =
                                class_loader_manager_.CreateClassObj<ComponentBase>(class_name);
                        if (base == nullptr) {
                                return false;
                        }

                        if (!base->Initialize(component.config())) {
                                return false;
                        }
                        component_list_.emplace_back(std::move(base));
                }

                for (auto& component : module_config.timer_components()) {
                        const std::string& class_name = component.class_name();
                        std::shared_ptr<ComponentBase> base =
                                class_loader_manager_.CreateClassObj<ComponentBase>(class_name);
                        if (base == nullptr) {
                                return false;
                        }

                        if (!base->Initialize(component.config())) {
                                return false;
                        }
                        component_list_.emplace_back(std::move(base));
                }
        }
        return true;
}
//@zyk: LoadModule("/apollo/modules/planning/dag/planning.dag")
/* planning.dag
module_config {
  module_library : "/apollo/bazel-bin/modules/planning/libplanning_component.so"
  components {
    class_name : "PlanningComponent"
    config {
      name: "planning"
      flag_file_path:  "/apollo/modules/planning/conf/planning.conf"
      readers: [
        {
          channel: "/apollo/prediction"
        },
        {
          channel: "/apollo/canbus/chassis"
          qos_profile: {
              depth : 15
          }
          pending_queue_size: 50
        },
        {
          channel: "/apollo/localization/pose"
          qos_profile: {
              depth : 15
          }
          pending_queue_size: 50
        }
      ]
    }
  }
}
*/
bool ModuleController::LoadModule(const std::string& path) {
        DagConfig dag_config;
        if (!common::GetProtoFromFile(path, &dag_config)) {
                AERROR << "Get proto failed, file: " << path;
                return false;
        }
        return LoadModule(dag_config);
}

} // namespace mainboard
} // namespace cyber
} // namespace apollo
