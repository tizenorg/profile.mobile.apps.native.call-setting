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

#include "Controller/MainViewController.h"

#include "App/Application.h"

namespace CallSettings {
	using namespace Model;
	using namespace Controller;

	Application::Application() :
		m_pSettingsManager(nullptr),
		m_pTelephonyManager(nullptr),
		m_pMainViewController(nullptr)
	{
	}

	Application::~Application()
	{
	}

	ISettingsManager &Application::getSettingsManager()
	{
		return *m_pSettingsManager;
	}

	ITelephonyManager &Application::getTelephonyManager()
	{
		return *m_pTelephonyManager;
	}

	bool Application::onAppCreate()
	{
		m_pSettingsManager = new SettingsManager();
		m_pTelephonyManager = new TelephonyManager();
		RETVM_IF(!m_pSettingsManager || !m_pTelephonyManager, false, "Failed to start application");

		m_pMainViewController = gui::ViewController::create<MainViewController>(*this,
				NotiHandler::wrap<Application, &Application::onDestroyRequest>(this));

		if (m_pMainViewController) {
			return true;
		}

		return false;
	}

	void Application::onAppTerminate()
	{
		delete m_pMainViewController;
		delete m_pSettingsManager;
		delete m_pTelephonyManager;
	}

	void Application::onDestroyRequest()
	{
		terminate();
	}
}
