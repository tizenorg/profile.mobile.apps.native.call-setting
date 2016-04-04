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

#include "gui/Widgets/Checkbox.h"

namespace gui {

	Checkbox::Checkbox() :
		m_checkHandler()
	{
	}

	Checkbox::~Checkbox()
	{
	}

	bool Checkbox::initialize(const Widget &parent, CheckboxType type)
	{
		Evas_Object *checkboxParent = parent.getEvasObject();
		RETVM_IF(!checkboxParent, false, "Failed to create checkbox: Parent in null");

		m_pEvasObject = elm_check_add(checkboxParent);
		RETVM_IF(!m_pEvasObject, false, "Failed to create layout: Internal error");

		switch (type) {
		case CHECKBOX_SWITCHER :
			elm_object_style_set(m_pEvasObject, "on&off");
			break;
		case CHECKBOX_STAR :
			elm_object_style_set(m_pEvasObject, "favorite");
			break;
		case CHECKBOX_DEFAULT:
		default:
			elm_object_style_set(m_pEvasObject, "default");
			break;
		}

		elm_check_state_set(m_pEvasObject, EINA_FALSE);
		evas_object_repeat_events_set(m_pEvasObject, EINA_FALSE);
		evas_object_propagate_events_set(m_pEvasObject, EINA_FALSE);
		evas_object_smart_callback_add(m_pEvasObject, "changed",
			EoSmartCb::make<Checkbox, &Checkbox::onChecked>(), this);

		return true;
	}

	bool Checkbox::isChecked()
	{
		if (elm_check_state_get(m_pEvasObject)) {
			return true;
		} else {
			return false;
		}
	}

	void Checkbox::setChecked(bool checkedState)
	{
		if (checkedState) {
			elm_check_state_set(m_pEvasObject, EINA_TRUE);
		} else {
			elm_check_state_set(m_pEvasObject, EINA_FALSE);
		}
	}

	void Checkbox::setCheckHandler(NotifyHandler handler)
	{
		m_checkHandler = handler;
	}

	void Checkbox::onChecked(Evas_Object *obj, void *event_info)
	{
		if (m_checkHandler.assigned()) {
			m_checkHandler();
		}
	}
}
