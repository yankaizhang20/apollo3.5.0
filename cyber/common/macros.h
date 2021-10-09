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

#ifndef CYBER_COMMON_MACROS_H_
#define CYBER_COMMON_MACROS_H_

#include <iostream>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>

#include "cyber/base/macros.h"

DEFINE_TYPE_TRAIT(HasShutdown, Shutdown)

template <typename T>
typename std::enable_if<HasShutdown<T>::value>::type CallShutdown(T *instance) {
        instance->Shutdown();
}

template <typename T>
typename std::enable_if<!HasShutdown<T>::value>::type CallShutdown(T *instance) {
        (void)instance;
}

// There must be many copy-paste versions of these macros which are same
// things, undefine them to avoid conflict.
#undef UNUSED
#undef DISALLOW_COPY_AND_ASSIGN

#define UNUSED(param) (void)param

#define DISALLOW_COPY_AND_ASSIGN(classname)    \
        classname(const classname &) = delete; \
        classname &operator=(const classname &) = delete;
//@zyk:std::once_flag用于std::call_once
//@zyk:std::call_once用于在多线程时保证函数只被调用一次
//@zyk:std::nothrow用于使new失败时返回空指针
/*@zyk:
单例模式注意
1.构造函数，拷贝构造函数，赋值号，析构函数private
2.有一个公有接口用于创建或者获得该类的实例，由于无法通过构造函数显示创建实例，所以该类应当是静态的
3.实例要一直存在，所以实例的指针应当是静态的，以便下次调用接口时可以访问到该实例
4.多线程安全，用std::call_once
5.记得写清理函数
*/
#define DECLARE_SINGLETON(classname)                                                              \
    public:                                                                                       \
        static classname *Instance(bool create_if_needed = true) {                                \
                static classname *instance = nullptr;                                             \
                if (!instance && create_if_needed) {                                              \
                        static std::once_flag flag;                                               \
                        std::call_once(flag, [&] { instance = new (std::nothrow) classname(); }); \
                }                                                                                 \
                return instance;                                                                  \
        }                                                                                         \
                                                                                                  \
        static void CleanUp() {                                                                   \
                auto instance = Instance(false);                                                  \
                if (instance != nullptr) {                                                        \
                        CallShutdown(instance);                                                   \
                }                                                                                 \
        }                                                                                         \
                                                                                                  \
    private:                                                                                      \
        classname();                                                                              \
        DISALLOW_COPY_AND_ASSIGN(classname)

#endif // CYBER_COMMON_MACROS_H_
