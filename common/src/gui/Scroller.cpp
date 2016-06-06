/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "gui/Scroller.h"

namespace gui {

	bool Scroller::initialize(const Widget &parent)
	{
		RETVM_IF(!parent.getEvasObject(), false, "Failed to create Scroller: Parent in null");

		m_pEvasObject = elm_scroller_add(parent.getEvasObject());
		RETVM_IF(!m_pEvasObject, false, "Failed to create editfield: Internal error");

		evas_object_size_hint_align_set(m_pEvasObject, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_size_hint_weight_set(m_pEvasObject, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		return true;

	}

	void Scroller::setContent(const Widget &content)
	{
		elm_object_content_set(m_pEvasObject, content.getEvasObject());
	}

	void Scroller::setScrollbarPolicy(ScrollerPolicy widthScroll, ScrollerPolicy heightScroll)
	{
		elm_scroller_policy_set(m_pEvasObject, toElmSollerPolicy(widthScroll), toElmSollerPolicy(heightScroll));
	}

	void Scroller::bringIn(int x, int y, int width, int height)
	{
		elm_scroller_region_bring_in(m_pEvasObject, x, y, width, height);
	}

	void Scroller::regionShow(int x, int y, int width, int height)
	{
		elm_scroller_region_show(m_pEvasObject, x, y, width, height);
	}

	void Scroller::setBounceEffect(bool bounceHorizontal, bool bounceVertical)
	{
		Eina_Bool hBounceEffect= (bounceHorizontal) ? EINA_TRUE : EINA_FALSE;
		Eina_Bool wBounceEffect = (bounceVertical) ? EINA_TRUE : EINA_FALSE;

		elm_scroller_bounce_set(m_pEvasObject, hBounceEffect, wBounceEffect);
	}

	Elm_Scroller_Policy Scroller::toElmSollerPolicy(ScrollerPolicy policy)
	{
		switch (policy) {
		case SCROLLBAR_POLICY_AUTO:
			return ELM_SCROLLER_POLICY_AUTO;
		case SCROLLBAR_POLICY_ON:
			return ELM_SCROLLER_POLICY_ON;
		case SCROLLBAR_POLICY_OFF:
		default:
			return ELM_SCROLLER_POLICY_OFF;
		}
	}

}
