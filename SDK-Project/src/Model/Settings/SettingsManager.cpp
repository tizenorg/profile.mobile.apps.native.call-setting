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
	while(!intPropertyListeners.empty()) {
		IntListenerPair pair = intPropertyListeners.back();
		intPropertyListeners.pop_back();
		unregisterPropertyListener<IntKey>(pair.first);
		pair.second->onDetach();
	}

	while(!boolPropertyListeners.empty()) {
		BoolListenerPair pair = boolPropertyListeners.back();
		boolPropertyListeners.pop_back();
		unregisterPropertyListener<BoolKey>(pair.first);
		pair.second->onDetach();
	}

	while(!stringPropertyListeners.empty()) {
		StringListenerPair pair = stringPropertyListeners.back();
		stringPropertyListeners.pop_back();
		unregisterPropertyListener<StringKey>(pair.first);
		pair.second->onDetach();
	}
}

ResultCode SettingsManager::setIntProperty(IntKey key, int value)
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

ResultCode SettingsManager::getIntProperty(IntKey key, int &value)
{
	const char *vconfKey = convertPropertyKeyToVconfKey(key);
	RETVM_IF(!vconfKey, SETTINGS_RES_FAIL_KEY_UNKNOWN, "Key was not found");

	return getIntValueByVconfKey(vconfKey, value);

}

ResultCode SettingsManager::setBoolProperty(BoolKey key, bool value)
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

ResultCode SettingsManager::getBoolProperty(BoolKey key, bool &value)
{
	notifyIntListenersForChanges(VCONFKEY_CISSAPPL_REJECT_CALL_MSG_INT);
	const char *vconfKey = convertPropertyKeyToVconfKey(key);
	RETVM_IF(!vconfKey, SETTINGS_RES_FAIL_KEY_UNKNOWN, "Key was not found");

	return getBoolValueByVconfKey(vconfKey, value);
}

ResultCode SettingsManager::setStringProperty(StringKey key, const std::string &value)
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

ResultCode SettingsManager::getStringProperty(StringKey key, std::string &value)
{
	const char *vconfKey = convertPropertyKeyToVconfKey(key);
	RETVM_IF(!vconfKey, SETTINGS_RES_FAIL_KEY_UNKNOWN, "Key was not found");

	return getStringValueByVconfKey(vconfKey, value);
}

const char* SettingsManager::convertPropertyKeyToVconfKey(IntKey key)
{
	switch(key) {
	case INT_KEY_SHOW_CALLER_ID:
		return VCONFKEY_TELEPHONY_SS_CLI_STATE;
	case INT_KEY_REJECT_MSG_COUNT:
		return VCONFKEY_CISSAPPL_REJECT_CALL_MSG_INT;
	default:
		return NULL;
	}
}

const char* SettingsManager::convertPropertyKeyToVconfKey(BoolKey key)
{
	switch(key) {
	case BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY:
		return VCONFKEY_CISSAPPL_ANSWERING_KEY_BOOL;
	case BOOL_KEY_CALL_ENDING_BY_POWER_KEY:
		return VCONFKEY_CISSAPPL_POWER_KEY_ENDS_CALL_BOOL;
	default:
		return NULL;
	}
}

const char* SettingsManager::convertPropertyKeyToVconfKey(StringKey key)
{
	switch(key) {
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
		return NULL;
	}
}

IntKey SettingsManager::convertVconfKeyToIntKey(const char *vconfKey)
{
	if (!strcmp(VCONFKEY_TELEPHONY_SS_CLI_STATE, vconfKey))
		return INT_KEY_SHOW_CALLER_ID;
	else if (!strcmp(VCONFKEY_CISSAPPL_REJECT_CALL_MSG_INT, vconfKey))
		return INT_KEY_REJECT_MSG_COUNT;
	else
		return INT_KEY_UNDEFINED;
}

BoolKey SettingsManager::convertVconfKeyToBoolKey(const char *vconfKey)
{
	if (!strcmp(VCONFKEY_CISSAPPL_ANSWERING_KEY_BOOL, vconfKey))
		return BOOL_KEY_CALL_ANSWERING_BY_HOME_KEY;
	else if (!strcmp(VCONFKEY_CISSAPPL_POWER_KEY_ENDS_CALL_BOOL, vconfKey))
		return BOOL_KEY_CALL_ENDING_BY_POWER_KEY;
	else
		return BOOL_KEY_UNDEFINED;
}

StringKey SettingsManager::convertVconfKeyToStringKey(const char *vconfKey)
{
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
	else
		return STRING_KEY_UNDEFINED;
}

ResultCode SettingsManager::getIntValueByVconfKey(const char *vconfKey, int &value)
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

ResultCode SettingsManager::getBoolValueByVconfKey(const char *vconfKey, bool &value)
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

ResultCode SettingsManager::getStringValueByVconfKey(const char *vconfKey, std::string &value)
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

template <typename KEY_TYPE>
ResultCode SettingsManager::registerPropertyListener(KEY_TYPE key)
{
	const char *vconfKey = convertPropertyKeyToVconfKey(key);
	RETVM_IF(!vconfKey, SETTINGS_RES_FAIL_KEY_UNKNOWN, "Key was not found");

	int result = vconf_notify_key_changed(vconfKey, onVconfPropertyChangeNotifyCb, this);
	RETVM_IF(result != 0, SETTINGS_RES_FAIL_UNKNOWN_ERROR, "Internal error: vconf_notify_key_changed failed");

	return SETTINGS_RES_SUCCESS;
}

template <typename KEY_TYPE>
void SettingsManager::unregisterPropertyListener(KEY_TYPE key)
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

	int vconfType = vconf_keynode_get_type(node);
	switch(vconfType) {
	case VCONF_TYPE_INT:
		manager->notifyIntListenersForChanges(vconfKey);
		break;
	case VCONF_TYPE_BOOL:
		manager->notifyBoolListenersForChanges(vconfKey);
		break;
	case VCONF_TYPE_STRING:
		manager->notifyStringListenersForChanges(vconfKey);
		break;
	default:
		WARN("Unsupported Type! Ignore it");
	}
}

void SettingsManager::notifyIntListenersForChanges(const char *vconfKey)
{
	int value = 0;
	IntKey key = convertVconfKeyToIntKey(vconfKey);
	if (getIntValueByVconfKey(vconfKey, value) == SETTINGS_RES_SUCCESS && key != INT_KEY_UNDEFINED) {
		for(auto item : intPropertyListeners) {
			if(item.first == key) {
				item.second->onPropertyChanged(value);
			}
		}
	}
}

void SettingsManager::notifyBoolListenersForChanges(const char *vconfKey)
{
	bool value = false;
	BoolKey key = convertVconfKeyToBoolKey(vconfKey);
	if (getBoolValueByVconfKey(vconfKey, value) == SETTINGS_RES_SUCCESS && key != BOOL_KEY_UNDEFINED) {
		for(auto item : boolPropertyListeners) {
			if(item.first == key)
				item.second->onPropertyChanged(value);
		}
	}
}

void SettingsManager::notifyStringListenersForChanges(const char *vconfKey)
{
	std::string value;
	StringKey key = convertVconfKeyToStringKey(vconfKey);
	if (getStringValueByVconfKey(vconfKey, value) == SETTINGS_RES_SUCCESS && key != STRING_KEY_UNDEFINED) {
		for(auto item : stringPropertyListeners) {
			if(item.first == key)
				item.second->onPropertyChanged(value);
		}
	}
}

ResultCode SettingsManager::addPropertyListener(BoolKey key, PropertyListener<bool> *listener)
{
	ResultCode res = registerPropertyListener<BoolKey>(key);
	if (res == SETTINGS_RES_SUCCESS) {
		if (listener->onAttach(this)) {
			boolPropertyListeners.push_back(std::make_pair(key, listener));
		}
	}

	return res;
}

ResultCode SettingsManager::addPropertyListener(IntKey key, PropertyListener<int> *listener)
{
	ResultCode res = registerPropertyListener<IntKey>(key);
	if (res == SETTINGS_RES_SUCCESS) {
		if (listener->onAttach(this)) {
			intPropertyListeners.push_back(std::make_pair(key, listener));
		}
	}

	return res;
}

ResultCode SettingsManager::addPropertyListener(StringKey key, PropertyListener<std::string> *listener)
{
	ResultCode res = registerPropertyListener<StringKey>(key);
	if (res == SETTINGS_RES_SUCCESS) {
		if (listener->onAttach(this)) {
			stringPropertyListeners.push_back(std::make_pair(key, listener));
		}
	}

	return res;
}

void SettingsManager::removePropertyListener(PropertyListener<bool> *listener)
{
	auto it = find_if(boolPropertyListeners.begin(), boolPropertyListeners.end(), [&listener](const boolListenerPair &item) {
			return item.second == listener;
	});

	if (it == boolPropertyListeners.end()) {
		WARN("Try to unregister listener which is not set");
		return;
	}

	BoolListenerPair listenerPair = *it;
	unregisterPropertyListener<BoolKey>(listenerPair.first);

	boolPropertyListeners.erase(it);
	listener->onDetach();
}

void SettingsManager::removePropertyListener(PropertyListener<int> *listener)
{
	auto it = find_if(intPropertyListeners.begin(), intPropertyListeners.end(), [&listener](const IntListenerPair &item) {
				return item.second == listener;
	});

	if (it == intPropertyListeners.end()) {
		WARN("Try to unregister listener which is not set");
		return;
	}

	IntListenerPair listenerPair = *it;
	unregisterPropertyListener<IntKey>(listenerPair.first);

	intPropertyListeners.erase(it);
	listener->onDetach();
}

void SettingsManager::removePropertyListener(PropertyListener<std::string> *listener)
{
	auto it = find_if(stringPropertyListeners.begin(), stringPropertyListeners.end(), [&listener](const StringListenerPair &item) {
				return item.second == listener;
	});

	if (it == stringPropertyListeners.end()) {
		WARN("Try to unregister listener which is not set");
		return;
	}

	StringListenerPair listenerPair = *it;
	unregisterPropertyListener<StringKey>(listenerPair.first);

	stringPropertyListeners.erase(it);
	listener->onDetach();
}
