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

#include "App/AppConfig.h"
#include "App/BaseApplication.h"

#include "Utils/Common.h"

namespace App {

	BaseApplication::BaseApplication() :
		m_pEventManager(nullptr),
		m_pViewManager(nullptr)
	{
	}

	BaseApplication::~BaseApplication()
	{
	}

	int BaseApplication::run(int argc, char* argv[])
	{
		ui_app_lifecycle_callback_s cbs;
		cbs.create = [](void *user_data)->bool {
			return static_cast<BaseApplication*>(user_data)->AppCreateCb();
		};

		cbs.terminate = [](void *user_data) {
			return static_cast<BaseApplication*>(user_data)->AppTerminateCb();
		};

		cbs.pause = [](void *user_data) {
			return static_cast<BaseApplication*>(user_data)->AppPauseCb();
		};

		cbs.resume = [](void *user_data) {
			return static_cast<BaseApplication*>(user_data)->AppResumeCb();
		};

		cbs.app_control = [](app_control_h app_control, void *user_data) {
			return static_cast<BaseApplication*>(user_data)->AppControlCb(app_control);
		};

		return ui_app_main(argc, argv, &cbs, this);
	}

	void BaseApplication::terminate()
	{
		ui_app_exit();
	}

	SystemEventManager &BaseApplication::getSystemEventManager()
	{
		return *m_pEventManager;
	}

	View::ViewManager &BaseApplication::getViewManager()
	{
		return *m_pViewManager;
	}

	bool BaseApplication::AppCreateCb()
	{
		bindtextdomain(TEXT_DOMAIN, Utils::getLocaleDir().c_str());
		textdomain(TEXT_DOMAIN);

		elm_app_base_scale_set(UI_BASE_SCALE);
		elm_config_accel_preference_set("3d");

		m_pEventManager = new SystemEventManager();
		m_pViewManager = View::ViewManager::create();

		RETVM_IF(!m_pViewManager || !m_pEventManager, false, "Failed to create application!");

		return onAppCreate();
	}

	void BaseApplication::AppTerminateCb()
	{
		DBG("Application Terminate");

		onAppTerminate();
		delete m_pEventManager;
		View::ViewManager::destroy(m_pViewManager);
	}

	void BaseApplication::AppPauseCb()
	{
		DBG("Application Pause");

		m_pEventManager->dispatchPauseEvent();
	}

	void BaseApplication::AppResumeCb()
	{
		DBG("Application Resume");

		m_pEventManager->dispatchResumeEvent();
	}

	void BaseApplication::AppControlCb(app_control_h request)
	{
		DBG("onAppControl callback");
		onAppControl(request);
	}

}
