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

#ifndef _CONTROLLER_TEL_NUMBER_PICKER_H_
#define _CONTROLLER_TEL_NUMBER_PICKER_H_

#include "Common.h"

#include <app_control.h>

namespace CallSettings { namespace Controller {
	class TelNumberPicker : private util::NonCopyable {
	public:
		typedef enum {
			NUM_PICKER_RES_SUCCESS = 0,
			NUM_PICKER_RES_FAIL_LAUNCH_ERR,
			NUM_PICKER_RES_FAIL_TERMINATE_ERR,
			NUM_PICKER_RES_FAIL_INVALID_APP_RESPONCE,
			NUM_PICKER_RES_FAIL_INVALID_STATE,
			NUM_PICKER_RES_FAIL_CONTACTS_DB_ERROR,
		} NumPickerResCode;

		typedef util::Delegate<void(NumPickerResCode, std::string)> ResultEventHandler;

		static TelNumberPicker *create();
		~TelNumberPicker();

		NumPickerResCode launch(ResultEventHandler resultHandler);
		void terminate();

	private:
		TelNumberPicker();
		bool initialize();

		NumPickerResCode runContactsApp();
		void sendReplyToCaller(app_control_h reply);

		static NumPickerResCode parseContactsReply(app_control_h reply, int &telNumberId);
		static NumPickerResCode getTelNumberById(int telNumberId, std::string &telNumber);

	private:
		app_control_h m_pAppControl;
		bool m_isAppLaunched;
		ResultEventHandler m_resultHandler;
	};
} }

#endif /* _CONTROLLER_TEL_NUMBER_PICKER_H_ */
