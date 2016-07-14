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

	/*
	 * Represents VCONF wrapper interface
	 */
	class ISettingsManager {
	public:
		virtual ~ISettingsManager(){}

		/*
		 * @brief Add callback for change Boolean VCONF preference
		 * @param[in]	key			Boolean setting key
		 * @param[in]	handler		Change event handler
		 * @return Result code
		 */
		virtual SettingResultCode addPropertyHandler(BoolKey key, NotiHandler handler) = 0;

		/*
		 * @brief Add callback for change Int VCONF preference
		 * @param[in]	key			Int setting key
		 * @param[in]	handler		Change event handler
		 * @return Result code
		 */
		virtual SettingResultCode addPropertyHandler(IntKey key, NotiHandler handler) = 0;

		/*
		 * @brief Add callback for change string VCONF preference
		 * @param[in]	key			String setting key
		 * @param[in]	handler		Change event handler
		 * @return Result code
		 */
		virtual SettingResultCode addPropertyHandler(StringKey key, NotiHandler handler) = 0;

		/*
		 * @brief Remove callback for change Boolean VCONF preference
		 * @param[in]	key			Boolean setting key
		 * @param[in]	handler		Change event handler
		 * @return Result code
		 */
		virtual void removePropertyHandler(BoolKey key, NotiHandler handler) = 0;

		/*
		 * @brief Remove callback for change Int VCONF preference
		 * @param[in]	key			Int setting key
		 * @param[in]	handler		Change event handler
		 * @return Result code
		 */
		virtual void removePropertyHandler(IntKey key, NotiHandler handler) = 0;

		/*
		 * @brief Remove callback for change string VCONF preference
		 * @param[in]	key			String setting key
		 * @param[in]	handler		Change event handler
		 * @return Result code
		 */
		virtual void removePropertyHandler(StringKey key, NotiHandler handler) = 0;

		/*
		 * @brief Set new value for int VCONF preference
		 * @param[in]	key		int VCONF preference
		 * @param[in]	value	New value of preference to be set
		 * @return Result code
		 */
		virtual SettingResultCode setProperty(IntKey key, int value) = 0;

		/*
		 * @brief Get current value for int VCONF preference
		 * @param[in]	key		int VCONF preference
		 * @param[out]	value	Current value of preference
		 * @return Result code
		 */
		virtual SettingResultCode getProperty(IntKey key, int &value) = 0;

		/*
		 * @brief Set new value for bool VCONF preference
		 * @param[in]	key		bool VCONF preference
		 * @param[in]	value	New value of preference to be set
		 * @return Result code
		 */
		virtual SettingResultCode setProperty(BoolKey key, bool value) = 0;

		/*
		 * @brief Get current value for bool VCONF preference
		 * @param[in]	key		bool VCONF preference
		 * @param[out]	value	Current value of preference
		 * @return Result code
		 */
		virtual SettingResultCode getProperty(BoolKey key, bool &value) = 0;

		/*
		 * @brief Set new value for string VCONF preference
		 * @param[in]	key		string VCONF preference
		 * @param[in]	value	New value of preference to be set
		 * @return Result code
		 */
		virtual SettingResultCode setProperty(StringKey key, const std::string &value) = 0;

		/*
		 * @brief Get current value for string VCONF preference
		 * @param[in]	key		string VCONF preference
		 * @param[out]	value	Current value of preference
		 * @return Result code
		 */
		virtual SettingResultCode getProperty(StringKey key, std::string &value) = 0;
	};
} }

#endif /* _MODEL_I_SETTINGS_MANAGER_H_ */
