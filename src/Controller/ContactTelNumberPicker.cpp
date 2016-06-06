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

		const char *const MIME_CONTACTS = "application/vnd.tizen.contact";
		const char *const DATA_RESULT_TYPE  = "phone";

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
		reset(true);

		m_app.getSystemEventManager().removeSystemEventHandler(
				appfw::SystemEventHandler::wrap<ContactTelNumberPicker, &ContactTelNumberPicker::onSystemEvent>(this));
	}

	app_control_h ContactTelNumberPicker::createAppControl()
	{
		app_control_h appControl = nullptr;
		int res = app_control_create(&appControl);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, nullptr, "Failed to create app_control, result %d", res);

		if (!initAppControl(appControl)) {
			app_control_destroy(appControl);
			return nullptr;
		}

		return appControl;
	}

	bool ContactTelNumberPicker::initAppControl(app_control_h appControl)
	{
		int res = app_control_set_operation(appControl, APP_CONTROL_OPERATION_PICK);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to set operation, result %d", res);

		res = app_control_set_mime(appControl, MIME_CONTACTS);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to set mime, result %d", res);

		res = app_control_add_extra_data(appControl, APP_CONTROL_DATA_TYPE, DATA_RESULT_TYPE);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to add extra data, result %d", res);

		res = app_control_set_launch_mode(appControl, APP_CONTROL_LAUNCH_MODE_SINGLE);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to set launch mode, result %d", res);

		return true;
	}

	ContactTelNumberPicker::ResultCode ContactTelNumberPicker::launch(ResultEventHandler resultHandler)
	{
		RETVM_IF(m_pAppControl, RES_INVALID_STATE_ERROR, "App is already launched!");

		m_pAppControl = createAppControl();
		RETVM_IF(!m_pAppControl, RES_LAUNCH_ERROR, "Failed to setup app control!");

		m_resultHandler = resultHandler;

		auto result = sendLunchRequest();
		if (result != RES_SUCCESS) {
			reset();
		}

		return result;
	}

	void ContactTelNumberPicker::terminate()
	{
		reset(true);
	}

	void ContactTelNumberPicker::reset(bool terminateApp)
	{
		m_resultHandler = nullptr;

		if (m_pAppControl) {
			if (terminateApp) {
				app_control_send_terminate_request(m_pAppControl);
			}

			app_control_destroy(m_pAppControl);
			m_pAppControl = nullptr;
		}
	}

	void ContactTelNumberPicker::resetWithResult(ResultCode result, std::string telNumber)
	{
		auto resultHandler = m_resultHandler;
		reset();
		resultHandler(result, telNumber);
	}

	ContactTelNumberPicker::ResultCode ContactTelNumberPicker::sendLunchRequest()
	{
		auto res = app_control_send_launch_request(m_pAppControl,
			[](app_control_h request, app_control_h reply, app_control_result_e replyResult, void *data) {
				ContactTelNumberPicker *instance = static_cast<ContactTelNumberPicker *>(data);

				std::string telNumber;
				ResultCode result = RES_LAUNCH_ERROR;

				switch(replyResult) {
				case APP_CONTROL_RESULT_SUCCEEDED:
					result = instance->parseContactsReply(reply, telNumber);
					break;
				case APP_CONTROL_RESULT_CANCELED:
					result = RES_LAUNCH_CANCELED;
					break;
				default:
					ERR("Error result received: %d", replyResult);
				}
				instance->resetWithResult(result, telNumber);
		}, this);

		if (res != APP_CONTROL_ERROR_NONE) {
			return RES_LAUNCH_ERROR;
		}

		return RES_SUCCESS;
	}

	void ContactTelNumberPicker::onSystemEvent(appfw::SystemEvent event)
	{
		if (event == appfw::SYS_EVENT_RESUME && m_pAppControl) {
			resetWithResult(RES_APP_TERMINATED, "");
		}
	}

	ContactTelNumberPicker::ResultCode ContactTelNumberPicker::parseContactsReply(app_control_h reply, std::string &telNumber)
	{
		int telNumberId = -1;
		auto res = getTelNumberId(reply, telNumberId);

		if (res != RES_SUCCESS) {
			return res;
		}

		return getTelNumberById(telNumberId, telNumber);
	}

	ContactTelNumberPicker::ResultCode ContactTelNumberPicker::getTelNumberId(app_control_h reply, int &telNumberId)
	{
		char **array = nullptr;
		int count = 0;

		int err = app_control_get_extra_data_array(reply, APP_CONTROL_DATA_SELECTED, &array, &count);
		RETVM_IF(err != APP_CONTROL_ERROR_NONE, RES_APP_RESPONCE_ERROR, "Extra data parse error! Code: %d", err);

		RETVM_IF((count < 1 || !array), RES_APP_RESPONCE_ERROR, "Response data is empty!");

		auto resCode = RES_APP_RESPONCE_ERROR;
		if (array[0]) {
			telNumberId = atoi(array[0]);
			resCode = RES_SUCCESS;
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

		int res = contacts_connect();
		RETVM_IF(res != CONTACTS_ERROR_NONE, RES_CONTACTS_DB_ERROR, "Failed to connect to contacts_db! Error %d", res);

		contacts_record_h dbRecord = nullptr;
		res = contacts_db_get_record(_contacts_number._uri, telNumberId, &dbRecord);
		if (res != CONTACTS_ERROR_NONE) {
			ERR("Failed to get contacts_db record! Error %d", res);
			contacts_disconnect();
			return RES_CONTACTS_DB_ERROR;
		}

		char *rawTelNumber = nullptr;
		res = contacts_record_get_str_p(dbRecord, _contacts_number.number, &rawTelNumber);
		if (res != CONTACTS_ERROR_NONE) {
			ERR("Failed to get tel number! Error %d", res);
			contacts_record_destroy(dbRecord, true);
			contacts_disconnect();
			return RES_CONTACTS_DB_ERROR;
		}

		telNumber = rawTelNumber;
		normalizeTelNumber(telNumber);
		DBG("Selected telNumber %s", telNumber.c_str());

		contacts_record_destroy(dbRecord, true);
		contacts_disconnect();
		return RES_SUCCESS;
	}

} }
