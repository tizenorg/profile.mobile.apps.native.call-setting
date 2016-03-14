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

#ifndef APPCORE_H_
#define APPCORE_H_

#include "App/SystemEventManager.h"
#include "View/ViewManager/ViewManager.h"
#include "Model/Settings/SettingsManager.h"

namespace App {
	using namespace Model;
	using namespace View;
	class AppCore : private NonCopyable {
	public:
		static AppCore *initialize();
		static void finalize(AppCore *core);

		SystemEventManager &getSystemEventManager();
		Settings::SettingsManager &getSettingsManager();
		ViewManager &getViewManager();

	private:
		AppCore();
		~AppCore();
		SystemEventManager *m_pEventManager;
		Settings::SettingsManager *m_pSettingsManager;
		View::ViewManager *m_pViewManager;
	};
}
#endif /* APPCORE_H_ */
