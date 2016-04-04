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

#ifndef _MODEL_SETTINGS_MANAGER_H_
#define _MODEL_SETTINGS_MANAGER_H_

#include <vconf.h>

#include "ISettingsManager.h"

namespace CallSettings { namespace Model {

	class SettingsManager: public ISettingsManager {
	public:
		SettingsManager();
		virtual ~SettingsManager();

		virtual SettingResultCode addPropertyHandler(BoolKey key, NotiHandler handler) override;
		virtual SettingResultCode addPropertyHandler(IntKey key, NotiHandler handler) override;
		virtual SettingResultCode addPropertyHandler(StringKey key, NotiHandler handler) override;

		virtual void removePropertyHandler(BoolKey key, NotiHandler handler) override;
		virtual void removePropertyHandler(IntKey key, NotiHandler handler) override;
		virtual void removePropertyHandler(StringKey key, NotiHandler handler) override;

		virtual SettingResultCode setProperty(IntKey key, int value) override;
		virtual SettingResultCode getProperty(IntKey key, int &value) override;
		virtual SettingResultCode setProperty(BoolKey key, bool value) override;
		virtual SettingResultCode getProperty(BoolKey key, bool &value) override;
		virtual SettingResultCode setProperty(StringKey key, const std::string &value) override;
		virtual SettingResultCode getProperty(StringKey key, std::string &value) override;

	private:
		const char* convertPropertyKeyToVconfKey(int key);
		int convertVconfKeyToPropertyKey(const char *VconfKey);
		SettingResultCode getValueByVconfKey(const char *VconfKey, int &value);
		SettingResultCode getValueByVconfKey(const char *VconfKey, bool &value);
		SettingResultCode getValueByVconfKey(const char *VconfKey, std::string &value);
		SettingResultCode registerVconfKeyChangeCb(int key);
		void unregisterVconfKeyChangeCb(int key);

		SettingResultCode addHandlerImpl(int key, NotiHandler handler);
		void removeHandlerImpl(int key, NotiHandler handler);

		static void onVconfPropertyChangeNotifyCb(keynode_t *node, void *userData);
		void invokePropertyHandlers(const char *vconfKey);

	private:
		typedef util::Delegation<void()> HandlersCollection;
		typedef std::map<int, HandlersCollection *> HandlersMap;

		HandlersMap m_handlersMap;
	};
} }

#endif /* _MODEL_SETTINGS_MANAGER_H_ */
