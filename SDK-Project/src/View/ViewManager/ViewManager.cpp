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

#include <efl_extension.h>

#include "View/ViewManager/ViewManager.h"
#include "View/ViewManager/BaseView.h"
#include "View/ViewManager/ViewEventListener.h"
#include "View/Widgets/NaviItem.h"
#include "Utils/Logger.h"

namespace View {
	ViewManager::ViewManager() :
		m_pWindow(nullptr),
		m_pRootLayout(nullptr),
		m_pNaviframe(nullptr),
		isTransitionState(false),
		m_pActiveView(nullptr)
	{
	}

	ViewManager::~ViewManager()
	{
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
		createWindow();
		createNaviframe();
		return true;
	}

	bool ViewManager::createWindow()
	{
		Evas_Object *m_pWindow = elm_win_add(nullptr, nullptr, ELM_WIN_BASIC);
		RETVM_IF(!m_pWindow, false, "Failed to create window!");
		elm_win_indicator_mode_set(m_pWindow, ELM_WIN_INDICATOR_SHOW);
		elm_win_conformant_set(m_pWindow, EINA_TRUE);

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
			[](void *data, Evas_Object *obj, void *event_info) {
				return static_cast<ViewManager *>(data)->onBackKeyPressed();
			}, this);

		eext_object_event_callback_add(m_pNaviframe, EEXT_CALLBACK_MORE,
			[](void *data, Evas_Object *obj, void *event_info) {
				return static_cast<ViewManager *>(data)->onMenuKeyPressed();
			}, this);

		evas_object_smart_callback_add(m_pNaviframe, "transition,finished",
			[](void *data, Evas_Object *obj, void *event_info) {
				return static_cast<ViewManager *>(data)->onTransitionFinished();
			}, this);

		return true;
	}

	template <class ViewType>
	ViewType* ViewManager::pushView(ViewManager *manager, bool showAnimation)
	{
		RETVM_IF(!manager, nullptr, "Invalid args");

		Elm_Object_Item *newItem = nullptr;
		Elm_Object_Item *topItem = elm_naviframe_top_item_get(manager->m_pNaviframe);

		if (topItem && !showAnimation) {
			newItem = elm_naviframe_item_insert_after(manager->m_pNaviframe, topItem, NULL, NULL, NULL, NULL, NULL);
		} else {
			newItem = elm_naviframe_item_push(manager->m_pNaviframe, NULL, NULL, NULL, NULL, NULL);
		}

		RETVM_IF(!newItem, nullptr, "Failed to create new navi item! EFL internal error");

		Widgets::NaviItem *naviItem = Widgets::WidgetItem::create<Widgets::NaviItem>(newItem);
		RETVM_IF(!naviItem, nullptr, "Internal error!");
		naviItem->setDestroyCallback(std::bind(&ViewManager::onNaviItemDestroy, manager, std::placeholders::_1));

		ViewType *view = BaseView::create<ViewType>(naviItem);
		RETVM_IF(!view, nullptr, "Failed to create view!");
		naviItem->setData(view);
		view->show();

		if (showAnimation) {
			manager->isTransitionState = true;
			manager->m_pActiveView = nullptr;
			for(auto listener : manager->m_ListenersSet) {
				listener->onViewChangeBegin();
			}

		} else {
			manager->m_pActiveView = view;
			for(auto listener : manager->m_ListenersSet) {
				listener->onViewChangeBegin();
				listener->onViewChangeEnd();
			}
		}

		return view;
	}

	bool ViewManager::popView()
	{
		RETVM_IF(isTransitionState, false, "Transition in progress, pop is not allowed now");

		Elm_Object_Item *topItem = elm_naviframe_top_item_get(m_pNaviframe);
		RETVM_IF(!topItem, false, "View stack is empty, no need to pop!");

		if (topItem == elm_naviframe_bottom_item_get(m_pNaviframe)) {
			ERR("Last view in stack! unable to make pop");
			return false;
		}

		elm_naviframe_item_pop(m_pNaviframe);
		m_pActiveView = nullptr;
		isTransitionState = true;
		for(auto listener : m_ListenersSet) {
			listener->onViewChangeBegin();
		}

		return true;
	}

	bool ViewManager::addEventListener(ViewEventListener *listener)
	{
		RETVM_IF(!listener, false, "Invalid argument");

		auto result = m_ListenersSet.insert(listener);
		return result.second;
	}

	void ViewManager::removeEventListener(ViewEventListener *listener)
	{
		RETM_IF(!listener, "Invalid argument");
		m_ListenersSet.erase(listener);
	}

	bool ViewManager::isTopView(BaseView *view)
	{
		RETVM_IF(!view, false, "Argument is nullptr");
		if(view == getTopView()) {
			return true;
		} else {
			return false;
		}
	}

	void ViewManager::onNaviItemDestroy(Widgets::WidgetItem *widgetItem)
	{
		RETM_IF(!widgetItem, "Callback data is not valid");

		BaseView *view = static_cast<BaseView*>(widgetItem->getData());
		if (view) {
			/*Naviframe item was removed silently without pop. Need to activate previous view immediately*/
			if (view == m_pActiveView) {
				for(auto listener : m_ListenersSet) {
					listener->onViewChangeBegin();
					listener->onViewChangeEnd();
				}
			}
			m_pActiveView = getTopView();
			view->onNaviItemDestroy();
		}
	}

	BaseView *ViewManager::getTopView()
	{
		Elm_Object_Item *topItem = elm_naviframe_top_item_get(m_pNaviframe);
		Widgets::NaviItem *naviItem = static_cast<Widgets::NaviItem*>(elm_object_item_data_get(topItem));
		if (naviItem) {
			return static_cast<BaseView*>(naviItem->getData());
		} else {
			return nullptr;
		}
	}

	void ViewManager::onBackKeyPressed()
	{
		RETM_IF(isTransitionState, "Events are blocked");

		if (elm_naviframe_top_item_get(m_pNaviframe) == elm_naviframe_bottom_item_get(m_pNaviframe)) {
			elm_win_lower(m_pWindow);
			return;
		}

		BaseView *view = getTopView();
		if (view) {
			view->onBackKeyPressed();
		}
	}

	void ViewManager::onMenuKeyPressed()
	{
		RETM_IF(isTransitionState, "Events are blocked");

		BaseView *view = getTopView();
		if (view) {
			view->onMenuKeyPressed();
		}
	}

	void ViewManager::onTransitionFinished()
	{
		for(auto listener : m_ListenersSet) {
			listener->onViewChangeEnd();
		}
		m_pActiveView = getTopView();
		isTransitionState = false;
	}

}
