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
#include "Utils/Path.h"
#include "Utils/Logger.h"

using namespace App;

Application::Application()
	:m_pAppCore(nullptr)
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
	m_pAppCore = new AppCore();
	RETVM_IF(m_pAppCore, false, "Failed to init app core");

	bindtextdomain(TEXT_DOMAIN, Utils::getLocaleDir().c_str());
	textdomain(TEXT_DOMAIN);

	app_event_handler_h handle = nullptr;
	ui_app_add_event_handler(&handle, APP_EVENT_LANGUAGE_CHANGED, [](app_event_info_h event_info, void *data) {
		return static_cast<Application*>(data)->onLanguageChanged(event_info);
	}, this);

	elm_app_base_scale_set(UI_BASE_SCALE);
	elm_config_preferred_engine_set("opengl_x11");

	return true;
}

void Application::onAppTerminate()
{
	delete m_pAppCore;
	m_pAppCore = nullptr;
}

void Application::onAppPause()
{
	INF("onAppPause callback");
}

void Application::onAppResume()
{
	INF("onAppResume callback");
}

void Application::onAppControl(app_control_h request)
{
	INF("onAppControl callback");
}

void Application::onLanguageChanged(app_event_info_h event)
{
	INF("onLanguageChanged callback start");
	char *lang = nullptr;
	app_event_get_language(event, &lang);
	if (lang) {
		elm_language_set(lang);
		free(lang);
	}
}
