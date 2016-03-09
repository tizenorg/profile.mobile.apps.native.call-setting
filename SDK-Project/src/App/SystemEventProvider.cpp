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

#include "App/SystemEventProvider.h"
#include "App/SystemEventListener.h"
#include "Utils/Logger.h"

namespace App {

	bool SystemEventProvider::addEventListener(SystemEvenListener *listener)
	{
		RETVM_IF(!listener, false, "Invalid argument");

		auto result = m_ListenersSet.insert(listener);
		return result.second;
	}

	void SystemEventProvider::removeEventListener(SystemEvenListener *listener)
	{
		RETM_IF(!listener, "Invalid argument");
		m_ListenersSet.erase(listener);
	}

	void SystemEventProvider::onLanguageChanged()
	{
		for(auto listener : m_ListenersSet) {
			listener->onUpdate(CST_APP_LANGUAGE_CHANGED);
		}

	}

	void SystemEventProvider::onOrientationChanged()
	{
		for(auto listener : m_ListenersSet) {
			listener->onUpdate(CST_APP_ORIENTATION_CHANGED);
		}
	}

	void SystemEventProvider::onRegionFormatChanged()
	{
		for(auto listener : m_ListenersSet) {
			listener->onUpdate(CST_APP_REGION_FMT_CHANGED);
		}

	}
	void SystemEventProvider::onAppPause()
	{
		for(auto listener : m_ListenersSet) {
			listener->onPause();
		}
	}

	void SystemEventProvider::onAppResume()
	{
		for(auto listener : m_ListenersSet) {
			listener->onResume();
		}
	}
}
