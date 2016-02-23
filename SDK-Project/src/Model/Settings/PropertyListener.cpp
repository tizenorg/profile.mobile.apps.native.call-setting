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
#include <string>
#include "Model/Settings/PropertyListener.h"
#include "Model/Settings/SettingsManager.h"
#include "Utils/Logger.h"

namespace Model { namespace Settings {

	template <typename VALUE_TYPE>
	PropertyListener<VALUE_TYPE>::PropertyListener()
		: m_pSettingsManager(nullptr)
	{

	}

	template <typename VALUE_TYPE>
	PropertyListener<VALUE_TYPE>::~PropertyListener()
	{
		detach();
	}

	template <typename VALUE_TYPE>
	bool PropertyListener<VALUE_TYPE>::onAttach(ISettingsManager *settingsManager)
	{
		if (m_pSettingsManager) {
			return false;
		} else {
			m_pSettingsManager = settingsManager;
		}
		return true;
	}

	template <typename VALUE_TYPE>
	void PropertyListener<VALUE_TYPE>::onDetach()
	{
		m_pSettingsManager = nullptr;
	}

	template <typename VALUE_TYPE>
	void PropertyListener<VALUE_TYPE>::detach()
	{
		if (m_pSettingsManager) {
			m_pSettingsManager->removePropertyListener(this);
		}
	}

	template <typename VALUE_TYPE>
	bool PropertyListener<VALUE_TYPE>::isAttached()
	{
		if (m_pSettingsManager) {
			return true;
		} else {
			return false;
		}
	}

	template class PropertyListener<int>;
	template class PropertyListener<bool>;
	template class PropertyListener<std::string>;

} }
