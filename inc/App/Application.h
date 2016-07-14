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

#ifndef _CST_APPLICATION_H_
#define _CST_APPLICATION_H_

#include "appfw/BaseApplication.h"
#include "gui/ViewController.h"

#include "Model/Settings/ISettingsManager.h"
#include "Model/Telephony/ITelephonyManager.h"

namespace CallSettings {

	/*
	 * @brief Represents Call Settings application core instance
	 */
	class Application : public appfw::BaseApplication {
	public:
		Application();
		virtual ~Application();

		/*
		 * @return Settings manager instance
		 */
		Model::ISettingsManager &getSettingsManager();

		/*
		 * @return Telephony manager instance
		 */
		Model::ITelephonyManager &getTelephonyManager();

	private:

		/*
		 * @brief Called when application creates
		 * @return true on success, otherwise false. If false returned applciation will be terminated
		 */
		virtual bool onAppCreate() override;

		/*
		 * @brief Called when application terminates. Resource release must be done here.
		 */
		virtual void onAppTerminate() override;

		/*
		 * @brief Called when root view controller request destroy him.
		 * @note At this point application may terminate or create another root controller
		 */
		void onDestroyRequest();

	private:
		Model::ISettingsManager *m_pSettingsManager;
		Model::ITelephonyManager *m_pTelephonyManager;
		gui::ViewController *m_pMainViewController;
	};
}

#endif /* _CST_APPLICATION_H_ */
