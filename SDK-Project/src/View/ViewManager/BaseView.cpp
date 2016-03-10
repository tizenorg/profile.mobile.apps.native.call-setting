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
#include "Utils/Common.h"

namespace View {

	BaseView::BaseView(NaviItem *naviItem) :
		m_pViewLayout(nullptr),
		m_pNaviItem(naviItem),
		m_destroyHandler(),
		m_isDestroying(false)
	{
	}

	BaseView::~BaseView()
	{
		if (m_pNaviItem) {
			WidgetItem::destroy(m_pNaviItem);
		}

		if (m_destroyHandler.assigned()) {
			m_destroyHandler(this);
		}
	}

	bool BaseView::initialize()
	{
		if (!createBaseLayout() || !createViewContent()) {
			ERR("Failed to create view content");
			return false;
		}

		m_pNaviItem->setContent(*m_pViewLayout);
		m_pNaviItem->setDestroyHandler(makeHandler(NotifyHandler, BaseView, &BaseView::onNaviItemDestroy, this));
		m_pViewLayout->setDestroyHandler(makeHandler(NotifyHandler, BaseView, &BaseView::onViewLayoutDestroy, this));

		return true;
	}

	void BaseView::destroy(BaseView *view)
	{
		RETM_IF(!view, "Invalid arguments!");

		view->m_isDestroying = true;
		delete view;
	}

	void BaseView::setDestroyHandler(ViewDestroyHandler handler)
	{
		m_destroyHandler = handler;
	}

	bool BaseView::createBaseLayout()
	{
		RETVM_IF(!m_pNaviItem, false, "Error: View is not attached to naviframe!");

		Evas_Object *parent = m_pNaviItem->getParent();
		m_pViewLayout = Widget::create<Layout>(WidgetWrapper(parent), "layout", "application", "default");
		return  m_pViewLayout ? true : false;
	}

	void BaseView::onNaviItemDestroy()
	{
		m_pNaviItem = nullptr;
		if (m_pViewLayout) {
			Widget::destroy(m_pViewLayout);
			m_pViewLayout = nullptr;
		}

		if (!m_isDestroying) {
			delete this;
		}
	}

	void BaseView::onViewLayoutDestroy()
	{
		m_pViewLayout = nullptr;
	}

}
