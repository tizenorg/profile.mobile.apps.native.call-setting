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

#ifndef _GUI_VIEW_MANAGER_H_
#define _GUI_VIEW_MANAGER_H_

#include "gui/GuiTypes.h"
#include "gui/Base/BaseView.h"
#include "gui/Widgets/NaviItem.h"

namespace gui {
	class ViewManager {
	public:
		static ViewManager *create();
		static void destroy(ViewManager *instance);

		template <class ViewType>
		static ViewType* pushView(ViewManager &manager, bool showAnimation = true);
		bool popView();

		bool isTopView(BaseView *view);
		bool addViewEventHandler(ViewEventHandler handler);
		void removeViewEventHandler(ViewEventHandler handler);

	private:
		ViewManager();
		~ViewManager();
		bool initialize();
		bool createWindow();
		bool createNaviframe();
		NaviItem *createNaviItem(bool showAnimation);
		void attachView(BaseView *view,  bool showAnimation);
		Evas_Object *createSoftwareBackButton();
		void onHwBackKeyPressed(Evas_Object *obj, void *event_info);
		void onHwMenuKeyPressed(Evas_Object *obj, void *event_info);
		void onSoftwareBackKeyPressed(Evas_Object *obj, void *event_info);
		void onTransitionFinished(Evas_Object *obj, void *event_info);
		void onOrientationChanged(Evas_Object *obj, void *event_info);
		void onViewDestroy(BaseView *view);
		void backButtonClickHandler();
		void menuButtonClickHandler();

	private:
		Evas_Object *m_pWindow;
		Evas_Object *m_pRootLayout;
		Evas_Object *m_pNaviframe;

		std::list<BaseView *> m_ViewList;
		ViewEventHandlersDelegation m_ViewEventHandlers;

		bool isTransitionState;
		Ecore_Job * m_pActivateJob;
	};

	template <class ViewType>
	ViewType* ViewManager::pushView(ViewManager &manager, bool showAnimation)
	{
		NaviItem *naviItem = manager.createNaviItem(showAnimation);
		RETVM_IF(!naviItem, nullptr, "Failed to create NaviItem!");

		ViewType *view = BaseView::create<ViewType>(naviItem);
		RETVM_IF(!view, nullptr, "Failed to create view!");

		manager.attachView(view, showAnimation);
		return view;
	}
}

#endif /* _GUI_VIEW_MANAGER_H_ */
