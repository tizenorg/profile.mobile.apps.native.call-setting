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

#include "View/Widgets/Genlist.h"
#include "View/Widgets/GenlistItem.h"

namespace Widgets {

	bool Genlist::initialize(const Widget &parent)
	{
		Evas_Object *listParent = parent.getEvasObject();
		RETVM_IF(!listParent, false, "Failed to create genlist: Parent in null");

		m_pEvasObject = elm_genlist_add(listParent);
		RETVM_IF(!m_pEvasObject, false, "Failed to create genlist: Internal error");

		elm_genlist_mode_set(m_pEvasObject, ELM_LIST_COMPRESS);
		elm_genlist_homogeneous_set(m_pEvasObject, EINA_TRUE);
		elm_scroller_policy_set(m_pEvasObject, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
		elm_scroller_content_min_limit(m_pEvasObject, EINA_FALSE, EINA_TRUE);

		evas_object_smart_callback_add(m_pEvasObject, "selected",
				makeEvasSmartCb(Genlist, &Genlist::onItemSelected), this);
		evas_object_smart_callback_add(m_pEvasObject, "realized",
				makeEvasSmartCb(Genlist, &Genlist::onItemRealized), this);
		evas_object_smart_callback_add(m_pEvasObject, "unrealized",
				makeEvasSmartCb(Genlist, &Genlist::onItemUnrealized), this);

		return true;
	}

	void Genlist::onItemSelected(Evas_Object *obj, void *eventInfo)
	{
		Elm_Object_Item *objectItem = static_cast<Elm_Object_Item *>(eventInfo);
		RETM_IF(!objectItem, "Callback data is invalid!");

		GenlistItem *item = (GenlistItem *) elm_object_item_data_get(objectItem);
		if (item) {
			item->onSelected();
		}
	}

	void Genlist::onItemRealized(Evas_Object *obj, void *eventInfo)
	{
		Elm_Object_Item *objectItem = static_cast<Elm_Object_Item *>(eventInfo);
		RETM_IF(!objectItem, "Callback data is invalid!");

		GenlistItem *item = (GenlistItem *) elm_object_item_data_get(objectItem);
		if (item) {
			item->onRealized();
		}
	}

	void Genlist::onItemUnrealized(Evas_Object *obj, void *eventInfo)
	{
		Elm_Object_Item *objectItem = static_cast<Elm_Object_Item *>(eventInfo);
		RETM_IF(!objectItem, "Callback data is invalid!");

		GenlistItem *item = (GenlistItem *) elm_object_item_data_get(objectItem);
		if (item) {
			item->onUnrealized();
		}
	}

	void Genlist::update()
	{
		Eina_List *list = elm_genlist_realized_items_get(getEvasObject());
		Eina_List *node = nullptr;
		void *item = nullptr;

		EINA_LIST_FOREACH(list, node, item) {
			elm_genlist_item_update((Elm_Object_Item *) item);
		}

		eina_list_free(list);
	}

}
