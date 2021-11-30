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

#pragma once

#include <memory>

#include "cyber/class_loader/class_loader.h"
#include "cyber/component/component.h"
#include "cyber/message/raw_message.h"
#include "modules/canbus/proto/chassis.pb.h"
#include "modules/localization/proto/localization.pb.h"
#include "modules/perception/proto/traffic_light_detection.pb.h"
#include "modules/planning/common/planning_gflags.h"
#include "modules/planning/open_space_planning.h"
#include "modules/planning/planning_base.h"
#include "modules/planning/proto/pad_msg.pb.h"
#include "modules/planning/proto/planning.pb.h"
#include "modules/planning/proto/planning_config.pb.h"
#include "modules/planning/std_planning.h"
#include "modules/prediction/proto/prediction_obstacle.pb.h"
#include "modules/routing/proto/routing.pb.h"

namespace apollo {
namespace planning {
//@zyk:Planning的组件，相当于ROS的节点，Planning的一切从这里开始
class PlanningComponent final
    : public cyber::Component<prediction::PredictionObstacles, canbus::Chassis, localization::LocalizationEstimate> {
    public:
        PlanningComponent() = default;
        ~PlanningComponent() = default;

    public:
        bool Init() override;

        bool Proc(const std::shared_ptr<prediction::PredictionObstacles>& prediction_obstacles,
                  const std::shared_ptr<canbus::Chassis>& chassis,
                  const std::shared_ptr<localization::LocalizationEstimate>& localization_estimate) override;

    private:
        void CheckRerouting();
        bool CheckInput();
        //@zyk:订阅者
        std::shared_ptr<cyber::Reader<perception::TrafficLightDetection>> traffic_light_reader_;
        std::shared_ptr<cyber::Reader<routing::RoutingResponse>> routing_reader_;
        std::shared_ptr<cyber::Reader<planning::PadMessage>> pad_message_reader_;
        std::shared_ptr<cyber::Reader<relative_map::MapMsg>> relative_map_reader_;
        //@zyk:发布者
        std::shared_ptr<cyber::Writer<ADCTrajectory>> planning_writer_;
        std::shared_ptr<cyber::Writer<routing::RoutingRequest>> rerouting_writer_;

        std::mutex mutex_;
        //@zyk:环境理解
        perception::TrafficLightDetection traffic_light_; //@zyk:交通灯
        routing::RoutingResponse routing_;                //@zyk:路由
        PadMessage pad_message_;                          //@zyk:pad消息
        relative_map::MapMsg relative_map_;               //@zyk:地图信息

        LocalView local_view_; //@zyk:综合信息
        //@zyk:规划器   
        std::unique_ptr<PlanningBase> planning_base_;   //@zyk:planning_base_ = std::unique_ptr<PlanningBase>(new StdPlanning());
        //@zyk:配置文件
        PlanningConfig config_; //@zyk:这个PlanningConfig是从proto生成的数据结构
};

//@zyk:注册组件
/*
namespace {
  struct ProxyType__COUNTER__ {
    ProxyType__COUNTER__() {
      apollo::cyber::class_loader::utility::RegisterClass<PlanningComponent, apollo::cyber::ComponentBase>(
          "PlanningComponent", "apollo::cyber::ComponentBase");
    }
  };
  static ProxyType__COUNTER__ g_register_class___COUNTER__;
}
*/
CYBER_REGISTER_COMPONENT(PlanningComponent)

} // namespace planning
} // namespace apollo
