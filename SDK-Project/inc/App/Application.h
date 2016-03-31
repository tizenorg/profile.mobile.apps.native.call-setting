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

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "App/BaseApplication.h"

#include "Model/Settings/SettingsManager.h"
#include "Model/Telephony/TelephonyManager.h"
#include "Controller/ViewController.h"

namespace App {

	class Application : public BaseApplication {
	public:
		Application();
		virtual ~Application();
		Model::Settings::SettingsManager &getSettingsManager();

	private:
		virtual bool onAppCreate() override;
		virtual void onAppTerminate() override;
		void onDestroyRequest();

	private:
		Model::Settings::SettingsManager *m_pSettingsManager;
		Model::Telephony::TelephonyManager *m_pTelephonyManager;
		Controller::ViewController *m_pMainViewController;
	};
}

#endif /* APPLICATION_H_ */
