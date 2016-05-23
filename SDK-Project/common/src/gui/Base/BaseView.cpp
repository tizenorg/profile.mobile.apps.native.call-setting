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

#include "gui/Base/BaseView.h"

namespace gui {

	BaseView::BaseView(NaviItem *naviItem) :
		m_pNaviItem(naviItem),
		m_pViewLayout(nullptr),
		m_isDestroying(false)
	{
	}

	BaseView::~BaseView()
	{
		WidgetItem::destroy(m_pNaviItem);

		if (m_destroyHandler.assigned()) {
			m_destroyHandler(this);
		}
	}

	bool BaseView::initialize()
	{
		RETVM_IF(!m_pNaviItem, false, "Error: View is not attached to naviframe!");

		Evas_Object *parent = m_pNaviItem->getParent();
		m_pViewLayout = Widget::create<Layout>(WidgetWrapper(parent), "layout", "application", "default");
		RETVM_IF(!m_pViewLayout, false, "Error: base view layout create failed!");

		m_pNaviItem->setContent(*m_pViewLayout);

		m_pNaviItem->setDestroyHandler(NotiHandler::wrap<BaseView, &BaseView::onNaviItemDestroy>(this));
		m_pViewLayout->setDestroyHandler(NotiHandler::wrap<BaseView, &BaseView::onViewLayoutDestroy>(this));

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

	bool BaseView::setViewContent(Widget &content)
	{
		RETVM_IF(!m_pViewLayout, false, "Error: base view layout is empty!");

		return m_pViewLayout->setContent("elm.swallow.content", content);
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

	void BaseView::enableInputEvents()
	{
		evas_object_freeze_events_set(m_pViewLayout->getEvasObject(), EINA_FALSE);
	}

	void BaseView::disableInputEvents()
	{
		evas_object_freeze_events_set(m_pViewLayout->getEvasObject(), EINA_TRUE);
	}

	Layout &BaseView::getLayout()
	{
		return *m_pViewLayout;
	}

	NaviItem &BaseView::getItem()
	{
		return *m_pNaviItem;
	}

}
