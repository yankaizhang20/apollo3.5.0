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

#include "cyber/init.h"

#include <libgen.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <csignal>
#include <string>

#include "cyber/binary.h"
#include "cyber/common/environment.h"
#include "cyber/common/file.h"
#include "cyber/common/global_data.h"
#include "cyber/data/data_dispatcher.h"
#include "cyber/event/perf_event_cache.h"
#include "cyber/logger/async_logger.h"
#include "cyber/scheduler/scheduler.h"
#include "cyber/service_discovery/topology_manager.h"
#include "cyber/task/task.h"
#include "cyber/timer/timer_manager.h"
#include "cyber/transport/transport.h"

namespace apollo {
namespace cyber {

using apollo::cyber::common::EnsureDirectory;
using apollo::cyber::common::GetAbsolutePath;
using apollo::cyber::common::GetProtoFromFile;
using apollo::cyber::common::WorkRoot;
using apollo::cyber::croutine::CRoutine;
using apollo::cyber::event::PerfEventCache;
using apollo::cyber::scheduler::Scheduler;
using apollo::cyber::service_discovery::TopologyManager;

namespace {
bool g_atexit_registered = false;
std::mutex g_mutex;
logger::AsyncLogger* async_logger = nullptr;
} // namespace

namespace {
//@zyk:InitLogger("mainboard")
void InitLogger(const char* binary_name) {
        const char* slash = strrchr(binary_name, '/');   //@zyk:strrchr????????????????????????????????????????????????
        if (slash) {
                ::apollo::cyber::Binary::SetName(slash + 1);
        } else {
                ::apollo::cyber::Binary::SetName(binary_name);
        }
        //glog??????
        CHECK_NOTNULL(common::GlobalData::Instance());

        // Init glog
        google::InitGoogleLogging(binary_name);
        google::SetLogDestination(google::ERROR, "");
        google::SetLogDestination(google::WARNING, "");
        google::SetLogDestination(google::FATAL, "");

        // Init async logger
        async_logger =
                new ::apollo::cyber::logger::AsyncLogger(google::base::GetLogger(FLAGS_minloglevel), 2 * 1024 * 1024);
        google::base::SetLogger(FLAGS_minloglevel, async_logger);
        async_logger->Start();
}

void StopLogger() {
        if (async_logger != nullptr) {
                async_logger->Stop();
        }
}
} // namespace

void OnShutdown(int sig) {
        (void)sig;
        if (GetState() != STATE_SHUTDOWN) {
                SetState(STATE_SHUTTING_DOWN);
        }
}

void ExitHandle() { Clear(); }

//@zyk:Init("mainboard")
bool Init(const char* binary_name) {
        std::lock_guard<std::mutex> lg(g_mutex);
        if (GetState() != STATE_UNINITIALIZED) {
                return false;
        }
        //@zyk:InitLogger("mainboard")
        InitLogger(binary_name)
        //@zyk:???????????????????????????State???STATE_SHUTTING_DOWN
        std::signal(SIGINT,OnShutdown);
        // Register exit handlers
        if (!g_atexit_registered) {
                if (std::atexit(ExitHandle) != 0) {
                        AERROR << "Register exit handle failed";
                        return false;
                }
                AINFO << "Register exit handle succ.";
                g_atexit_registered = true;
        }
        SetState(STATE_INITIALIZED);
        return true;
}

void Clear() {
        std::lock_guard<std::mutex> lg(g_mutex);
        if (GetState() == STATE_SHUTDOWN || GetState() == STATE_UNINITIALIZED) {
                return;
        }
        TaskManager::CleanUp();
        TimerManager::CleanUp();
        scheduler::CleanUp();
        service_discovery::TopologyManager::CleanUp();
        transport::Transport::CleanUp();
        StopLogger();
        SetState(STATE_SHUTDOWN);
}

} // namespace cyber
} // namespace apollo
