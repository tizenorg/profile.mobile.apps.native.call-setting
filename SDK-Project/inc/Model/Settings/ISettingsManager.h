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

#include "Model/Settings/SettingsTypes.h"
namespace Model {
	namespace Settings {
		class ISettingsManager {
		public:
			ISettingsManager();
			virtual ~ISettingsManager();
			virtual ResultCode addSettingsListener(KeyType key, BaseSettingsListener *listener) = 0;
			virtual void removeSettingsListenerById(int listenerId) = 0;
			virtual void removeAllListeners() = 0;
			virtual ResultCode setIntSettingValueByKey(KeyType key, int keyValue) = 0;
			virtual ResultCode getIntSettingValueByKey(KeyType key, int& keyValue) = 0;
			virtual ResultCode setBoolSettingValueByKey(KeyType key, bool keyValue) = 0;
			virtual ResultCode getBoolSettingValueByKey(KeyType key, bool& keyValue) = 0;
			virtual ResultCode setTextSettingValueByKey(KeyType key, string keyValue) = 0;
			virtual ResultCode getTextSettingValueByKey(KeyType key, string& keyValue) = 0;
		};
	}
}


#endif /* I_SETTINGS_MANAGER_H_ */
