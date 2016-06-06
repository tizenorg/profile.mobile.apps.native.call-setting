/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 *
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include "App/AppConfig.h"

#include <functional>
#include <algorithm>
#include <utility>
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <memory>

#include <app_i18n.h>
#include <libintl.h>
#include <stdlib.h>
#include <string.h>

#include "util/Logger.h"
#include "util/Helpers.h"
#include "util/TString.h"
#include "util/Delegation.h"
#include "util/NonCopyable.h"

typedef util::Delegate<void()> NotiHandler;

#endif /* _COMMON_H_ */
