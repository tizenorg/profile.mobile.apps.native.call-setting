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

#ifndef _CONTROLLER_CONTACT_TEL_NUMBER_PICKER_H_
#define _CONTROLLER_CONTACT_TEL_NUMBER_PICKER_H_

#include "App/Application.h"
#include "Common.h"

#include <app_control.h>

namespace CallSettings { namespace Controller {
	class ContactTelNumberPicker : private util::NonCopyable {
	public:
		typedef enum {
			RES_SUCCESS = 0,
			RES_LAUNCH_ERROR,
			RES_LAUNCH_CANCELED,
			RES_APP_TERMINATED,
			RES_APP_RESPONCE_ERROR,
			RES_INVALID_STATE_ERROR,
			RES_CONTACTS_DB_ERROR,
		} ResultCode;

		typedef util::Delegate<void(ResultCode, std::string)> ResultEventHandler;

	public:
		ContactTelNumberPicker(Application &app);
		~ContactTelNumberPicker();

		ResultCode launch(ResultEventHandler resultHandler);
		void terminate();

	private:

		static app_control_h createAppControl();
		static bool initAppControl(app_control_h appControl);

		ResultCode sendLunchRequest();

		void reset(bool terminateApp = false);
		void resetWithResult(ResultCode result, std::string telNumber);

		static ResultCode parseContactsReply(app_control_h reply, std::string &telNumber);
		static ResultCode getTelNumberId(app_control_h reply, int &telNumberId);
		static ResultCode getTelNumberById(int telNumberId, std::string &telNumber);

		void onSystemEvent(appfw::SystemEvent event);

	private:
		Application &m_app;
		app_control_h m_pAppControl;
		ResultEventHandler m_resultHandler;
	};
} }

#endif /* _CONTROLLER_CONTACT_TEL_NUMBER_PICKER_H_ */
