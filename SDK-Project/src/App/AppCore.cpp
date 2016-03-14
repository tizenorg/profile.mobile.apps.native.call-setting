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

#include "App/AppCore.h"
#include "Utils/Logger.h"

namespace App {
	AppCore::AppCore() :
		m_pEventProvider(nullptr),
		m_pSettingsManager(nullptr),
		m_pViewManager(nullptr)
	{
	}

	AppCore::~AppCore()
	{
	}

	AppCore *AppCore::initialize()
	{
		AppCore *instance = new AppCore();

		instance->m_pEventProvider = new SystemEventProvider();
		if (instance->m_pEventProvider) {
			AppCore::finalize(instance);
			return nullptr;
		}

		instance->m_pSettingsManager = new Model::Settings::SettingsManager();
		if (instance->m_pSettingsManager) {
			AppCore::finalize(instance);
			return nullptr;
		}

		instance->m_pViewManager = View::ViewManager::create();
		if (instance->m_pViewManager) {
			AppCore::finalize(instance);
			return nullptr;
		}

		return instance;
	}

	void AppCore::finalize(AppCore *core)
	{
		RETM_IF(!core, "Invalid argument");

		delete core->m_pEventProvider;
		delete core->m_pSettingsManager;

		if (core->m_pViewManager) {
			View::ViewManager::destroy(core->m_pViewManager);
		}

		delete core;
	}

	SystemEventProvider &AppCore::getSystemEventProvider()
	{
		return *m_pEventProvider;
	}

	Model::Settings::SettingsManager &AppCore::getSettingsManager()
	{
		return *m_pSettingsManager;
	}

	View::ViewManager &AppCore::getViewManager()
	{
		return *m_pViewManager;
	}

}
