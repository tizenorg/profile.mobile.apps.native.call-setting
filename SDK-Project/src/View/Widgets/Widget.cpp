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

namespace Widgets {

	Widget::Widget() :
		m_pEvasObject(nullptr),
		m_isDestroying(false),
		m_isOwner(false),
		m_OnDestroyCb(nullptr)
	{
	}

	Widget::~Widget()
	{
		if (m_isOwner) {
			evas_object_del(m_pEvasObject);
		}

		if (m_OnDestroyCb) {
			m_OnDestroyCb();
		}
	}

	void Widget::destroy(Widget *widget)
	{
		if (widget) {
			widget->m_isDestroying = true;
			delete widget;
		}
	}

	void Widget::onEvasObjectDelBase()
	{
		m_pEvasObject = nullptr;
		onEvasObjectDel();
		if (!m_isDestroying) {
			delete this;
		}
	}

	bool Widget::setDestroyCallback(std::function<void()> callback)
	{
		if (callback && !m_OnDestroyCb) {
			m_OnDestroyCb = std::move(callback);
			return true;
		}

		return false;
	}

	void Widget::unsetDestroyCallback()
	{
		if (m_OnDestroyCb) {
			m_OnDestroyCb = nullptr;
		}
	}

	Evas_Object *Widget::getEvasObject()
	{
		return m_pEvasObject;
	}

}
