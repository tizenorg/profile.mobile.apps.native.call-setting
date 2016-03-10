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

#ifndef SIM_SETTINGS_VIEW_H_
#define SIM_SETTINGS_VIEW_H_

#include "View/ViewManager/View.h"

namespace View { namespace ViewManager {
	class SimSettingsView : public View {
	public:
		virtual void show() {};
		virtual void hide() override;
		virtual void activate() override;
		virtual void deactivate()override;
		virtual void onMenuKeyPressed() override;
		virtual void onBackKeyPressed() override;
	protected:
		friend SimSettingsView *View::create<SimSettingsView>(Widgets::NaviItem *naviItem);

		virtual bool createViewContent() override;
		SimSettingsView();
		virtual ~SimSettingsView();
	};
} }



#endif /* SIM_SETTINGS_VIEW_H_ */
