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

#ifndef I_SETTINGS_MANAGER_H_
#define I_SETTINGS_MANAGER_H_

#include <string>
#include "Model/Settings/SettingsTypes.h"
#include "Model/Settings/PropertyListener.h"

namespace Model { namespace Settings {
	class ISettingsManager {
	private:
		ISettingsManager(ISettingsManager const &) = delete;
		ISettingsManager& operator = (ISettingsManager const &) = delete;
	public:
		ISettingsManager();
		virtual ~ISettingsManager();
		virtual ResultCode addBoolPropertyListener(BoolKey key, PropertyListener<bool> *listener) = 0;
		virtual ResultCode addIntPropertyListener(IntKey key, PropertyListener<int> *listener) = 0;
		virtual ResultCode addStringPropertyListener(StringKey key, PropertyListener<std::string> *listener) = 0;
		virtual void removeBoolPropertyListener(PropertyListener<bool> *listener) = 0;
		virtual void removeIntPropertyListener(PropertyListener<int> *listener) = 0;
		virtual void removeStringPropertyListener(PropertyListener<std::string> *listener) = 0;
		virtual void removeAllListeners() = 0;
		virtual ResultCode setIntProperty(IntKey key, int &value) = 0;
		virtual ResultCode getIntProperty(IntKey key, int &value) = 0;
		virtual ResultCode setBoolProperty(BoolKey key, bool &value) = 0;
		virtual ResultCode getBoolProperty(BoolKey key, bool &value) = 0;
		virtual ResultCode setStringProperty(StringKey key, std::string &value) = 0;
		virtual ResultCode getStringProperty(StringKey key, std::string &value) = 0;
	};
} }

#endif /* I_SETTINGS_MANAGER_H_ */
