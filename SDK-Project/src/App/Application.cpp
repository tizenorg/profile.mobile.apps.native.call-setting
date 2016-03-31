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

#include <Elementary.h>
#include "App/AppConfig.h"
#include "App/Application.h"
#include "Utils/Common.h"
#include "Controller/MainView/MainViewController.h"

namespace App {

	Application::Application()
		:m_pAppCore(nullptr),
		m_pMainViewController(nullptr)
	{
	}

	Application::~Application()
	{
	}

	int Application::appRun(int argc, char* argv[])
	{
		ui_app_lifecycle_callback_s cbs;
		cbs.create = [](void *user_data)->bool {
			return static_cast<Application*>(user_data)->onAppCreate();
		};

		cbs.terminate = [](void *user_data) {
			return static_cast<Application*>(user_data)->onAppTerminate();
		};

		cbs.pause = [](void *user_data) {
			return static_cast<Application*>(user_data)->onAppPause();
		};

		cbs.resume = [](void *user_data) {
			return static_cast<Application*>(user_data)->onAppResume();
		};

		cbs.app_control = [](app_control_h app_control, void *user_data) {
			return static_cast<Application*>(user_data)->onAppControl(app_control);
		};

		return ui_app_main(argc, argv, &cbs, this);
	}

	void Application::appTerminate()
	{
		ui_app_exit();
	}

	bool Application::onAppCreate()
	{
		m_pAppCore = AppCore::initialize();
		RETVM_IF(!m_pAppCore, false, "Failed to init app core");

		bindtextdomain(TEXT_DOMAIN, Utils::getLocaleDir().c_str());
		textdomain(TEXT_DOMAIN);

		elm_app_base_scale_set(UI_BASE_SCALE);
		elm_config_preferred_engine_set("opengl_x11");

		m_pMainViewController = ViewController::create<MainController::MainViewController>(*m_pAppCore,
				NotiHandler::wrap<Application, &Application::onDestroyRequest>(this));

		if (m_pMainViewController) {
			return true;
		}

		return false;
	}

	void Application::onAppTerminate()
	{
		DBG("Application Terminate");
		if (m_pAppCore) {
			AppCore::finalize(m_pAppCore);
			m_pAppCore = nullptr;
		}
	}

	void Application::onAppPause()
	{
		DBG("Application Pause");
		m_pAppCore->getSystemEventManager().dispatchPauseEvent();

	}

	void Application::onAppResume()
	{
		DBG("Application Resume");
		m_pAppCore->getSystemEventManager().dispatchResumeEvent();
	}

	void Application::onAppControl(app_control_h request)
	{
		INF("onAppControl callback ++");
	}

	void Application::onDestroyRequest()
	{
		appTerminate();
	}
}
