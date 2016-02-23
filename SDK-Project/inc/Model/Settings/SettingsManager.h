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

#ifndef SETTINGS_MANAGER_H_
#define SETTINGS_MANAGER_H_

#include <vector>
#include <vconf.h>
#include "Model/Settings/ISettingsManager.h"

namespace Model { namespace Settings {
	class SettingsManager: public ISettingsManager {
	public:
		SettingsManager();
		virtual ~SettingsManager();
		virtual ResultCode addPropertyListener(BoolKey key, PropertyListener<bool> *listener);
		virtual ResultCode addPropertyListener(IntKey key, PropertyListener<int> *listener);
		virtual ResultCode addPropertyListener(StringKey key, PropertyListener<std::string> *listener);
		virtual void removePropertyListener(PropertyListener<bool> *listener);
		virtual void removePropertyListener(PropertyListener<int> *listener);
		virtual void removePropertyListener(PropertyListener<std::string> *listener);
		virtual ResultCode setIntProperty(IntKey key, int value);
		virtual ResultCode getIntProperty(IntKey key, int &value);
		virtual ResultCode setBoolProperty(BoolKey key, bool value);
		virtual ResultCode getBoolProperty(BoolKey key, bool &value);
		virtual ResultCode setStringProperty(StringKey key, const std::string &value);
		virtual ResultCode getStringProperty(StringKey key, std::string &value);

	private:
		typedef std::pair<IntKey, PropertyListener<int> *> intListenerPair;
		typedef std::pair<BoolKey,PropertyListener<bool> *> boolListenerPair;
		typedef std::pair<StringKey,PropertyListener<std::string> *> stringListenerPair;

		std::vector<intListenerPair> intPropertyListeners;
		std::vector<boolListenerPair> boolPropertyListeners;
		std::vector<stringListenerPair> stringPropertyListeners;

		const char* convertPropertyKeyToVconfKey(IntKey key);
		const char* convertPropertyKeyToVconfKey(BoolKey key);
		const char* convertPropertyKeyToVconfKey(StringKey key);

		IntKey convertVconfKeyToIntKey(const char *VconfKey);
		BoolKey convertVconfKeyToBoolKey(const char *VconfKey);
		StringKey convertVconfKeyToStringKey(const char *VconfKey);

		ResultCode getIntValueByVconfKey(const char *VconfKey, int &value);
		ResultCode getBoolValueByVconfKey(const char *VconfKey, bool &value);
		ResultCode getStringValueByVconfKey(const char *VconfKey, std::string &value);

		template <typename KEY_TYPE>
		ResultCode registerPropertyListener(KEY_TYPE key);

		template <typename KEY_TYPE>
		void unregisterPropertyListener(KEY_TYPE key);

		static void onVconfPropertyChangeNotifyCb(keynode_t *node, void *userData);
		void notifyIntListenersForChanges(const char *VconfKey);
		void notifyBoolListenersForChanges(const char *VconfKey);
		void notifyStringListenersForChanges(const char *VconfKey);
	};
} }

#endif /* SETTINGSMANAGER_H_ */
