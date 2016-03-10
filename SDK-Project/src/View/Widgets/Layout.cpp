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


namespace View { namespace Widgets {

	bool Layout::initialize(Widget *parent, const char *className, const char *groupName, const char *styleName)
	{
		RETVM_IF(!className || !groupName || !styleName, false, "Invalid parameters");

		m_pEvasObject = elm_layout_add(parent->getEvasObject());
		RETVM_IF(!m_pEvasObject, false, "Failed to create layout: Internal error");

		elm_layout_theme_set(m_pEvasObject, className, groupName, styleName);
		evas_object_size_hint_align_set(m_pEvasObject, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_size_hint_weight_set(m_pEvasObject, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		return true;

	}

	bool Layout::initialize(Widget *parent, const char *edcFilePath, const char *groupName)
	{
		RETVM_IF(!edcFilePath || !groupName, false, "Invalid parameters");

		m_pEvasObject = elm_layout_add(parent->getEvasObject());
		RETVM_IF(!m_pEvasObject, false, "Failed to create layout: Internal error");

		elm_layout_file_set(m_pEvasObject, edcFilePath, groupName);
		evas_object_size_hint_align_set(m_pEvasObject, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_size_hint_weight_set(m_pEvasObject, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

		return true;
	}

	bool Layout::setContent(Widget *content, char *swallowPart)
	{
		RETVM_IF(!content || !content->getEvasObject(), false, "Invalid parameters");
		if (elm_layout_content_set(m_pEvasObject, swallowPart, content->getEvasObject())) {
			evas_object_show(content->getEvasObject());
			return true;
		} else {
			ERR("Failed to set layout content!");
			return false;
		}
	}

	bool Layout::unsetContent(char *swallowPart)
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

	bool Layout::setText(const char *text, char *partPart)
	{
		RETVM_IF(!text, false, "Invalid parameters");
		return elm_layout_text_set(m_pEvasObject, partPart, text);
	}
} }
