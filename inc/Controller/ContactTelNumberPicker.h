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
	/*
	 * @brief Provides functionality for pick phone number from contacts application
	 */
	class ContactTelNumberPicker : private util::NonCopyable {
	public:
		/*
		 * @brief Result types
		 */
		typedef enum {
			RES_SUCCESS = 0,
			RES_LAUNCH_ERROR,
			RES_LAUNCH_CANCELED,
			RES_APP_TERMINATED,
			RES_APP_RESPONCE_ERROR,
			RES_INVALID_STATE_ERROR,
			RES_CONTACTS_DB_ERROR,
		} ResultCode;

		/*
		 * @brief Contact pick result handler
		 * @param[in]	ResultCode		Result code for contact pick operation
		 * @param[in]	string			Picked phone number
		 */
		typedef util::Delegate<void(ResultCode, std::string)> ResultEventHandler;

	public:
		ContactTelNumberPicker(Application &app);
		~ContactTelNumberPicker();

		/*
		 * @brief Launch contacts application in phone number pick mode
		 * @param[in]	resultHandler	operation result handler
		 */
		ResultCode launch(ResultEventHandler resultHandler);

		/*
		 * @brief Terminates launched contacts application
		 */
		void terminate();

	private:

		/*
		 * @brief Create app control handler instance
		 * @return app control handler on success, otherwise nullptr
		 */
		static app_control_h createAppControl();

		/*
		 * @brief Setup app control parameters in order to perform launch contacts phone picked view
		 * @param[in]	appControl	app control handler
		 * @return true on success, otherwise false
		 */
		static bool initAppControl(app_control_h appControl);

		/*
		 * @brief Launch app control request
		 * @return launch result code
		 */
		ResultCode sendLunchRequest();

		/*
		 * @brief Release app_control_h underlying object and provide terminate of launched app
		 * @param[in]	terminateApp	If true launched app will be terminated, otherwise not
		 */
		void reset(bool terminateApp = false);
		/*
		 * @brief Release app_control_h underlying object and provide pick operation result to caller
		 * @param[in]	result			operation result
		 * @param[in]	telNumber		telephone number
		 */
		void resetWithResult(ResultCode result, std::string telNumber);

		/*
		 * @brief Get telephone number from Contacts application reply data
		 * @param[in]	reply		app control reply handler
		 * @param[out]	telNumber	picked telephone number
		 * @return		result code of operation
		 */
		static ResultCode parseContactsReply(app_control_h reply, std::string &telNumber);

		/*
		 * @brief Get Contacts_db telephone id from Contacts application reply data
		 * @param[in]	reply			app control reply handler
		 * @param[out]	telNumberId		picked telephone number id in contacts data base
		 * @return		result code of operation
		 */
		static ResultCode getTelNumberId(app_control_h reply, int &telNumberId);

		/*
		 * @brief Get telephone number string by telephone number contacts data base id
		 * @param[in]	telNumberId			telephone number contacts data base id
		 * @param[out]	telNumber			telephone number
		 * @return		result code of operation
		 */
		static ResultCode getTelNumberById(int telNumberId, std::string &telNumber);

		/*
		 * @brief SystemEvent callback
		 */
		void onSystemEvent(appfw::SystemEvent event);

	private:
		Application &m_app;
		app_control_h m_pAppControl;
		ResultEventHandler m_resultHandler;
	};
} }

#endif /* _CONTROLLER_CONTACT_TEL_NUMBER_PICKER_H_ */
