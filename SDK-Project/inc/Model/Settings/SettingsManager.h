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
		virtual ResultCode addPropertyHandler(BoolKey key, BoolPropertyHandler handler) override;
		virtual ResultCode addPropertyHandler(IntKey key, IntPropertyHandler handler) override;
		virtual ResultCode addPropertyHandler(StringKey key, StringPropertyHandler handler) override;

		virtual void removePropertyHandler(BoolKey key, BoolPropertyHandler handler) override;
		virtual void removePropertyHandler(IntKey key, IntPropertyHandler handler) override;
		virtual void removePropertyHandler(StringKey key, StringPropertyHandler handler) override;

		virtual ResultCode setIntProperty(IntKey key, int value) override;
		virtual ResultCode getIntProperty(IntKey key, int &value) override;
		virtual ResultCode setBoolProperty(BoolKey key, bool value) override;
		virtual ResultCode getBoolProperty(BoolKey key, bool &value) override;
		virtual ResultCode setStringProperty(StringKey key, const std::string &value) override;
		virtual ResultCode getStringProperty(StringKey key, std::string &value) override;

	private:
		const char* convertPropertyKeyToVconfKey(IntKey key);
		const char* convertPropertyKeyToVconfKey(BoolKey key);
		const char* convertPropertyKeyToVconfKey(StringKey key);

		IntKey convertVconfKeyToIntKey(const char *VconfKey);
		BoolKey convertVconfKeyToBoolKey(const char *VconfKey);
		StringKey convertVconfKeyToStringKey(const char *VconfKey);

		ResultCode getValueByVconfKey(const char *VconfKey, int &value);
		ResultCode getValueByVconfKey(const char *VconfKey, bool &value);
		ResultCode getValueByVconfKey(const char *VconfKey, std::string &value);

		template <typename KEY_TYPE>
		ResultCode registerPropertyHandler(KEY_TYPE key);

		template <typename KEY_TYPE>
		void unregisterPropertyHandler(KEY_TYPE key);

		template <typename MAP_T>
		void clearHandlersMap(MAP_T map);

		template <typename KEY_T, typename HANDL_T, typename COLLECTION_T>
		ResultCode addHandlerImpl(KEY_T key, HANDL_T handler, std::map<KEY_T, COLLECTION_T*> &handlMap);

		template <typename KEY_T, typename HANDL_T, typename COLLECTION_T>
		void removeHandlerImpl(KEY_T key, HANDL_T handler, std::map<KEY_T, COLLECTION_T*> &handlMap);

		static void onVconfPropertyChangeNotifyCb(keynode_t *node, void *userData);
		void invokeIntPropertyHandlers(const char *VconfKey);
		void invokeBoolPropertyHandlers(const char *VconfKey);
		void invokeStringPropertyHandlers(const char *VconfKey);

		template<typename KEY_T, typename VALUE_T, typename HANDL_MAP_T>
		void invokeHandlersImpl(KEY_T key, HANDL_MAP_T &handlMap, const char *vconfKey);

	private:
		typedef Delegation<void(int)> IntHandlersCollection;
		typedef Delegation<void(bool)> BoolHandlersCollection;
		typedef Delegation<void(std::string)> StringHandlersCollection;

		typedef std::map<IntKey, IntHandlersCollection *> IntHandlersMap;
		typedef std::map<BoolKey, BoolHandlersCollection *> BoolHandlersMap;
		typedef std::map<StringKey, StringHandlersCollection *> StringHandlersMap;

		IntHandlersMap m_intHandlersMap;
		BoolHandlersMap m_boolHandlersMap;
		StringHandlersMap m_stringHandlersMap;
	};
} }

#endif /* SETTINGSMANAGER_H_ */
