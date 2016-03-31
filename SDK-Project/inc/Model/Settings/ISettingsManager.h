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

namespace Model { namespace Settings {
	class ISettingsManager {
	public:

		virtual ~ISettingsManager(){}

		virtual ResultCode addPropertyHandler(BoolKey key, NotiHandler handler) = 0;
		virtual ResultCode addPropertyHandler(IntKey key, NotiHandler handler) = 0;
		virtual ResultCode addPropertyHandler(StringKey key, NotiHandler handler) = 0;

		virtual void removePropertyHandler(BoolKey key, NotiHandler handler) = 0;
		virtual void removePropertyHandler(IntKey key, NotiHandler handler) = 0;
		virtual void removePropertyHandler(StringKey key, NotiHandler handler) = 0;

		virtual ResultCode setProperty(IntKey key, int value) = 0;
		virtual ResultCode getProperty(IntKey key, int &value) = 0;
		virtual ResultCode setProperty(BoolKey key, bool value) = 0;
		virtual ResultCode getProperty(BoolKey key, bool &value) = 0;
		virtual ResultCode setProperty(StringKey key, const std::string &value) = 0;
		virtual ResultCode getProperty(StringKey key, std::string &value) = 0;
	};
} }

#endif /* I_SETTINGS_MANAGER_H_ */
