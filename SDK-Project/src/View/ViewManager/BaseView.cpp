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

#include "View/ViewManager/BaseView.h"
#include "Utils/Logger.h"

namespace View {

	BaseView::BaseView(Widgets::NaviItem *naviItem) :
		m_pViewLayout(nullptr),
		m_pNaviItem(naviItem),
		m_OnMenuPressedCb(nullptr),
		m_OnBackPressedCb(nullptr)
	{
	}

	BaseView::~BaseView()
	{
	}

	template <class ViewClass>
	ViewClass *BaseView::create(Widgets::NaviItem *naviItem)
	{
		RETVM_IF(!naviItem, nullptr, "Invalid arguments!");

		ViewClass *viewInstance = new ViewClass(naviItem);

		if (!viewInstance->createBaseLayout() || viewInstance->createViewContent()) {
			delete viewInstance;
		}

		viewInstance->m_pNaviItem->setContent(viewInstance->m_pViewLayout);
		viewInstance->m_pViewLayout->setDestroyCallback(&BaseView::onViewLayoutDestroy);

		return viewInstance;
	}

	void BaseView::destroy(BaseView *view)
	{
		RETM_IF(!view, "Invalid arguments!");

		if (view->m_pViewLayout) {
			Widgets::WidgetItem::destroy(view->m_pNaviItem);
		}

	}

	bool BaseView::createBaseLayout()
	{
		RETVM_IF(!m_pNaviItem, false, "Error: View is not attached to naviframe!");

		Widgets::WidgetWrapper *parent = m_pNaviItem->getParent();
		m_pViewLayout = Widgets::Widget::create<Widgets::Layout>(parent, "layout", "application", "default");
		delete parent;
		return  m_pViewLayout ? true : false;
	}

	void BaseView::onNaviItemDestroy()
	{
		if (m_pViewLayout) {
			Widgets::Widget::destroy(m_pViewLayout);
			m_pViewLayout = nullptr;
			m_pNaviItem = nullptr;
		}

		delete this;
	}

	void BaseView::onViewLayoutDestroy()
	{
		m_pViewLayout = nullptr;
	}

	void BaseView::onMenuKeyPressed()
	{
		if (m_OnMenuPressedCb) {
			m_OnMenuPressedCb();
		}
	}

	void BaseView::onBackKeyPressed()
	{
		if (m_OnBackPressedCb) {
			m_OnBackPressedCb();
		}
	}

	bool BaseView::setMenuKeyPressedCb(std::function<void()> callback)
	{
		if(callback && !m_OnBackPressedCb) {
			m_OnBackPressedCb = std::move(callback);
			return true;
		}

		return false;
	}

	bool BaseView::setBackKeyPressedCb(std::function<void()> callback)
	{
		if(callback && !m_OnBackPressedCb) {
			m_OnBackPressedCb = std::move(callback);
			return true;
		}

		return false;
	}

}
