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

#ifndef _APPFW_SYSTEM_EVENT_MANAGER_H_
#define _APPFW_SYSTEM_EVENT_MANAGER_H_

#include "AppTypes.h"

namespace appfw {

	class ISystemEventDispatcher {
		public:
			virtual ~ISystemEventDispatcher() {}
			virtual void dispatchPauseEvent() = 0;
			virtual void dispatchResumeEvent() = 0;
	};

	class ISystemEventProvider {
		public:
			virtual ~ISystemEventProvider() {}
			virtual void addSystemEventHandler(SystemEventHandler handler) = 0;
			virtual void removeSystemEventHandler(SystemEventHandler handler) = 0;
	};

	class SystemEventManager :
		public ISystemEventDispatcher,
		public ISystemEventProvider,
		private util::NonCopyable
	{
		public:
			SystemEventManager();
			virtual ~SystemEventManager();
			virtual void dispatchPauseEvent() final;
			virtual void dispatchResumeEvent() final;

			virtual void addSystemEventHandler(SystemEventHandler handler) final;
			virtual void removeSystemEventHandler(SystemEventHandler handler) final;

		private:
			void initialize();
			void finalize();
			void onLanguageChanged(app_event_info_h event);
			void onRegionFormatChanged(app_event_info_h event);

		private:
			app_event_handler_h m_pLanguageChange_h;
			app_event_handler_h m_pRegionFmtChange_h;
			util::Delegation<void(SystemEvent)> m_SystemEventHandlers;
	};
}
#endif /* _APPFW_SYSTEM_EVENT_MANAGER_H_ */
