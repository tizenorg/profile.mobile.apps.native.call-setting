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

#include "View/Widgets/Layout.h"
#include "Utils/Logger.h"

namespace Widgets {

	bool Layout::initialize(const Widget &parent, const char *className, const char *groupName, const char *styleName)
	{
		Evas_Object *layoutParent = parent.getEvasObject();
		RETVM_IF(!layoutParent, false, "Failed to create layout: Parent in null");

		m_pEvasObject = elm_layout_add(layoutParent);
		RETVM_IF(!m_pEvasObject, false, "Failed to create layout: Internal error");

		if (!themeSet(className, groupName, styleName)) {
			ERR("Failed to set layout theme");
			return false;
		}

		evas_object_size_hint_align_set(m_pEvasObject, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_size_hint_weight_set(m_pEvasObject, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

		return true;
	}

	bool Layout::initialize(const Widget &parent, const char *edcFilePath, const char *groupName)
	{
		Evas_Object *layoutParent = parent.getEvasObject();
		RETVM_IF(!layoutParent, false, "Failed to create layout: Parent in null");

		m_pEvasObject = elm_layout_add(layoutParent);
		RETVM_IF(!m_pEvasObject, false, "Failed to create layout: Internal error");

		if (!styleSet(edcFilePath, groupName)) {
			ERR("Failed to set style");
			return false;
		}

		evas_object_size_hint_align_set(m_pEvasObject, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_size_hint_weight_set(m_pEvasObject, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

		return true;
	}

	bool Layout::setContent(const char *swallowPart, const Widget &content)
	{
		RETVM_IF(!content.getEvasObject(), false, "Invalid parameters");

		if (elm_layout_content_set(m_pEvasObject, swallowPart, content.getEvasObject())) {
			evas_object_show(content.getEvasObject());
			return true;
		} else {
			ERR("Failed to set layout content!");
			return false;
		}
	}

	bool Layout::unsetContent(const char *swallowPart)
	{
		Evas_Object *content = elm_layout_content_unset(m_pEvasObject, swallowPart);
		if (content) {
			evas_object_hide(content);
			return true;
		} else {
			ERR("Failed to unset layout content!");
			return false;
		}
	}

	bool Layout::setText(const char *textPart, const char *text)
	{
		RETVM_IF(!text, false, "Invalid parameters");
		return elm_layout_text_set(m_pEvasObject, textPart, text);
	}

	bool Layout::themeSet(const char *className, const char *groupName, const char *styleName)
	{
		RETVM_IF(!className || !groupName || !styleName, false, "Invalid parameters");
		return elm_layout_theme_set(m_pEvasObject, className, groupName, styleName) ? true : false;
	}

	bool Layout::styleSet(const char *edjFilePath, const char *groupName)
	{
		RETVM_IF(!edjFilePath || !groupName, false, "Invalid parameters");
		return elm_layout_file_set(m_pEvasObject, edjFilePath, groupName) ? true : false;
	}
}
