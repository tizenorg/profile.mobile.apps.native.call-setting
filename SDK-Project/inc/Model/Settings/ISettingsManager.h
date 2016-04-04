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

#ifndef _MODEL_I_SETTINGS_MANAGER_H_
#define _MODEL_I_SETTINGS_MANAGER_H_

#include "SettingsTypes.h"

namespace CallSettings { namespace Model {

	class ISettingsManager {
	public:
		virtual ~ISettingsManager(){}

		virtual SettingResultCode addPropertyHandler(BoolKey key, NotifyHandler handler) = 0;
		virtual SettingResultCode addPropertyHandler(IntKey key, NotifyHandler handler) = 0;
		virtual SettingResultCode addPropertyHandler(StringKey key, NotifyHandler handler) = 0;

		virtual void removePropertyHandler(BoolKey key, NotifyHandler handler) = 0;
		virtual void removePropertyHandler(IntKey key, NotifyHandler handler) = 0;
		virtual void removePropertyHandler(StringKey key, NotifyHandler handler) = 0;

		virtual SettingResultCode setProperty(IntKey key, int value) = 0;
		virtual SettingResultCode getProperty(IntKey key, int &value) = 0;
		virtual SettingResultCode setProperty(BoolKey key, bool value) = 0;
		virtual SettingResultCode getProperty(BoolKey key, bool &value) = 0;
		virtual SettingResultCode setProperty(StringKey key, const std::string &value) = 0;
		virtual SettingResultCode getProperty(StringKey key, std::string &value) = 0;
	};
} }

#endif /* _MODEL_I_SETTINGS_MANAGER_H_ */
