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

#ifndef SYSTEM_EVENT_PROVIDER_H_
#define SYSTEM_EVENT_PROVIDER_H_

#include <set>
#include "App/SystemEventListener.h"
namespace App {
	enum {
		/*System orientation was changed*/
		CST_APP_ORIENTATION_CHANGED = 1,
		/* System language was changed */
		CST_APP_LANGUAGE_CHANGED = 2,
		/* System region format was changed */
		CST_APP_REGION_FMT_CHANGED = 4,
	};

	class SystemEventProvider {
	public:
		SystemEventProvider() {};
		~SystemEventProvider() {};
		bool addEventListener(SystemEventListener *listener);
		void removeEventListener(SystemEventListener *listener);
		void onLanguageChanged();
		void onOrientationChanged();
		void onRegionFormatChanged();
		void onAppPause();
		void onAppResume();
	private:
		std::set<SystemEventListener*> m_ListenersSet;
	};
}



#endif /* SYSTEM_EVENT_PROVIDER_H_ */
