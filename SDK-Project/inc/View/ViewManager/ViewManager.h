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

#ifndef VIEW_MANAGER_H_
#define VIEW_MANAGER_H_

#include <Elementary.h>
#include <set>
#include "View/Widgets/WidgetItem.h"

namespace View {
	class BaseView;
	class ViewEventListener;
	class ViewManager {
	public:
		static ViewManager *create();
		static void destroy(ViewManager *instance);

		template <class ViewType>
		static ViewType* pushView(ViewManager *manager, bool showAnimation = true);
		bool popView();
		bool isTopView(BaseView *view);
		bool addEventListener(ViewEventListener *listener);
		void removeEventListener(ViewEventListener *listener);

	private:
		ViewManager();
		~ViewManager();
		bool initialize();
		bool createWindow();
		bool createNaviframe();
		void onBackKeyPressed();
		void onMenuKeyPressed();
		void onTransitionFinished();
		void onNaviItemDestroy(Widgets::WidgetItem *widgetItem);
		BaseView *getTopView();


	private:
		Evas_Object *m_pWindow;
		Evas_Object *m_pRootLayout;
		Evas_Object *m_pNaviframe;
		bool isTransitionState;
		std::set<ViewEventListener*> m_ListenersSet;
		BaseView *m_pActiveView;
	};
}

#endif /* VIEW_MANAGER_H_ */
