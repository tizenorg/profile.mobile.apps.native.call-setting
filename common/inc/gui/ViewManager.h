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

#include "GuiTypes.h"
#include "BaseView.h"
#include "NaviItem.h"

namespace gui {

	/*
	 * @brief Represents instance of ViewManager
	 */
	class ViewManager {
	public:
		/*
		 * @brief ViewManager instance creation
		 * @return ViewManagerinstance
		 */
		static ViewManager *create();

		/*
		 * @brief Destroy view manager instance
		 */
		static void destroy(ViewManager *instance);

		/*
		 * @brief Setup edje theme for main window
		 * @param[in]	themePath	Path to edje theme
		 * @return true on success, otherwise false
		 */
		bool setupTheme(const std::string &themePath);

		/*
		 * @brief Fabric method for creation new View and provide view push into naviframe stack
		 * @param[in]	showAnimation	Animation state for push view
		 * @return	View instance pointer
		 **/
		template <class ViewType>
		ViewType *pushView(bool showAnimation = true);

		/*
		 * @brief Pop view from naviframe stack
		 * @return true on success otherwise false
		 */
		bool popView();

		/*
		 * @brief Check whether view is on top or not
		 * @param[in]	view	View instance
		 * @return true if view is top most, otherwise false
		 */
		bool isTopView(BaseView *view);

		/*
		 * @brief Register view event handler.
		 * @param[in]	handler		View event handler
		 * @return true on success otherwise false
		 */
		bool addViewEventHandler(ViewEventHandler handler);

		/*
		 * @brief Unregister view event handler.
		 * @param[in]	handler		View event handler
		 */
		void removeViewEventHandler(ViewEventHandler handler);

		/*
		 * @return Get window evas object instance
		 */
		Evas_Object *getWindow();

		/*
		 * @return Get naviframe evas object instance
		 */
		Evas_Object *getNaviframe();

		/*
		 * @brief Show toast popup message on top most view
		 * @param[in]	notiText	Notification text message
		 */
		void showToastNotification(util::TString notiText);

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
		Evas_Object *m_pConformant;
		Evas_Object *m_pNaviframe;
		Elm_Theme *m_pTheme;

		std::list<BaseView *> m_ViewList;
		ViewEventHandlersDelegation m_ViewEventHandlers;

		bool isTransitionState;
		Ecore_Job * m_pActivateJob;
	};

	template <class ViewType>
	ViewType *ViewManager::pushView(bool showAnimation)
	{
		NaviItem *naviItem = createNaviItem(showAnimation);
		RETVM_IF(!naviItem, nullptr, "Failed to create NaviItem!");

		ViewType *view = BaseView::create<ViewType>(naviItem);
		RETVM_IF(!view, nullptr, "Failed to create view!");

		attachView(view, showAnimation);
		return view;
	}
}

#endif /* _GUI_VIEW_MANAGER_H_ */
