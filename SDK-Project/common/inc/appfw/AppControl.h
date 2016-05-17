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

#ifndef _APPFW_APPCONTROL_H_
#define _APPFW_APPCONTROL_H_

#include "Common.h"
#include <app_control.h>

namespace appfw {
	class AppControl : private util::NonCopyable {
	public:
		typedef enum {
			APP_LAUNCH_MODE_SINGLE,
			APP_LAUNCH_MODE_GROUP
		} AppLaunchMode;

		typedef enum {
			APP_LAUNCH_RESULT_APP_STARTED,
			APP_LAUNCH_RESULT_SUCCEEDED,
			APP_LAUNCH_RESULT_FAILED,
			APP_LAUNCH_RESULT_REJECTED
		} AppLaunchResult;

		typedef util::Delegate<void(AppLaunchResult)> LaunchEventHandler;

		~AppControl();

		static AppControl *create(const char *operation = APP_CONTROL_OPERATION_DEFAULT,
				const char *mime = nullptr, const char *uri = nullptr);

		bool setMime(const char *appId);
		bool setURI(const char *appId);
		bool setAppId(const char *appId);

		void setLaunchEventHandler(LaunchEventHandler launchEventHandler);

		bool appLaunch(AppLaunchMode launchMode = APP_LAUNCH_MODE_GROUP);
		void appTerminate();

		bool addExtraData(const char *key, const char *value);
		bool addExtraDataArray(const char *key, const char **array, int length);

		bool getReplyData(const char *key, std::string &replyData);
		bool getReplyDataArray(const char *key, std::vector<std::string> &replyDataArray);

	private:
		AppControl();

		bool initialize(const char *operation, const char *mime = nullptr, const char *uri = nullptr);
		static AppLaunchResult toAppLaunchResult(app_control_result_e result);

	private:
		app_control_h m_appControl;
		app_control_h m_appReply;
		LaunchEventHandler m_launchEventHandler;
	};
}

#endif /* _APPFW_APPCONTROL_H_ */
