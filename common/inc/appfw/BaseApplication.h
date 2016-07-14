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

#ifndef _APPFW_BASE_APPLICATION_H_
#define _APPFW_BASE_APPLICATION_H_

#include "gui/ViewManager.h"

#include "AppTypes.h"
#include "SystemEventManager.h"
#include "Path.h"

namespace appfw {
	/**
	 *@brief Application life cycle controller
	 */
	class BaseApplication : private util::NonCopyable {
	public:
		BaseApplication();
		virtual ~BaseApplication();

		/**
		 * @brief Run application main loop.
		 * @param[in]	argc	argc parameter received in main()
		 * @param[in]	argv	argv parameter received in main()
		 */
		int run(int argc, char *argv[]);

		/**
		 * @brief Terminate application.
		 */
		void terminate();

		/**
		 * @brief Provide System event manager instance
		 */
		SystemEventManager &getSystemEventManager();

		/**
		 * @brief Provide View manager instance
		 */
		gui::ViewManager &getViewManager();

	protected:
		/**
		 * @brief Called when application is created
		 * @return true on success, otherwise false
		 */
		virtual bool onAppCreate() = 0;

		/**
		 * @brief Called when application is terminated
		 */
		virtual void onAppTerminate() = 0;

		/**
		 * @brief Called when application receives App Control request
		 * @param[in]	request		App Control request
		 */
		virtual void onAppControl(app_control_h request) {}

	private:
		/**
		 * @brief Callback for application create event
		 */
		bool AppCreateCb();

		/**
		 * @brief Callback for application terminate event
		 */
		void AppTerminateCb();

		/**
		 * @brief Callback for application pause event
		 */
		void AppPauseCb();

		/**
		 * @brief Callback for application resume event
		 */
		void AppResumeCb();

		/**
		 * @brief Callback for application app control request event
		 */
		void AppControlCb(app_control_h request);

	private:
		/*
		 * @brief Event manager instance
		 */
		SystemEventManager *m_pEventManager;

		/*
		 * @brief View manager instance
		 */
		gui::ViewManager *m_pViewManager;
	};
}

#endif /* _APPFW_BASE_APPLICATION_H_ */
