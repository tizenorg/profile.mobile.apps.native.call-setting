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

#ifndef PROPERTY_LISTENER_H_
#define PROPERTY_LISTENER_H_

namespace Model { namespace Settings {

	class ISettingManager;

	template <typename VALUE_TYPE>
	class IPropertyListener {
	public:
		virtual ~IPropertyListener(){}
		virtual bool onAttach(ISettingManager *settingsManager) = 0;
		virtual void onDetach() = 0;
		virtual void onPropertyChanged(const VALUE_TYPE &value);
	};

	template <typename VALUE_TYPE>
	class PropertyListener : public IPropertyListener<VALUE_TYPE> {
	private:
		ISettingManager *m_pSettingManager;
	public:
		PropertyListener();
		virtual ~PropertyListener();
		void detach();
		bool isAttached();
	};
} }
#endif /* PROPERTY_LISTENER_H_ */
