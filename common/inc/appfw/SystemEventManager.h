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

	/*
	 * @brief Interface of system event dispatcher entity
	 */
	class ISystemEventDispatcher {
		public:
			virtual ~ISystemEventDispatcher() {}

			/*
			 * @brief Called when needed to dispatch application pause event to system event manager
			 */
			virtual void dispatchPauseEvent() = 0;

			/*
			 * @brief Called when needed to dispatch application resume event to system event manager
			 */
			virtual void dispatchResumeEvent() = 0;
	};

	/*
	 * @brief Interface of system event provider entity
	 */
	class ISystemEventProvider {
		public:
			virtual ~ISystemEventProvider() {}

			/*
			 * @brief Subscribe for System events
			 * @param[in]	handler		System event handler callback which will be invoked on system events
			 * @see	SystemEvent
			 */
			virtual void addSystemEventHandler(SystemEventHandler handler) = 0;

			/*
			 * @brief Remove subscribe for System events
			 * @param[in]	handler		System event handler callback which will be unsubscribe
			 * @see	addSystemEventHandler()
			 */
			virtual void removeSystemEventHandler(SystemEventHandler handler) = 0;
	};

	/*
	 * @brief Implementation of ISystemEventProvider and ISystemEventDispatcher.
	 */
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
			typedef util::CallbackAlt<void(app_event_info_h event_info)> AppEventCb;

			/*
			 * @brief Initialization of Event manager.
			 * Called on create SystemEventManager entity.
			 * Setup callbacks for system events is provided here
			 */
			void initialize();

			/*
			 * @brief Finalization of Event manager.
			 * Called on destroy SystemEventManager entity.
			 * Unset system callbacks for system events is provided here
			 */
			void finalize();

			/*
			 * @brief Language change callback. Invoked when system language was changed.
			 * param[in]	event	application event handler
			 */
			void onLanguageChanged(app_event_info_h event);

			/*
			 * @brief Region format change callback. Invoked when system format was changed.
			 * Change of system language change may also automatically cause changes of Region format so this callback
			 * very often invokes after language change callback but it may be invoked separately when region settings changed.
			 * param[in]	event	application event handler
			 */
			void onRegionFormatChanged(app_event_info_h event);

		private:
			/*
			 * @brief Handler for language change callback
			 */
			app_event_handler_h m_pLanguageChange_h;

			/*
			 * @brief Handler for region format callback
			 */
			app_event_handler_h m_pRegionFmtChange_h;

			/*
			 * @brief Collection of handlers which are subscribed for events from EventManager.
			 */
			util::Delegation<void(SystemEvent)> m_SystemEventHandlers;
	};
}
#endif /* _APPFW_SYSTEM_EVENT_MANAGER_H_ */
