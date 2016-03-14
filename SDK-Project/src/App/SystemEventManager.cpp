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

#include "App/SystemEventManager.h"

namespace App {

	SystemEventManager::SystemEventManager() :
		m_pLanguageChange_h(nullptr),
		m_pRegionFmtChange_h(nullptr),
		m_SystemEventHandlers()
	{
		initialize();
	}

	SystemEventManager::~SystemEventManager()
	{
		finalize();
	}

	void SystemEventManager::initialize()
	{
		ui_app_add_event_handler(&m_pLanguageChange_h, APP_EVENT_LANGUAGE_CHANGED,
			makeAppEventCb(SystemEventManager, &SystemEventManager::onLanguageChanged), this);

		ui_app_add_event_handler(&m_pRegionFmtChange_h, APP_EVENT_REGION_FORMAT_CHANGED,
			makeAppEventCb(SystemEventManager, &SystemEventManager::onRegionFormatChanged), this);
	}

	void SystemEventManager::finalize()
	{
		ui_app_remove_event_handler(m_pLanguageChange_h);
		ui_app_remove_event_handler(m_pRegionFmtChange_h);
	}

	void SystemEventManager::dispatchPauseEvent()
	{
		m_SystemEventHandlers.invoke(SYS_EVENT_PAUSE);
	}

	void SystemEventManager::dispatchResumeEvent()
	{
		m_SystemEventHandlers.invoke(SYS_EVENT_RESUME);
	}

	void SystemEventManager::addSystemEventHandler(SystemEventHandler handler)
	{
		m_SystemEventHandlers += handler;
	}

	void SystemEventManager::removeSystemEventHandler(SystemEventHandler handler)
	{
		m_SystemEventHandlers -= handler;
	}

	void SystemEventManager::onLanguageChanged(app_event_info_h event)
	{
		INF("System Language changes");
		char *lang = nullptr;
		app_event_get_language(event, &lang);
		if (lang) {
			elm_language_set(lang);
			free(lang);
		}

		m_SystemEventHandlers.invoke(SYS_EVENT_LANGUAGE_CHANGE);
	}

	void SystemEventManager::onRegionFormatChanged(app_event_info_h event)
	{
		INF("System Region format changes");
		m_SystemEventHandlers.invoke(SYS_EVENT_REGION_FMT_CHANGE);
	}

}
