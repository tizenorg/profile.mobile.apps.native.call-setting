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

	using namespace appfw;

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
		m_pAppControl(nullptr)
	{
	}

	TelNumberPicker::~TelNumberPicker()
	{
		delete m_pAppControl;
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
		m_pAppControl = AppControl::create(APP_CONTROL_OPERATION_PICK, mimeContact);
		RETVM_IF(!m_pAppControl, false, "Failed to create appControl");

		m_pAppControl->setLaunchEventHandler(AppControl::LaunchEventHandler::wrap<TelNumberPicker, &TelNumberPicker::contactLauncherHandler>(this));

		if (!m_pAppControl->addExtraData(APP_CONTROL_DATA_TYPE, DATA_RESULT_TYPE)) {
			ERR("Failed to set data type!");
			return false;
		}

		return true;
	}

	bool TelNumberPicker::launch(ResultEventHandler resultHandler)
	{
		m_resultHandler = resultHandler;

		if (!m_pAppControl->appLaunch()) {
			ERR("Failed to launch contacts!");
			return false;
		}

		return true;
	}

	void TelNumberPicker::terminate()
	{
		m_pAppControl->appTerminate();
	}

	void TelNumberPicker::contactLauncherHandler(AppControl::AppLaunchResult result)
	{
		if (result == AppControl::APP_LAUNCH_RESULT_FAILED || result == AppControl::APP_LAUNCH_RESULT_REJECTED) {
			ERR("Failed to launch contacts app!");
			m_resultHandler(false);
		} else if (result == AppControl::APP_LAUNCH_RESULT_SUCCEEDED) {
			m_resultHandler(true);
		}
	}

	bool TelNumberPicker::getTelNumber(std::string &telNumber)
	{
		std::string telNumberId;

		if (!m_pAppControl->getReplyData(APP_CONTROL_DATA_SELECTED, telNumberId)) {
			ERR("Failed to parse contacts data!");
			return false;
		}

		contacts_record_h dbRecord = nullptr;
		char *rawTelNumber = nullptr;

		int res = contacts_connect();
		RETVM_IF(res != CONTACTS_ERROR_NONE, false, "Failed to connect to contacts_db! Error %d", res);

		res = contacts_db_get_record(_contacts_number._uri, std::stoi (telNumberId, nullptr, 0), &dbRecord);
		if (res != CONTACTS_ERROR_NONE) {
			ERR("Failed to get contacts_db record! Error %d", res);
			contacts_disconnect();
			return false;
		}

		res = contacts_record_get_str_p(dbRecord, _contacts_number.number, &rawTelNumber);
		if (res != CONTACTS_ERROR_NONE) {
			ERR("Failed to get tel number! Error %d", res);
			contacts_disconnect();
			return false;
		}

		telNumber = rawTelNumber;
		telNumberNormalaizer(telNumber);
		DBG("Selected telNumber %s", telNumber.c_str());

		contacts_disconnect();
		return true;
	}

} }
