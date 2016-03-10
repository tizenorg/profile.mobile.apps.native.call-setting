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

#ifndef COMMON_H_
#define COMMON_H_

#include <Elementary.h>

#include "Utils/Delegation.h"
#include "Utils/Path.h"
#include "Utils/Logger.h"

using namespace Utils;

typedef enum {
	SYS_EVENT_LANGUAGE_CHANGE,
	SYS_EVENT_REGION_FMT_CHANGE,
	SYS_EVENT_PAUSE,
	SYS_EVENT_RESUME,
} SystemEvent;

typedef enum {
	VIEW_CHANGE_BEGIN,
	VIEW_CHANGE_END,
	MENU_BUTTON_PRESSED,
	BACK_BUTTON_PRESSED,
	ORIENTATION_CHANGED,
} ViewEvent;

typedef Delegate<void()> NotifyHandler;
typedef Delegate<void(ViewEvent)> ViewEventHandler;
typedef Delegate<void(SystemEvent)> SystemEventHandler;

#define makeHandler(handlerType, classType, classMethod, data) \
		handlerType::wrap<classType, classMethod>(data)

typedef Callback<void(Evas *, Evas_Object *, void *)> EvasSmartCb;

#define makeEvasEventCb(classType, classMethod) \
	Callback<void(Evas *e, Evas_Object *obj, void *event_info)>::template make<classType, classMethod>()

#define makeEvasSmartCb(classType, classMethod) \
	Callback<void(Evas_Object *obj, void *event_info)>::template make<classType, classMethod>()

#define makeAppEventCb(classType, classMethod) \
	CallbackAlt<void(app_event_info_h event_info)>::template make<classType, classMethod>()


#endif /* COMMON_H_ */
