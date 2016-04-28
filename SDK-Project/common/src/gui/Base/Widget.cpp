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
		m_tag(0)
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

	void Widget::setText(const util::TString &text)
	{
		if (text.isTranslatable()) {
			if (text.hasDomain()) {
				elm_object_domain_translatable_text_set(m_pEvasObject, text.getDomain(), ne(text));
			} else {
				elm_object_translatable_text_set(m_pEvasObject, ne(text));
			}
		} else {
			elm_object_text_set(m_pEvasObject, ne(text));
		}
	}

	void Widget::setPartText(const char *partName, const util::TString &text)
	{
		RETM_IF(!partName, "partName is null");

		if (text.isTranslatable()) {
			if (text.hasDomain()) {
				elm_object_domain_translatable_part_text_set(m_pEvasObject, partName, text.getDomain(), ne(text));
			} else {
				elm_object_translatable_part_text_set(m_pEvasObject, partName, ne(text));
			}
		} else {
			elm_object_part_text_set(m_pEvasObject, partName, ne(text));
		}
	}

	void Widget::setContent(Widget *pContent)
	{
		elm_object_content_set(m_pEvasObject, (pContent ? pContent->getEvasObject() : nullptr));
	}

	void Widget::setPartContent(const char *partName, Widget *pContent)
	{
		RETM_IF(!partName, "partName is null");

		elm_object_part_content_set(m_pEvasObject, partName, (pContent ? pContent->getEvasObject() : nullptr));
	}

	void Widget::setDisabled(bool disabledState)
	{
		elm_object_disabled_set(m_pEvasObject, TO_EINA_BOOL(disabledState));
	}

	void Widget::setFocus(bool focused)
	{
		elm_object_focus_set(m_pEvasObject, TO_EINA_BOOL(focused));
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

	void Widget::setTag(int tag)
	{
		m_tag = tag;
	}

	int Widget::getTag() const
	{
		return m_tag;
	}
}
