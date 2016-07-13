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

#include "gui/Genlist.h"
#include "gui/GenlistItem.h"

namespace gui {

	bool Genlist::initialize(const Widget &parent)
	{
		Evas_Object *listParent = parent.getEvasObject();
		RETVM_IF(!listParent, false, "Failed to create genlist: Parent in null");

		m_pEvasObject = elm_genlist_add(listParent);
		RETVM_IF(!m_pEvasObject, false, "Failed to create genlist: Internal error");

		evas_object_size_hint_weight_set(m_pEvasObject, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(m_pEvasObject, EVAS_HINT_FILL, EVAS_HINT_FILL);
		elm_genlist_mode_set(m_pEvasObject, ELM_LIST_COMPRESS);
		elm_genlist_homogeneous_set(m_pEvasObject, EINA_TRUE);

		evas_object_smart_callback_add(m_pEvasObject, "selected",
			EoSmartCb::make<Genlist, &Genlist::onItemSelected>(), this);

		return true;
	}

	GenlistItem *Genlist::toGenlistItem(void *eventInfo)
	{
		RETVM_IF(!eventInfo, nullptr, "Event Info data is null!");

		Elm_Object_Item *objectItem = static_cast<Elm_Object_Item *>(eventInfo);
		void *data = elm_object_item_data_get(objectItem);
		return GenlistItem::fromData(data);
	}

	void Genlist::onItemSelected(Evas_Object *obj, void *eventInfo)
	{
		GenlistItem *item = toGenlistItem(eventInfo);
		if (item) {
			item->onSelected();
		} else {
			ERR("Invalid data!");
		}
	}

	void Genlist::update()
	{
		elm_genlist_realized_items_update(m_pEvasObject);
	}

	void Genlist::clear()
	{
		elm_genlist_clear(m_pEvasObject);
	}

	void Genlist::setHomogeneousMode(bool isHomogenModeEnabled)
	{
		elm_genlist_homogeneous_set(m_pEvasObject, TO_EINA_BOOL(isHomogenModeEnabled));
	}

	void Genlist::setContentMinLimit(bool w, bool h)
	{
		elm_scroller_content_min_limit(m_pEvasObject, TO_EINA_BOOL(w), TO_EINA_BOOL(h));
	}

}
