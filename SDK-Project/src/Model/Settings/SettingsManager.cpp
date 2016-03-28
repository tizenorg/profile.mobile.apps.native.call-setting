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

#include <algorithm>
#include "Model/Settings/SettingsManager.h"
#include "Utils/Logger.h"

using namespace Model::Settings;

SettingsManager::SettingsManager()
{
}

SettingsManager::~SettingsManager()
{
	for (auto &it : m_handlersMap) {
		unregisterVconfKeyChangeCb(it.first);
		delete it.second;
	}
}

ResultCode SettingsManager::setProperty(IntKey key, int value)
{
	const char *vconfKey = convertPropertyKeyToVconfKey(key);
	RETVM_IF(!vconfKey, SETTINGS_RES_FAIL_KEY_UNKNOWN, "Key was not found");

	if (vconf_set_int(vconfKey, value) < 0) {
		ERR("Failed to set vconf key %s", vconfKey);
		return SETTINGS_RES_FAIL_UNKNOWN_ERROR;
	} else {
		return SETTINGS_RES_SUCCESS;
	}
}

ResultCode SettingsManager::getProperty(IntKey key, int &value)
{
	const char *vconfKey = convertPropertyKeyToVconfKey(key);
	RETVM_IF(!vconfKey, SETTINGS_RES_FAIL_KEY_UNKNOWN, "Key was not found");

	return getValueByVconfKey(vconfKey, value);

}

ResultCode SettingsManager::setProperty(BoolKey key, bool value)
{
	const char *vconfKey = convertPropertyKeyToVconfKey(key);
	RETVM_IF(!vconfKey, SETTINGS_RES_FAIL_KEY_UNKNOWN, "Key was not found");
	if (vconf_set_bool(vconfKey, static_cast<int>(value)) < 0) {
		ERR("Failed to set vconf key %s", vconfKey);
		return SETTINGS_RES_FAIL_UNKNOWN_ERROR;
	} else {
		return SETTINGS_RES_SUCCESS;
	}
}

ResultCode SettingsManager::getProperty(BoolKey key, bool &value)
{
	const char *vconfKey = convertPropertyKeyToVconfKey(key);
	RETVM_IF(!vconfKey, SETTINGS_RES_FAIL_KEY_UNKNOWN, "Key was not found");

	return getValueByVconfKey(vconfKey, value);

}

ResultCode SettingsManager::setProperty(StringKey key, const std::string &value)
{
	const char *vconfKey = convertPropertyKeyToVconfKey(key);
	RETVM_IF(!vconfKey, SETTINGS_RES_FAIL_KEY_UNKNOWN, "Key was not found");
	if (vconf_set_str(vconfKey, value.c_str()) < 0) {
		ERR("Failed to set vconf key %s", vconfKey);
		return SETTINGS_RES_FAIL_UNKNOWN_ERROR;
	} else {
		return SETTINGS_RES_SUCCESS;
	}
}

ResultCode SettingsManager::getProperty(StringKey key, std::string &value)
{
	const char *vconfKey = convertPropertyKeyToVconfKey(key);
	RETVM_IF(!vconfKey, SETTINGS_RES_FAIL_KEY_UNKNOWN, "Key was not found");

	return getValueByVconfKey(vconfKey, value);
}

const char* SettingsManager::convertPropertyKeyToVconfKey(int key)
{
	switch(key) {
	// IntKey Conversion //
	case INT_KEY_SHOW_CALLER_ID:
		return VCONFKEY_TELEPHONY_SS_CLI_STATE;
	case INT_KEY_REJECT_MSG_COUNT:
		return VCONFKEY_CISSAPPL_REJECT_CALL_MSG_INT;
	// BoolKey Conversion //
	case BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY:
		return VCONFKEY_CISSAPPL_ANSWERING_KEY_BOOL;
	case BOOL_KEY_CALL_ENDING_BY_POWER_KEY:
		return VCONFKEY_CISSAPPL_POWER_KEY_ENDS_CALL_BOOL;
	// StringKey Conversion //
	case STRING_KEY_REJECT_MSG_ITEM_1:
		return VCONFKEY_CISSAPPL_USER_CREATE_MSG1_STR;
	case STRING_KEY_REJECT_MSG_ITEM_2:
		return VCONFKEY_CISSAPPL_USER_CREATE_MSG2_STR;
	case STRING_KEY_REJECT_MSG_ITEM_3:
		return VCONFKEY_CISSAPPL_USER_CREATE_MSG3_STR;
	case STRING_KEY_REJECT_MSG_ITEM_4:
		return VCONFKEY_CISSAPPL_USER_CREATE_MSG4_STR;
	case STRING_KEY_REJECT_MSG_ITEM_5:
		return VCONFKEY_CISSAPPL_USER_CREATE_MSG5_STR;
	case STRING_KEY_REJECT_MSG_ITEM_6:
		return VCONFKEY_CISSAPPL_USER_CREATE_MSG6_STR;
	default:
		return nullptr;
	}
}

int SettingsManager::convertVconfKeyToPropertyKey(const char *vconfKey)
{
	// IntKey Conversion //
	if (!strcmp(VCONFKEY_TELEPHONY_SS_CLI_STATE, vconfKey))
		return INT_KEY_SHOW_CALLER_ID;
	else if (!strcmp(VCONFKEY_CISSAPPL_REJECT_CALL_MSG_INT, vconfKey))
		return INT_KEY_REJECT_MSG_COUNT;

	// BoolKey Conversion //
	if (!strcmp(VCONFKEY_CISSAPPL_ANSWERING_KEY_BOOL, vconfKey))
		return BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY;
	else if (!strcmp(VCONFKEY_CISSAPPL_POWER_KEY_ENDS_CALL_BOOL, vconfKey))
		return BOOL_KEY_CALL_ENDING_BY_POWER_KEY;

	// StringKey Conversion //
	if (!strcmp(VCONFKEY_CISSAPPL_USER_CREATE_MSG1_STR, vconfKey))
		return STRING_KEY_REJECT_MSG_ITEM_1;
	else if (!strcmp(VCONFKEY_CISSAPPL_USER_CREATE_MSG2_STR, vconfKey))
		return STRING_KEY_REJECT_MSG_ITEM_2;
	else if (!strcmp(VCONFKEY_CISSAPPL_USER_CREATE_MSG3_STR, vconfKey))
		return STRING_KEY_REJECT_MSG_ITEM_3;
	else if (!strcmp(VCONFKEY_CISSAPPL_USER_CREATE_MSG4_STR, vconfKey))
		return STRING_KEY_REJECT_MSG_ITEM_4;
	else if (!strcmp(VCONFKEY_CISSAPPL_USER_CREATE_MSG5_STR, vconfKey))
		return STRING_KEY_REJECT_MSG_ITEM_5;
	else if (!strcmp(VCONFKEY_CISSAPPL_USER_CREATE_MSG6_STR, vconfKey))
		return STRING_KEY_REJECT_MSG_ITEM_6;

	return -1;
}

ResultCode SettingsManager::getValueByVconfKey(const char *vconfKey, int &value)
{
	int vconfValue = 0;
	if (vconf_get_int(vconfKey, &vconfValue) < 0) {
		ERR("Failed to get data from vconf property %s", vconfKey);
		return SETTINGS_RES_FAIL_UNKNOWN_ERROR;
	} else {
		value = vconfValue;
		return SETTINGS_RES_SUCCESS;
	}
}

ResultCode SettingsManager::getValueByVconfKey(const char *vconfKey, bool &value)
{
	int vconfValue = 0;
	if (vconf_get_bool(vconfKey, &vconfValue) < 0) {
		ERR("Failed to get data from vconf key %s", vconfKey);
		return SETTINGS_RES_FAIL_UNKNOWN_ERROR;
	} else {
		value = static_cast<bool>(vconfValue);
		return SETTINGS_RES_SUCCESS;
	}
}

ResultCode SettingsManager::getValueByVconfKey(const char *vconfKey, std::string &value)
{
	char *vconfValue = vconf_get_str(vconfKey);
	if (!vconfValue) {
		ERR("Failed to get data from vconf key %s", vconfKey);
		return SETTINGS_RES_FAIL_UNKNOWN_ERROR;
	} else {
		value.clear();
		value.assign(vconfValue);
		free(vconfValue);
		return SETTINGS_RES_SUCCESS;
	}
}

ResultCode SettingsManager::registerVconfKeyChangeCb(int key)
{
	const char *vconfKey = convertPropertyKeyToVconfKey(key);
	RETVM_IF(!vconfKey, SETTINGS_RES_FAIL_KEY_UNKNOWN, "Key was not found");

	int result = vconf_notify_key_changed(vconfKey, onVconfPropertyChangeNotifyCb, this);
	RETVM_IF(result != 0, SETTINGS_RES_FAIL_UNKNOWN_ERROR, "Internal error: vconf_notify_key_changed failed");

	return SETTINGS_RES_SUCCESS;
}

void SettingsManager::unregisterVconfKeyChangeCb(int key)
{
	const char *vconfKey = convertPropertyKeyToVconfKey(key);
	RETM_IF(!vconfKey, "Key was not found");

	vconf_ignore_key_changed(vconfKey, onVconfPropertyChangeNotifyCb);
}

void SettingsManager::onVconfPropertyChangeNotifyCb(keynode_t *node, void *userData)
{
	RETM_IF(!node, "System error: keynode data is Null");
	RETM_IF(!userData, "System error: keynode data is Null");

	SettingsManager *manager = static_cast<SettingsManager *>(userData);
	char *vconfKey = vconf_keynode_get_name(node);
	RETM_IF(!vconfKey, "Failed to get vconf key name");

	manager->invokePropertyHandlers(vconfKey);

}

void SettingsManager::invokePropertyHandlers(const char *vconfKey)
{
	int key = convertVconfKeyToPropertyKey(vconfKey);
	RETM_IF(key == -1, "Failed to convert vconfKey name!");

	auto *collection = m_handlersMap.find(key)->second;
	collection->invoke();
}

ResultCode SettingsManager::addPropertyHandler(IntKey key, NotifyHandler handler)
{
	return addHandlerImpl(key, handler);
}

ResultCode SettingsManager::addPropertyHandler(BoolKey key, NotifyHandler handler)
{
	return addHandlerImpl(key, handler);
}

ResultCode SettingsManager::addPropertyHandler(StringKey key, NotifyHandler handler)
{
	return addHandlerImpl(key, handler);
}

void SettingsManager::removePropertyHandler(IntKey key, NotifyHandler handler)
{
	removeHandlerImpl(key, handler);
}

void SettingsManager::removePropertyHandler(BoolKey key, NotifyHandler handler)
{
	removeHandlerImpl(key, handler);
}

void SettingsManager::removePropertyHandler(StringKey key, NotifyHandler handler)
{
	removeHandlerImpl(key, handler);
}

ResultCode SettingsManager::addHandlerImpl(int key, NotifyHandler handler)
{
	auto it = m_handlersMap.find(key);
	if (it == m_handlersMap.end()) {
		ResultCode res = registerVconfKeyChangeCb(key);
		RETVM_IF(res != SETTINGS_RES_SUCCESS, res, "Failed to register vconf change callback!");

		HandlersCollection *newCollection = new HandlersCollection();
		*newCollection += handler;
		m_handlersMap[key] = newCollection;
	} else {
		*(it->second) += handler;
	}

	return SETTINGS_RES_SUCCESS;
}

void SettingsManager::removeHandlerImpl(int key, NotifyHandler handler)
{
	auto it = m_handlersMap.find(key);
	if (it != m_handlersMap.end()) {
		HandlersCollection *collection = m_handlersMap[key];
		*collection -= handler;
		if(collection->isEmpty()) {
			unregisterVconfKeyChangeCb(key);
			delete collection;
			m_handlersMap.erase(it);
		}
	}
}
