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

#include "gui/OptionMenuPopup.h"

namespace gui {

	bool OptionMenuPopup::initialize(const Widget &parent)
	{
		m_pEvasObject = elm_ctxpopup_add(elm_object_top_widget_get(parent.getEvasObject()));
		RETVM_IF(!m_pEvasObject, false, "Failed to create option menu popup, unknown error");

		elm_object_style_set(m_pEvasObject, "more/default");

		evas_object_smart_callback_add(m_pEvasObject, "dismissed",
				EoSmartCb::make<OptionMenuPopup, &OptionMenuPopup::onPopupDismissed>(), this);

		eext_object_event_callback_add(m_pEvasObject, EEXT_CALLBACK_BACK, eext_ctxpopup_back_cb, nullptr);
		eext_object_event_callback_add(m_pEvasObject, EEXT_CALLBACK_MORE, eext_ctxpopup_back_cb, nullptr);

		return true;
	}

	bool OptionMenuPopup::addItem(NotiHandler optionSelectHandler, const util::TString &text, const Widget *icon)
	{
		Elm_Object_Item *item = nullptr;
		if (icon && icon->getEvasObject()) {
			item = elm_ctxpopup_item_append(m_pEvasObject, ne(text), icon->getEvasObject(),
					EoSmartCb::make<OptionMenuPopup, &OptionMenuPopup::onItemSelected>(), this);
		} else {
			item = elm_ctxpopup_item_append(m_pEvasObject, ne(text), nullptr,
					EoSmartCb::make<OptionMenuPopup, &OptionMenuPopup::onItemSelected>(), this);
		}

		if (text.isTranslatable()) {
			if (text.hasDomain()) {
				elm_object_item_domain_part_text_translatable_set(item, nullptr, text.getDomain(), EINA_TRUE);
			} else {
				elm_object_item_text_translatable_set(item, EINA_TRUE);
			}
		}

		m_itemSelectHandlers.push_back(std::make_pair(item, optionSelectHandler));
		return true;
	}

	void OptionMenuPopup::show()
	{
		Evas_Coord y = 0, h = 0;
		evas_object_geometry_get(elm_object_top_widget_get(m_pEvasObject), nullptr, &y, nullptr, &h);

		evas_object_move(m_pEvasObject, 0, y + h);
		evas_object_show(m_pEvasObject);
	}

	void OptionMenuPopup::hide()
	{
		elm_ctxpopup_dismiss(m_pEvasObject);
	}

	void OptionMenuPopup::onItemSelected(Evas_Object *obj, void *eventInfo)
	{
		Elm_Object_Item *item = static_cast<Elm_Object_Item *>(eventInfo);
		RETM_IF(!item, "Callback data is nullptr!");

		auto it = std::find_if(m_itemSelectHandlers.begin(), m_itemSelectHandlers.end(),
				[&item](std::pair<Elm_Object_Item *, NotiHandler> &itemPair) {
					return itemPair.first == item;
				}
		);

		if (it != m_itemSelectHandlers.end()) {
			NotiHandler itemSelectHandler = it->second;
			if (itemSelectHandler.assigned()) {
				itemSelectHandler();
			}
		}

		elm_ctxpopup_dismiss(m_pEvasObject);
	}

	void OptionMenuPopup::onPopupDismissed(Evas_Object *obj, void *eventInfo)
	{
		Widget::destroy(this);
	}
}
