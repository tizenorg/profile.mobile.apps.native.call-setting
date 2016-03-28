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
	clearHandlersMap<IntHandlersMap>(m_intHandlersMap);
	clearHandlersMap<BoolHandlersMap>(m_boolHandlersMap);
	clearHandlersMap<StringHandlersMap>(m_stringHandlersMap);
}

template <typename MAP_T>
void SettingsManager::clearHandlersMap(MAP_T map)
{
	for (auto &it : map) {
		unregisterPropertyHandler<decltype(it.first)>(it.first);
		delete it.second;
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

	return getValueByVconfKey(vconfKey, value);

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
	const char *vconfKey = convertPropertyKeyToVconfKey(key);
	RETVM_IF(!vconfKey, SETTINGS_RES_FAIL_KEY_UNKNOWN, "Key was not found");

	return getValueByVconfKey(vconfKey, value);

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

	return getValueByVconfKey(vconfKey, value);
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

template <typename KEY_TYPE>
ResultCode SettingsManager::registerPropertyHandler(KEY_TYPE key)
{
	const char *vconfKey = convertPropertyKeyToVconfKey(key);
	RETVM_IF(!vconfKey, SETTINGS_RES_FAIL_KEY_UNKNOWN, "Key was not found");

	int result = vconf_notify_key_changed(vconfKey, onVconfPropertyChangeNotifyCb, this);
	RETVM_IF(result != 0, SETTINGS_RES_FAIL_UNKNOWN_ERROR, "Internal error: vconf_notify_key_changed failed");

	return SETTINGS_RES_SUCCESS;
}

template <typename KEY_TYPE>
void SettingsManager::unregisterPropertyHandler(KEY_TYPE key)
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
		manager->invokeIntPropertyHandlers(vconfKey);
		break;
	case VCONF_TYPE_BOOL:
		manager->invokeBoolPropertyHandlers(vconfKey);
		break;
	case VCONF_TYPE_STRING:
		manager->invokeStringPropertyHandlers(vconfKey);
		break;
	default:
		WARN("Unsupported Type! Ignore it");
	}
}

void SettingsManager::invokeIntPropertyHandlers(const char *vconfKey)
{
	IntKey key = convertVconfKeyToIntKey(vconfKey);
	RETM_IF(key == STRING_KEY_UNDEFINED, "Key value is not valid!");

	invokeHandlersImpl<IntKey, int, IntHandlersMap>(key, m_intHandlersMap, vconfKey);
}

void SettingsManager::invokeBoolPropertyHandlers(const char *vconfKey)
{
	BoolKey key = convertVconfKeyToBoolKey(vconfKey);
	RETM_IF(key == STRING_KEY_UNDEFINED, "Key value is not valid!");

	invokeHandlersImpl<BoolKey, bool, BoolHandlersMap>(key, m_boolHandlersMap, vconfKey);
}

void SettingsManager::invokeStringPropertyHandlers(const char *vconfKey)
{
	StringKey key = convertVconfKeyToStringKey(vconfKey);
	RETM_IF(key == STRING_KEY_UNDEFINED, "Key value is not valid!");

	invokeHandlersImpl<StringKey, std::string, StringHandlersMap>(key, m_stringHandlersMap, vconfKey);
}

template<typename KEY_T, typename VALUE_T, typename HANDL_MAP_T>
void SettingsManager::invokeHandlersImpl(KEY_T key, HANDL_MAP_T &handlMap, const char *vconfKey)
{
	VALUE_T value;
	if (getValueByVconfKey(vconfKey, value) == SETTINGS_RES_SUCCESS) {
		auto *collection = handlMap.find(key)->second;
		collection->invoke(value);
	}
}

ResultCode SettingsManager::addPropertyHandler(IntKey key, IntPropertyHandler handler)
{
	return addHandlerImpl<IntKey, IntPropertyHandler, IntHandlersCollection>(key, handler, m_intHandlersMap);
}

ResultCode SettingsManager::addPropertyHandler(BoolKey key, BoolPropertyHandler handler)
{
	return addHandlerImpl<BoolKey, BoolPropertyHandler, BoolHandlersCollection>(key, handler, m_boolHandlersMap);
}

ResultCode SettingsManager::addPropertyHandler(StringKey key, StringPropertyHandler handler)
{
	return addHandlerImpl<StringKey, StringPropertyHandler, StringHandlersCollection>(key, handler, m_stringHandlersMap);
}

void SettingsManager::removePropertyHandler(IntKey key, IntPropertyHandler handler)
{
	removeHandlerImpl<IntKey, IntPropertyHandler, IntHandlersCollection>(key, handler, m_intHandlersMap);
}

void SettingsManager::removePropertyHandler(BoolKey key, BoolPropertyHandler handler)
{
	removeHandlerImpl<BoolKey, BoolPropertyHandler, BoolHandlersCollection>(key, handler, m_boolHandlersMap);
}

void SettingsManager::removePropertyHandler(StringKey key, StringPropertyHandler handler)
{
	removeHandlerImpl<StringKey, StringPropertyHandler, StringHandlersCollection>(key, handler, m_stringHandlersMap);
}

template <typename KEY_T, typename HANDL_T, typename COLLECTION_T>
ResultCode SettingsManager::addHandlerImpl(KEY_T key, HANDL_T handler, std::map<KEY_T, COLLECTION_T*> &handlMap)
{
	auto it = handlMap.find(key);
	if (it == handlMap.end()) {
		ResultCode res = registerPropertyHandler<decltype(key)>(key);
		RETVM_IF(res != SETTINGS_RES_SUCCESS, res, "Failed to register vconf change callback!");

		COLLECTION_T *newCollection = new COLLECTION_T();
		*newCollection += handler;
		handlMap[key] = newCollection;
	} else {
		*(it->second) += handler;
	}

	return SETTINGS_RES_SUCCESS;
}

template <typename KEY_T, typename HANDL_T, typename COLLECTION_T>
void SettingsManager::removeHandlerImpl(KEY_T key, HANDL_T handler, std::map<KEY_T, COLLECTION_T*> &handlMap)
{
	auto it = handlMap.find(key);
	if (it != handlMap.end()) {
		COLLECTION_T *collection = handlMap[key];
		*collection -= handler;
		if(collection->isEmpty()) {
			unregisterPropertyHandler<KEY_T>(key);
			delete collection;
			handlMap.erase(it);
		}
	}
}
