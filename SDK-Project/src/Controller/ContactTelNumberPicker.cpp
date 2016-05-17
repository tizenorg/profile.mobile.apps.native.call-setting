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
#include "Controller/ContactTelNumberPicker.h"

#include <contacts.h>

namespace CallSettings { namespace Controller {

	namespace {

		const char *MIME_CONTACTS = "application/vnd.tizen.contact";
		const char *DATA_RESULT_TYPE  = "phone";

		void normalizeTelNumber(std::string &telNumber)
		{
			telNumber.erase(std::remove_if(telNumber.begin(), telNumber.end(), [](char x) {
				return (Model::TELEPHONY_NUMBER_ALLOWED_SYMBOLS.find(x) == std::string::npos);
			}), telNumber.end());
		}
	}

	ContactTelNumberPicker::ContactTelNumberPicker(Application &app):
		m_app(app),
		m_pAppControl(nullptr)
	{
		m_app.getSystemEventManager().addSystemEventHandler(
				appfw::SystemEventHandler::wrap<ContactTelNumberPicker, &ContactTelNumberPicker::onSystemEvent>(this));
	}

	ContactTelNumberPicker::~ContactTelNumberPicker()
	{
		if (m_pAppControl) {
			doTerminate();
		}

		m_app.getSystemEventManager().removeSystemEventHandler(
				appfw::SystemEventHandler::wrap<ContactTelNumberPicker, &ContactTelNumberPicker::onSystemEvent>(this));
	}

	bool ContactTelNumberPicker::initialize(ResultEventHandler resultHandler)
	{
		int res = app_control_create(&m_pAppControl);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to create app_control, result %d", res);

		res = app_control_set_operation(m_pAppControl, APP_CONTROL_OPERATION_PICK);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to set operation, result %d", res);

		res = app_control_set_mime(m_pAppControl, MIME_CONTACTS);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to set mime, result %d", res);

		res = app_control_add_extra_data(m_pAppControl, APP_CONTROL_DATA_TYPE, DATA_RESULT_TYPE);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to add extra data, result %d", res);

		res = app_control_set_launch_mode(m_pAppControl, APP_CONTROL_LAUNCH_MODE_SINGLE);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to set launch mode, result %d", res);

		m_resultHandler = resultHandler;
		return true;
	}

	ContactTelNumberPicker::ResultCode ContactTelNumberPicker::launch(ResultEventHandler resultHandler)
	{
		RETVM_IF(m_pAppControl != nullptr, NUM_PICKER_RES_INVALID_STATE_ERROR, "App is already launched!")

		if (initialize(resultHandler)) {
			return doLaunch();
		} else {
			return NUM_PICKER_RES_LAUNCH_ERROR;
		}
	}

	ContactTelNumberPicker::ResultCode ContactTelNumberPicker::terminate()
	{
		return doTerminate();
	}

	ContactTelNumberPicker::ResultCode ContactTelNumberPicker::doTerminate()
	{
		RETVM_IF(!m_pAppControl, NUM_PICKER_RES_INVALID_STATE_ERROR, "App was not launched, skip terminate request");

		auto appControl = m_pAppControl;
		reset();

		int res = app_control_send_terminate_request(appControl);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, NUM_PICKER_RES_TERMINATE_ERROR, "Failed to send terminate request, result %d", res);

		return NUM_PICKER_RES_SUCCESS;
	}

	ContactTelNumberPicker::ResultCode ContactTelNumberPicker::doLaunch()
	{
		auto res = app_control_send_launch_request(m_pAppControl,
			[](app_control_h request, app_control_h reply, app_control_result_e result, void *data) {
				ContactTelNumberPicker *instance = static_cast<ContactTelNumberPicker *>(data);

				std::string telNumber;
				ResultCode replyResult = NUM_PICKER_RES_LAUNCH_ERROR;

				switch(result) {
				case APP_CONTROL_RESULT_SUCCEEDED:
					replyResult = instance->parseContactsReply(reply, telNumber);
					break;
				case APP_CONTROL_RESULT_CANCELED:
					replyResult = NUM_PICKER_RES_LAUNCH_CANCELED;
					break;
				default:
					ERR("Error result received: %d", result);
				}

				instance->sendResultToCaller(replyResult, telNumber);
		}, this);

		if (res != APP_CONTROL_ERROR_NONE) {
			reset();
			return NUM_PICKER_RES_LAUNCH_ERROR;
		}

		return NUM_PICKER_RES_SUCCESS;
	}

	void ContactTelNumberPicker::onSystemEvent(appfw::SystemEvent event)
	{
		if (event == appfw::SYS_EVENT_RESUME && m_pAppControl) {
			sendResultToCaller(NUM_PICKER_RES_LAUNCH_CANCELED, "");
		}
	}

	void ContactTelNumberPicker::sendResultToCaller(ResultCode result, std::string telNumber)
	{
		auto resultHandler = m_resultHandler;
		reset();
		resultHandler(result, telNumber);
	}

	void ContactTelNumberPicker::reset()
	{
		if (m_pAppControl) {
			app_control_destroy(m_pAppControl);
			m_pAppControl = nullptr;
		}

		m_resultHandler = nullptr;
	}

	ContactTelNumberPicker::ResultCode ContactTelNumberPicker::parseContactsReply(app_control_h reply, std::string &telNumber)
	{
		int telNumberId = -1;
		auto res = getTelNumberId(reply, telNumberId);
		if (res != NUM_PICKER_RES_SUCCESS) {
			return res;
		} else {
			return getTelNumberById(telNumberId, telNumber);
		}
	}

	ContactTelNumberPicker::ResultCode ContactTelNumberPicker::getTelNumberId(app_control_h reply, int &telNumberId)
	{
		char **array = nullptr;
		int count = 0;

		int err = app_control_get_extra_data_array(reply, APP_CONTROL_DATA_SELECTED, &array, &count);
		RETVM_IF(err != APP_CONTROL_ERROR_NONE, NUM_PICKER_RES_APP_RESPONCE_ERROR, "Extra data parse error! Code: %d", err);

		RETVM_IF((count < 1 || !array), NUM_PICKER_RES_APP_RESPONCE_ERROR, "Response data is empty!");

		auto resCode = NUM_PICKER_RES_APP_RESPONCE_ERROR;
		if (array[0]) {
			telNumberId = atoi(array[0]);
			resCode = NUM_PICKER_RES_SUCCESS;
		} else {
			ERR("Number ID is null!");
		}

		for (int i = 0; i < count; i++) {
			free(array[i]);
		}

		free(array);

		return resCode;
	}

	ContactTelNumberPicker::ResultCode ContactTelNumberPicker::getTelNumberById(int telNumberId, std::string &telNumber)
	{
		contacts_record_h dbRecord = nullptr;
		char *rawTelNumber = nullptr;

		int res = contacts_connect();
		RETVM_IF(res != CONTACTS_ERROR_NONE, NUM_PICKER_RES_CONTACTS_DB_ERROR, "Failed to connect to contacts_db! Error %d", res);

		res = contacts_db_get_record(_contacts_number._uri, telNumberId, &dbRecord);
		if (res != CONTACTS_ERROR_NONE) {
			ERR("Failed to get contacts_db record! Error %d", res);
			contacts_disconnect();
			return NUM_PICKER_RES_CONTACTS_DB_ERROR;
		}

		res = contacts_record_get_str_p(dbRecord, _contacts_number.number, &rawTelNumber);
		if (res != CONTACTS_ERROR_NONE) {
			ERR("Failed to get tel number! Error %d", res);
			contacts_record_destroy(dbRecord, true);
			contacts_disconnect();
			return NUM_PICKER_RES_CONTACTS_DB_ERROR;
		}

		telNumber = rawTelNumber;
		normalizeTelNumber(telNumber);
		DBG("Selected telNumber %s", telNumber.c_str());

		contacts_disconnect();
		return NUM_PICKER_RES_SUCCESS;
	}

} }
