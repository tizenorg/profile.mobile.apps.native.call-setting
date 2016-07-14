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

#ifndef _APPFW_APPTYPES_H_
#define _APPFW_APPTYPES_H_

#include "Common.h"
#include <app.h>

namespace appfw
{
	/**
	 * @brief Basic System event types.
	 */
	typedef enum {
		SYS_EVENT_LANGUAGE_CHANGE,
		SYS_EVENT_REGION_FMT_CHANGE,
		SYS_EVENT_PAUSE,
		SYS_EVENT_RESUME,
	} SystemEvent;

	/**
	 * @brief System Event handler type which invokes in case of system event
	 */
	typedef util::Delegate<void(SystemEvent)> SystemEventHandler;
}

#endif /* _APPFW_APPTYPES_H_ */
