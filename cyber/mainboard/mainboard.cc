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

#include "cyber/common/global_data.h"
#include "cyber/common/log.h"
#include "cyber/init.h"
#include "cyber/mainboard/module_argument.h"
#include "cyber/mainboard/module_controller.h"
#include "cyber/state.h"
#include "gflags/gflags.h"   //@zyk:处理命令行参数

using apollo::cyber::mainboard::ModuleArgument;
using apollo::cyber::mainboard::ModuleController;

//@zyk: nohup mainboard -p compute_sched -d /apollo/modules/planning/dag/planning.dag &
int main(int argc, char** argv) {
        //@zyk:用于在命令行中加--help中时，显示帮助信息
        google::SetUsageMessage("we use this program to load dag and run user apps.");

        // parse the argument
        ModuleArgument module_args;
        module_args.ParseArgument(argc, argv);

        // initialize cyber
        //@zyk:apollo::cyber::Init("mainboard")
        //@zyk:设置程序状态，初始化日志glog,定义程序中断和退出时的行为
        apollo::cyber::Init(argv[0]);

        // start module
        ModuleController controller(module_args);
        if (!controller.Init()) {
                controller.Clear();
                AERROR << "module start error.";
                return -1;
        }

        apollo::cyber::WaitForShutdown();
        controller.Clear();
        AINFO << "exit mainboard.";

        return 0;
}
