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

#include <map>
#include <vconf.h>

#include "Model/Settings/ISettingsManager.h"

namespace Model { namespace Settings {

	class SettingsManager: public ISettingsManager {
	public:
		SettingsManager();
		virtual ~SettingsManager();

		virtual ResultCode addPropertyHandler(BoolKey key, NotifyHandler handler) override;
		virtual ResultCode addPropertyHandler(IntKey key, NotifyHandler handler) override;
		virtual ResultCode addPropertyHandler(StringKey key, NotifyHandler handler) override;

		virtual void removePropertyHandler(BoolKey key, NotifyHandler handler) override;
		virtual void removePropertyHandler(IntKey key, NotifyHandler handler) override;
		virtual void removePropertyHandler(StringKey key, NotifyHandler handler) override;

		virtual ResultCode setProperty(IntKey key, int value) override;
		virtual ResultCode getProperty(IntKey key, int &value) override;
		virtual ResultCode setProperty(BoolKey key, bool value) override;
		virtual ResultCode getProperty(BoolKey key, bool &value) override;
		virtual ResultCode setProperty(StringKey key, const std::string &value) override;
		virtual ResultCode getProperty(StringKey key, std::string &value) override;

	private:
		const char* convertPropertyKeyToVconfKey(int key);
		int convertVconfKeyToPropertyKey(const char *VconfKey);
		ResultCode getValueByVconfKey(const char *VconfKey, int &value);
		ResultCode getValueByVconfKey(const char *VconfKey, bool &value);
		ResultCode getValueByVconfKey(const char *VconfKey, std::string &value);
		ResultCode registerVconfKeyChangeCb(int key);
		void unregisterVconfKeyChangeCb(int key);
		ResultCode addHandlerImpl(int key, NotifyHandler handler);
		void removeHandlerImpl(int key, NotifyHandler handler);
		static void onVconfPropertyChangeNotifyCb(keynode_t *node, void *userData);
		void invokePropertyHandlers(const char *vconfKey);

	private:
		typedef Delegation<void()> HandlersCollection;
		typedef std::map<int, HandlersCollection *> HandlersMap;

		HandlersMap m_handlersMap;
	};
} }

#endif /* SETTINGSMANAGER_H_ */
