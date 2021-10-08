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

#include "cyber/mainboard/module_argument.h"

#include <getopt.h>
#include <libgen.h>

using apollo::cyber::common::GlobalData;

namespace apollo {
namespace cyber {
namespace mainboard {

ModuleArgument::ModuleArgument() {}

ModuleArgument::~ModuleArgument() {}

void ModuleArgument::DisplayUsage() {
        AINFO << "Usage: \n    " << binary_name_ << " [OPTION]...\n"
              << "Description: \n"
              << "    -h, --help : help infomation \n"
              << "    -d, --dag_conf=CONFIG_FILE : module dag config file\n"
              << "    -p, --process_group=process_group: the process "
                 "namespace for running this module, default in manager process\n"
              << "    -s, --sched_name=sched_name: sched policy "
                 "conf for hole process, sched_name should be conf in cyber.pb.conf\n"
              << "Example:\n"
              << "    " << binary_name_ << " -h\n"
              << "    " << binary_name_ << " -d dag_conf_file1 -d dag_conf_file2 "
              << "-p process_group -s sched_name\n";
}
/*
*TODO:为什么要对传值参数const
*char* const argv[]  读类型的时候从右向左读，const修饰左边，除非const在最左边 (array of) (const) (pointer to char)
*/
void ModuleArgument::ParseArgument(const int argc, char* const argv[]) {
        binary_name_ = std::string(basename(argv[0]));   //@zyk:char* basename(char* path) 获取路径中的文件名
        GetOptions(argc, argv);

        if (process_group_.empty()) {
                process_group_ = DEFAULT_process_group_;
        }

        if (sched_name_.empty()) {
                sched_name_ = DEFAULT_sched_name_;
        }

        GlobalData::Instance()->SetProcessGroup(process_group_);
        GlobalData::Instance()->SetSchedName(sched_name_);
        AINFO << "binary_name_ is " << binary_name_ << ", process_group_ is " << process_group_ << ", has "
              << dag_conf_list_.size() << " dag conf";
        for (std::string& dag : dag_conf_list_) {
                AINFO << "dag_conf: " << dag;
        }
}

void ModuleArgument::GetOptions(const int argc, char* const argv[]) {
        opterr = 0; // extern int opterr
        int long_index = 0;
        const std::string short_opts = "hd:p:s:";
        /*在c程序应用程序执行的时，常常配有一些参数，如果参数少我们可以使用arvg，arvc来实现，
          如果参数很多并且需要传入参数复杂我们可以使用strcut option 来实现
          struct option {
                const char *name; //name表示的是长参数名
                int has_arg；
                //has_arg有3个值，no_argument(或者是0)，表示该参数后面不跟参数值
                // required_argument(或者是1),表示该参数后面一定要跟个参数值
                // optional_argument(或者是2),表示该参数后面可以跟，也可以不跟参数值
                int *flag;    
                //用来决定，getopt_long()的返回值到底是什么。如果这个指针为NULL，那么getopt_long()返回
                该结构val字段中的数值。如果该指针不为NULL，getopt_long()会使得它所指向的变量中填入val字段
                中的数值，并且getopt_long()返回0。如果flag不是NULL，但未发现长选项，那么它所指向的变量的数值不变。
                int val;    
                //和flag联合决定返回值 这个值是发现了长选项时的返回值，或者flag不是 NULL时载入*flag中的值。典型情况下，若flag不是NULL，那么val是个真／假值，譬如1 或0；另一方面，如 果flag是NULL，那么val通常是字符常量，若长选项与短选项一致，那么该字符常量应该与optstring中出现的这个选项的参数相同。
            };
        关于使用option解析参数的方式参考https://www.cnblogs.com/hnrainll/archive/2011/09/15/2176933.html
        */
        static const struct option long_opts[] = {{"help", no_argument, nullptr, 'h'},
                                                  {"dag_conf", required_argument, nullptr, 'd'},
                                                  {"process_name", required_argument, nullptr, 'p'},
                                                  {"sched_name", required_argument, nullptr, 's'},
                                                  {NULL, no_argument, nullptr, 0}};

        // log command for info
        std::string cmd("");
        for (int i = 0; i < argc; ++i) {
                cmd += argv[i];
                cmd += " ";
        }
        AINFO << "command: " << cmd;

        do {
                int opt = getopt_long(argc, argv, short_opts.c_str(), long_opts, &long_index);
                if (opt == -1) {
                        break;
                }
                switch (opt) {
                        case 'd':
                                dag_conf_list_.emplace_back(std::string(optarg));    //@zyk:全局变量，当处理一个带参数的选项时，全局变量optarg会指向它的参数
                                for (int i = optind; i < argc; i++) {    //@zyk:optind是getopt.h里定义的全局变量，是argv中将要被处理的下一个元素的下标
                                        if (*argv[i] != '-') {
                                                dag_conf_list_.emplace_back(std::string(argv[i]));
                                        } else {
                                                break;
                                        }
                                }
                                break;
                        case 'p':
                                process_group_ = std::string(optarg);
                                break;
                        case 's':
                                sched_name_ = std::string(optarg);
                                break;
                        case 'h':
                                DisplayUsage();
                                exit(0);
                        default:
                                break;
                }
        } while (true);
}

} // namespace mainboard
} // namespace cyber
} // namespace apollo
