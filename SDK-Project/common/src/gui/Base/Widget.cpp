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

#include "gui/Base/Widget.h"

namespace gui {

	Widget::Widget() :
		m_pEvasObject(nullptr),
		m_destroyHandler()
	{
	}

	Widget::~Widget()
	{
		if (m_destroyHandler.assigned()) {
			m_destroyHandler();
		}
	}

	void Widget::destroy(Widget *widget)
	{
		if (!widget) {
			return;
		}

		if (widget->m_pEvasObject) {
			Evas_Object *eo = widget->m_pEvasObject;
			widget->m_pEvasObject = nullptr;
			evas_object_del(eo);
		} else {
			delete widget;
		}
	}

	void Widget::show()
	{
		evas_object_show(m_pEvasObject);
	}

	void Widget::hide()
	{
		evas_object_hide(m_pEvasObject);
	}

	void Widget::onEvasObjectDelBase(Evas *e, Evas_Object *obj, void *event_info)
	{
		m_pEvasObject = nullptr;
		onEvasObjectDel();
		delete this;
	}

	bool Widget::prepare(Widget *widget)
	{
		RETVM_IF(!widget->m_pEvasObject, false, "Object is null");

		evas_object_event_callback_add(widget->m_pEvasObject, EVAS_CALLBACK_DEL,
				EvasEventCb::make<Widget, &Widget::onEvasObjectDelBase>(), widget);

		return true;
	}

	void Widget::setDestroyHandler(NotiHandler handler)
	{
		m_destroyHandler = handler;
	}

	Evas_Object *Widget::getEvasObject() const
	{
		return m_pEvasObject;
	}
}
