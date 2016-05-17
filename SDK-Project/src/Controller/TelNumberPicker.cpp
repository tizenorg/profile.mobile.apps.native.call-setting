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

#include "Model/Telephony/TelephonyTypes.h"
#include "Controller/TelNumberPicker.h"

#include <contacts.h>

namespace CallSettings { namespace Controller {

	namespace {

		const char* mimeContact = "application/vnd.tizen.contact";
		const char* DATA_RESULT_TYPE  = "phone";

		bool isValidTelNumChar (char x)
		{
			auto it = Model::TELEPHONY_NUMBER_ALLOWED_SYMBOLS.find(x);

			if (it != std::string::npos) {
				return false;
			} else {
				return true;
			}
		}

		void telNumberNormalaizer(std::string &telNumber)
		{
			telNumber.erase(std::remove_if(telNumber.begin(), telNumber.end(), isValidTelNumChar), telNumber.end());
		}
	}

	TelNumberPicker::TelNumberPicker():
		m_pAppControl(nullptr),
		m_isAppLaunched(false)
	{
	}

	TelNumberPicker::~TelNumberPicker()
	{
		if (m_isAppLaunched) {
			terminate();
		}

		app_control_destroy(m_pAppControl);
	}

	TelNumberPicker *TelNumberPicker::create()
	{
		TelNumberPicker *instance = new TelNumberPicker();

		if (!instance->initialize()) {
			ERR("Failed to create TelNumberPicker instance!");
			delete instance;
			return nullptr;
		}

		return instance;
	}

	bool TelNumberPicker::initialize()
	{

		int res = app_control_create(&m_pAppControl);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to create app_control, result %d", res);

		res = app_control_set_operation(m_pAppControl, APP_CONTROL_OPERATION_PICK);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to set operation, result %d", res);

		res = app_control_set_mime(m_pAppControl, mimeContact);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to set mime, result %d", res);

		res = app_control_add_extra_data(m_pAppControl, APP_CONTROL_DATA_TYPE, DATA_RESULT_TYPE);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to add extra data, result %d", res);

		res = app_control_set_launch_mode(m_pAppControl, APP_CONTROL_LAUNCH_MODE_SINGLE);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to set launch mode, result %d", res);

		res = app_control_enable_app_started_result_event(m_pAppControl);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to enable start result event, result %d", res);

		return true;
	}

	TelNumberPicker::NumPickerResCode TelNumberPicker::launch(ResultEventHandler resultHandler)
	{
		m_resultHandler = resultHandler;
		return runContactsApp();
	}

	void TelNumberPicker::terminate()
	{
		if (m_isAppLaunched) {
			int res = app_control_send_terminate_request(m_pAppControl);
			if (res != APP_CONTROL_ERROR_NONE) {
				WARN("Failed to send terminate request, result %d", res);
			}
		} else {
			WARN("Contacts app was not launched, skip terminate request");
		}

		m_isAppLaunched = false;
	}

	TelNumberPicker::NumPickerResCode TelNumberPicker::runContactsApp()
	{
		int res = app_control_send_launch_request(m_pAppControl,
			[](app_control_h request, app_control_h reply, app_control_result_e result, void *data) {

				TelNumberPicker *instance = static_cast<TelNumberPicker *>(data);
				switch(result) {
				case APP_CONTROL_RESULT_APP_STARTED:
					instance->m_isAppLaunched = true;
					break;
				case APP_CONTROL_RESULT_SUCCEEDED:
					instance->m_isAppLaunched = false;
					instance->sendReplyToCaller(reply);
					break;
				default:
					DBG("Failed to launch contacts, error: %d", result);
					instance->m_resultHandler(NUM_PICKER_RES_FAIL_LAUNCH_ERR, "");
				}
			}, this);

		if (res != APP_CONTROL_ERROR_NONE) {
			ERR("Failed to launch app, result %d", res);
			return NUM_PICKER_RES_FAIL_LAUNCH_ERR;
		}

		return NUM_PICKER_RES_SUCCESS;
	}

	void TelNumberPicker::sendReplyToCaller(app_control_h reply)
	{
		NumPickerResCode res = NUM_PICKER_RES_SUCCESS;
		int telNumberId = -1;
		std::string telNumber = "";

		res = parseContactsReply(reply, telNumberId);
		if (res == NUM_PICKER_RES_SUCCESS) {
			res = getTelNumberById(telNumberId, telNumber);
		}

		m_resultHandler(res, telNumber);
	}

	TelNumberPicker::NumPickerResCode TelNumberPicker::parseContactsReply(app_control_h reply, int &telNumberId)
	{
		char **array = nullptr;
		int count = 0;

		int err = app_control_get_extra_data_array(reply, APP_CONTROL_DATA_SELECTED, &array, &count);
		RETVM_IF(err != APP_CONTROL_ERROR_NONE, NUM_PICKER_RES_FAIL_INVALID_APP_RESPONCE, "Extra data parse error! Code: %d", err);

		if (array && array[0]) {
			telNumberId = atoi(array[0]);
		}

		for (int i = 0; i < count; ++i) {
			free(array[i]);
		}

		free(array);
		return NUM_PICKER_RES_SUCCESS;
	}

	TelNumberPicker::NumPickerResCode TelNumberPicker::getTelNumberById(int telNumberId, std::string &telNumber)
	{
		contacts_record_h dbRecord = nullptr;
		char *rawTelNumber = nullptr;

		int res = contacts_connect();
		RETVM_IF(res != CONTACTS_ERROR_NONE, NUM_PICKER_RES_FAIL_CONTACTS_DB_ERROR, "Failed to connect to contacts_db! Error %d", res);

		res = contacts_db_get_record(_contacts_number._uri, telNumberId, &dbRecord);
		if (res != CONTACTS_ERROR_NONE) {
			ERR("Failed to get contacts_db record! Error %d", res);
			contacts_disconnect();
			return NUM_PICKER_RES_FAIL_CONTACTS_DB_ERROR;
		}

		res = contacts_record_get_str_p(dbRecord, _contacts_number.number, &rawTelNumber);
		if (res != CONTACTS_ERROR_NONE) {
			ERR("Failed to get tel number! Error %d", res);
			contacts_disconnect();
			return NUM_PICKER_RES_FAIL_CONTACTS_DB_ERROR;
		}

		telNumber = rawTelNumber;
		telNumberNormalaizer(telNumber);
		DBG("Selected telNumber %s", telNumber.c_str());

		contacts_disconnect();
		return NUM_PICKER_RES_SUCCESS;
	}

} }
