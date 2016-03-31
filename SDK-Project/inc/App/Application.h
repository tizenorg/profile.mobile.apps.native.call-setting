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

#include <app.h>
#include "App/AppCore.h"
#include "Controller/ViewController.h"

namespace App {

	class Application : private NonCopyable {
	public:
		Application();
		~Application();
		int appRun(int argc, char *argv[]);
		void appTerminate();

	private:
		bool onAppCreate();
		void onAppTerminate();
		void onAppPause();
		void onAppResume();
		void onAppControl(app_control_h request);
		void onDestroyRequest();

	private:
		AppCore *m_pAppCore;
		Controller::ViewController *m_pMainViewController;

	};
}

#endif /* APPLICATION_H_ */
