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

#include "gui/Base/ViewManager.h"

namespace gui {

	ViewManager::ViewManager() :
		m_pWindow(nullptr),
		m_pRootLayout(nullptr),
		m_pNaviframe(nullptr),
		isTransitionState(false),
		m_pActivateJob(nullptr)
	{
	}

	ViewManager::~ViewManager()
	{
		if (m_pActivateJob) {
			ecore_job_del(m_pActivateJob);
		}
	}

	ViewManager *ViewManager::create()
	{
		ViewManager *instance = new ViewManager();
		if (!instance->initialize()) {
			ViewManager::destroy(instance);
			return nullptr;
		}

		return instance;
	}

	void ViewManager::destroy(ViewManager *instance)
	{
		evas_object_del(instance->m_pWindow);
		delete instance;
	}

	bool ViewManager::initialize()
	{
		RETVM_IF(m_pWindow, false, "ViewManager is already initialized!");

		if (createWindow()) {
			return createNaviframe();
		}

		return false;
	}

	bool ViewManager::createWindow()
	{
		m_pWindow = elm_win_add(nullptr, nullptr, ELM_WIN_BASIC);
		RETVM_IF(!m_pWindow, false, "Failed to create window!");
		elm_win_indicator_mode_set(m_pWindow, ELM_WIN_INDICATOR_SHOW);
		elm_win_conformant_set(m_pWindow, EINA_TRUE);

		if (elm_win_wm_rotation_supported_get(m_pWindow)) {
			int rotationAngels[4] = { 0, 90, 180, 270 };
			elm_win_wm_rotation_available_rotations_set(m_pWindow, rotationAngels, 4);
			evas_object_smart_callback_add(m_pWindow, "wm,rotation,changed",
				EoSmartCb::make<ViewManager, &ViewManager::onOrientationChanged>(), this);
		}

		Evas_Object *bg = elm_bg_add(m_pWindow);
		RETVM_IF(!bg, false, "Failed to create bg!");
		evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		elm_win_resize_object_add(m_pWindow, bg);
		evas_object_show(bg);

		Evas_Object *conformant = elm_conformant_add(m_pWindow);
		RETVM_IF(!conformant, false, "Failed to create conformant!");
		evas_object_size_hint_weight_set(conformant, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		elm_win_resize_object_add(m_pWindow, conformant);
		evas_object_show(conformant);

		m_pRootLayout = elm_layout_add(m_pWindow);
		RETVM_IF(!m_pRootLayout, false, "Failed to create root layout!");
		elm_layout_theme_set(m_pRootLayout, "layout", "application", "default");
		evas_object_size_hint_weight_set(m_pRootLayout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		elm_object_content_set(conformant, m_pRootLayout);
		evas_object_show(m_pRootLayout);

		evas_object_show(m_pWindow);
		return true;
	}

	bool ViewManager::createNaviframe()
	{
		m_pNaviframe = elm_naviframe_add(m_pRootLayout);
		RETVM_IF(!m_pNaviframe, false, "Failed to create naviframe!");
		elm_naviframe_prev_btn_auto_pushed_set(m_pNaviframe, EINA_TRUE);
		elm_layout_content_set(m_pRootLayout, "elm.swallow.content", m_pNaviframe);

		eext_object_event_callback_add(m_pNaviframe, EEXT_CALLBACK_BACK,
			EoSmartCb::make<ViewManager, &ViewManager::onHwBackKeyPressed>(), this);

		eext_object_event_callback_add(m_pNaviframe, EEXT_CALLBACK_MORE,
			EoSmartCb::make<ViewManager, &ViewManager::onHwMenuKeyPressed>(), this);

		evas_object_smart_callback_add(m_pNaviframe, "transition,finished",
			EoSmartCb::make<ViewManager, &ViewManager::onTransitionFinished>(), this);

		return true;
	}

	NaviItem *ViewManager::createNaviItem(bool showAnimation)
	{
		Elm_Object_Item *newItem = nullptr;
		Elm_Object_Item *topItem = elm_naviframe_top_item_get(m_pNaviframe);

		if (topItem && !showAnimation) {
			newItem = elm_naviframe_item_insert_after(m_pNaviframe, topItem, NULL, NULL, NULL, NULL, NULL);
		} else {
			newItem = elm_naviframe_item_push(m_pNaviframe, NULL, NULL, NULL, NULL, NULL);
		}
		RETVM_IF(!newItem, nullptr, "Failed to create new navi item! EFL internal error");

		Evas_Object *backButton = createSoftwareBackButton();
		NaviItem *naviItem = WidgetItem::create<NaviItem>(newItem, backButton);

		return naviItem;
	}

	void ViewManager::attachView(BaseView *view, bool showAnimation)
	{
		/*First view should be created without animation if any*/
		if (m_ViewList.empty()) {
			showAnimation = false;
		}

		m_ViewList.push_front(view);
		view->setDestroyHandler(ViewDestroyHandler::wrap<ViewManager, &ViewManager::onViewDestroy>(this));

		if (showAnimation) {
			isTransitionState = true;

			m_pActivateJob = ecore_job_add([](void *data) {
					ViewManager * manager = static_cast<ViewManager *>(data);
					manager->m_ViewEventHandlers.invoke(VIEW_CHANGE_BEGIN);
					manager->m_pActivateJob = nullptr;
			}, this);

		} else {
			m_pActivateJob = ecore_job_add([](void *data) {
					ViewManager * manager = static_cast<ViewManager *>(data);
					manager->m_ViewEventHandlers.invoke(VIEW_CHANGE_BEGIN);
					manager->m_ViewEventHandlers.invoke(VIEW_CHANGE_END);
					manager->m_pActivateJob = nullptr;
			}, this);
		}
	}

	bool ViewManager::popView()
	{
		Elm_Object_Item *topItem = elm_naviframe_top_item_get(m_pNaviframe);
		RETVM_IF(!topItem, false, "View stack is empty, no need to pop!");

		m_ViewList.pop_front();
		if (!isTransitionState) {
			elm_naviframe_item_pop(m_pNaviframe);
			isTransitionState = true;
			m_ViewEventHandlers.invoke(VIEW_CHANGE_BEGIN);
		} else {
			elm_object_item_del(topItem);
			m_ViewEventHandlers.invoke(VIEW_CHANGE_BEGIN);
			m_ViewEventHandlers.invoke(VIEW_CHANGE_END);
		}

		return true;
	}

	bool ViewManager::addViewEventHandler(ViewEventHandler handler)
	{
		RETVM_IF(!handler.assigned(), false, "Invalid argument");

		m_ViewEventHandlers += handler;
		return true;
	}

	void ViewManager::removeViewEventHandler(ViewEventHandler handler)
	{
		m_ViewEventHandlers -= handler;
	}

	bool ViewManager::isTopView(BaseView *view)
	{
		RETVM_IF(!view, false, "Argument is nullptr");

		if(view == m_ViewList.front()) {
			return true;
		} else {
			return false;
		}
	}

	void ViewManager::onViewDestroy(BaseView * view)
	{
		RETM_IF(!view, "Callback data is not valid");

		if (view) {
			/*Naviframe item was removed silently without pop. Need to activate previous view immediately*/
			if (view == m_ViewList.front()) {
				m_ViewList.pop_front();
				m_ViewEventHandlers.invoke(VIEW_CHANGE_BEGIN);
				m_ViewEventHandlers.invoke(VIEW_CHANGE_END);
			} else {
				m_ViewList.remove(view);
			}
		}
	}

	Evas_Object *ViewManager::createSoftwareBackButton()
	{
		Evas_Object *backBtn = elm_button_add(m_pNaviframe);
		if (backBtn) {
			elm_object_style_set(backBtn, "naviframe/back_btn/default");
			evas_object_smart_callback_add(backBtn, "clicked",
				EoSmartCb::make<ViewManager, &ViewManager::onSoftwareBackKeyPressed>(), this);
		}

		return backBtn;
	}

	void ViewManager::onHwBackKeyPressed(Evas_Object *obj, void *event_info)
	{
		DBG("Hardware back button pressed");
		backButtonClickHandler();

	}

	void ViewManager::onHwMenuKeyPressed(Evas_Object *obj, void *event_info)
	{
		DBG("Hardware menu button pressed");
		menuButtonClickHandler();
	}

	void ViewManager::onSoftwareBackKeyPressed(Evas_Object *obj, void *event_info)
	{
		DBG("Software back button pressed");
		backButtonClickHandler();
	}

	void ViewManager::backButtonClickHandler()
	{
		if (!isTransitionState) {
			m_ViewEventHandlers.invoke(BACK_BUTTON_PRESSED);
		}
	}

	void ViewManager::menuButtonClickHandler()
	{
		if (!isTransitionState) {
			m_ViewEventHandlers.invoke(MENU_BUTTON_PRESSED);
		}
	}

	void ViewManager::onTransitionFinished(Evas_Object *obj, void *event_info)
	{
		m_ViewEventHandlers.invoke(VIEW_CHANGE_END);
		isTransitionState = false;
	}

	void ViewManager::onOrientationChanged(Evas_Object *obj, void *event_info)
	{
		m_ViewEventHandlers.invoke(ORIENTATION_CHANGED);
		isTransitionState = false;
	}

}
