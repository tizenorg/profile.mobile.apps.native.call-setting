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

#include "View/Widgets/Widget.h"
#include "Utils/Logger.h"

#define WIDGET_DATA_KEY "View::WidgetData"

namespace View { namespace Widgets {

	Widget::Widget()
		:m_pEvasObject(nullptr)
	{
	}

	Widget::~Widget()
	{
		destroyWidget();
	}

	bool Widget::create(Evas_Object *parent)
	{
		RETVM_IF(!parent, false, "Failed to create Widget, parent is null");

		if (!m_pEvasObject) {
			setEvasObject(onCreate(parent));
			onCreated();
		}

		WARN("Widget is already created!");
		return true;
	}

	Evas_Object *Widget::getEvasObject() const
	{
		return m_pEvasObject;
	}

	Widget *Widget::getWidget(Evas_Object *object)
	{
		return (Widget *) evas_object_data_get(object, WIDGET_DATA_KEY);
	}

	void Widget::show()
	{
		if (m_pEvasObject) {
			evas_object_show(m_pEvasObject);
		}
	}

	void Widget::hide()
	{
		if (m_pEvasObject) {
			evas_object_hide(m_pEvasObject);
		}
	}

	void Widget::setEvasObject(Evas_Object *object)
	{
		m_pEvasObject = object;
		if (!evas_object_data_get(m_pEvasObject, WIDGET_DATA_KEY)) {
			evas_object_data_set(m_pEvasObject, WIDGET_DATA_KEY, this);
		}

		evas_object_event_callback_add(m_pEvasObject, EVAS_CALLBACK_DEL, onDelEvasObject, this);
		evas_object_event_callback_add(m_pEvasObject, EVAS_CALLBACK_FREE, onFreeEvasObject, this);
	}

	Evas_Object *Widget::resetEvasObject()
	{
		Evas_Object *object = m_pEvasObject;
		evas_object_event_callback_del_full(m_pEvasObject, EVAS_CALLBACK_DEL, onDelEvasObject, this);
		evas_object_event_callback_del_full(m_pEvasObject, EVAS_CALLBACK_FREE, onFreeEvasObject, this);
		m_pEvasObject = nullptr;
		return object;
	}

	void Widget::destroyWidget()
	{
		if (m_pEvasObject) {
			evas_object_del(resetEvasObject());
		}
	}

	void Widget::onDelEvasObject(void *data, Evas *e, Evas_Object *obj, void *event_info)
	{
		Widget *widget = static_cast<Widget*>(data);
		if (widget) {
			widget->onDestroy();
		}
	}

	void Widget::onFreeEvasObject(void *data, Evas *e, Evas_Object *obj, void *event_info)
	{
		Widget *widget = static_cast<Widget*>(data);
		if (widget) {
			widget->onDestroed();
			widget->resetEvasObject();
			delete widget;
		}
	}

} }
