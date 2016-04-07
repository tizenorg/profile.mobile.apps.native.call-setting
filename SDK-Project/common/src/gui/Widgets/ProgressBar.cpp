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

#include "gui/Widgets/ProgressBar.h"

namespace gui {

	bool ProgressBar::initialize(const Widget &parent, ProgressbarType type)
	{
		Evas_Object *eoParent = parent.getEvasObject();
		RETVM_IF(!eoParent, false, "Failed to create checkbox: Parent in null");

		m_pEvasObject = elm_progressbar_add(eoParent);
		RETVM_IF(!m_pEvasObject, false, "Failed to create layout: Internal error");

		elm_progressbar_value_set(m_pEvasObject, 0.0f);
		setStyle(type);
		return true;
	}

	void ProgressBar::setStyle(ProgressbarType type)
	{

		switch(type) {
		case PR_BAR_TYPE_CIRCLE_SMALL:
			elm_object_style_set(m_pEvasObject, "process_small");
			elm_progressbar_pulse(m_pEvasObject, EINA_TRUE);
			break;
		case PR_BAR_TYPE_CIRCLE_MID:
			elm_object_style_set(m_pEvasObject, "process_medium");
			elm_progressbar_pulse(m_pEvasObject, EINA_TRUE);
			break;
		case PR_BAR_TYPE_CIRCLE_LARGE:
			elm_object_style_set(m_pEvasObject, "process_large");
			elm_progressbar_pulse(m_pEvasObject, EINA_TRUE);
			break;
		case PR_BAR_TYPE_HORIZONTAL_PENDING:
			elm_object_style_set(m_pEvasObject, "pending");
			elm_progressbar_horizontal_set(m_pEvasObject, EINA_TRUE);
			elm_progressbar_pulse(m_pEvasObject, EINA_TRUE);
			break;
		case PR_BAR_TYPE_HORIZONTAL:
			elm_object_style_set(m_pEvasObject, "default");
			elm_progressbar_horizontal_set(m_pEvasObject, EINA_TRUE);
			elm_progressbar_value_set(m_pEvasObject, 0.0f);
			elm_progressbar_pulse(m_pEvasObject, EINA_FALSE);
			break;
		}
	}

	bool ProgressBar::setValue(double val)
	{
		RETVM_IF(val>1.0f || val < 0.0f, false, "Invalid data!");

		elm_progressbar_value_set(m_pEvasObject, val);
		return true;
	}

	double ProgressBar::getValue()
	{
		return elm_progressbar_value_get(m_pEvasObject);
	}

	void ProgressBar::setText(const char *textPart, const char *text)
	{
		RETM_IF(!textPart || !text, "Invalid args!");
		elm_object_part_text_set(m_pEvasObject, textPart, _(text));
	}
}
