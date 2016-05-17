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

#include "appfw/AppControl.h"

namespace appfw {
	AppControl::AppControl() :
		m_appControl(nullptr),
		m_appReply(nullptr)
	{
	}

	AppControl::~AppControl()
	{
		if (m_appControl) {
			app_control_destroy(m_appControl);
			m_appControl = nullptr;
		}
	}

	AppControl *AppControl::create(const char *operation, const char *mime, const char *uri)
	{
		AppControl *instance = new AppControl();
		if (!instance->initialize(operation, mime, uri)) {
			ERR("Failed to create AppControl instance!");
			delete instance;
			return nullptr;
		}

		return instance;
	}

	bool AppControl::initialize(const char *operation, const char *mime, const char *uri)
	{
		int res = APP_CONTROL_ERROR_NONE;
		res = app_control_create(&m_appControl);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to create app_control, result %d", res);

		res = app_control_set_operation(m_appControl, operation);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to set operation, result %d", res);

		if (mime) {
			res = app_control_set_mime(m_appControl, mime);
			RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to set mime, result %d", res);
		}

		if (uri) {
			res = app_control_set_uri(m_appControl, uri);
			RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to set uri, result %d", res);
		}

		return true;
	}

	void AppControl::setLaunchEventHandler(LaunchEventHandler launchEventHandler)
	{
		m_launchEventHandler = launchEventHandler;
	}

	bool AppControl::appLaunch(AppLaunchMode launchMode)
	{
		int res = APP_CONTROL_ERROR_NONE;
		if (launchMode == APP_LAUNCH_MODE_GROUP) {
			res = app_control_set_launch_mode(m_appControl, APP_CONTROL_LAUNCH_MODE_GROUP);
		} else {
			res = app_control_set_launch_mode(m_appControl, APP_CONTROL_LAUNCH_MODE_SINGLE);
		}
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to set launch mode, result %d", res);

		res = app_control_enable_app_started_result_event(m_appControl);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to enable start result event, result %d", res);

		res = app_control_send_launch_request(m_appControl,
			[](app_control_h request, app_control_h reply, app_control_result_e result, void *data) {
				AppControl *instance = static_cast<AppControl *>(data);
				if (result == APP_CONTROL_RESULT_CANCELED || result == APP_CONTROL_RESULT_FAILED) {
					DBG("Failed to launch app, error: %d", result);
				} else if (result == APP_CONTROL_RESULT_SUCCEEDED) {
					instance->m_appReply = reply;
				}

				if (instance->m_launchEventHandler.assigned()) {
					instance->m_launchEventHandler(toAppLaunchResult(result));
				}

			}, this);

		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to launch app, result %d", res);

		return true;
	}

	void AppControl::appTerminate()
	{
		int res = app_control_send_terminate_request(m_appControl);
		RETM_IF(res != APP_CONTROL_ERROR_NONE, "Failed to terminate app, result %d", res);
	}

	bool AppControl::setAppId(const char *appId)
	{
		int res = app_control_set_app_id(m_appControl, appId);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to set appId, result %d", res);

		return true;
	}

	bool AppControl::addExtraData(const char *key, const char *value)
	{
		int res = app_control_add_extra_data(m_appControl, key, value);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to add extra data, result %d", res);

		return true;
	}

	bool AppControl::addExtraDataArray(const char *key, const char **array, int length)
	{
		int res = app_control_add_extra_data_array(m_appControl, key, array, length);
		RETVM_IF(res != APP_CONTROL_ERROR_NONE, false, "Failed to add extra data array, result %d", res);

		return true;
	}

	bool AppControl::getReplyData(const char *key, std::string &replyData)
	{
		RETVM_IF(!key, false, "Invalid data!");
		RETVM_IF(!m_appReply, false, "Reply is empty!");

		char **array = nullptr;
		int count = 0;

		int err = app_control_get_extra_data_array(m_appReply, key, &array, &count);
		RETVM_IF(err  != APP_CONTROL_ERROR_NONE, false, "app_control_get_extra_data_array() failed err %d");

		if (array && array[0]) {
			replyData = array[0];
		}

		for (int i = 0; i < count; ++i) {
			free(array[i]);
		}

		free(array);
		return true;
	}

	bool AppControl::getReplyDataArray(const char *key, std::vector<std::string> &replyDataArray)
	{
		RETVM_IF(!key, false, "Invalid data!");
		RETVM_IF(!m_appReply, false, "Reply is empty!");

		char **array = nullptr;
		int count = 0;

		int err = app_control_get_extra_data_array(m_appReply, key, &array, &count);
		RETVM_IF(err  != APP_CONTROL_ERROR_NONE, false, "app_control_get_extra_data_array() failed err %d");
		replyDataArray.reserve(count);

		for (int i = 0; i < count; ++i) {
			replyDataArray.push_back(array[i]);
			free(array[i]);
		}
		free(array);

		return true;
	}

	AppControl::AppLaunchResult AppControl::toAppLaunchResult(app_control_result_e result)
	{
		switch(result) {
		case APP_CONTROL_RESULT_APP_STARTED:
			return APP_LAUNCH_RESULT_APP_STARTED;
		case APP_CONTROL_RESULT_SUCCEEDED:
			return APP_LAUNCH_RESULT_SUCCEEDED;
		case APP_CONTROL_RESULT_FAILED:
			return APP_LAUNCH_RESULT_FAILED;
		case APP_CONTROL_RESULT_CANCELED:
			return APP_LAUNCH_RESULT_REJECTED;
		}
	}
}
