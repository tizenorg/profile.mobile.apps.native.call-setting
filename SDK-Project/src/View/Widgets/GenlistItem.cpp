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

	GenlistItem::GenlistItem() :
		WidgetItem()
	{
	}

	GenlistItem::~GenlistItem()
	{
	}

	void GenlistItem::update()
	{
		if (m_pEOItem) {
			elm_genlist_item_update(m_pEOItem);
		}
	}

	void GenlistItem::update(const char *parts, int updateFlag)
	{
		RETM_IF(!parts, "Invalid args");
		RETM_IF(!updateFlag, "Invalid args");

		if (m_pEOItem) {
				int itemFieldType = 0;
				if (updateFlag & GL_PART_TYPE_TEXT) {
					itemFieldType |= ELM_GENLIST_ITEM_FIELD_TEXT;
				}

				if (updateFlag & GL_PART_TYPE_CONTENT) {
					itemFieldType |= ELM_GENLIST_ITEM_FIELD_CONTENT;
				}

				if (updateFlag & GL_PART_TYPE_STATE) {
					itemFieldType |= ELM_GENLIST_ITEM_FIELD_STATE;
				}

				if (itemFieldType) {
					elm_genlist_item_fields_update(m_pEOItem, parts,
							static_cast<Elm_Genlist_Item_Field_Type>(itemFieldType));
				}
		}
	}

	void GenlistItem::setSelectHandler(NotifyHandler handler)
	{
		m_selectHandler = handler;
	}

	void GenlistItem::setSelectionMode(ItemSelectionMode mode)
	{
		RETM_IF(!m_pEOItem, "Object is null");

		switch(mode) {
		case GENLIST_ITEM_SELECT_MODE_ONCE:
			elm_genlist_item_select_mode_set(m_pEOItem, ELM_OBJECT_SELECT_MODE_DEFAULT);
			break;
		case GENLIST_ITEM_SELECT_MODE_ALWAYS:
			elm_genlist_item_select_mode_set(m_pEOItem, ELM_OBJECT_SELECT_MODE_ALWAYS);
			break;
		case GENLIST_ITEM_SELECT_MODE_NONE:
			elm_genlist_item_select_mode_set(m_pEOItem, ELM_OBJECT_SELECT_MODE_NONE);
			break;
		}
	}

	bool GenlistItem::initialize(ItemAddMethod createItem, ItemSelectionMode itemSelectMode)
	{
		m_pEOItem = createItem(this);
		if (!m_pEOItem) {
			return false;
		}

		setSelectionMode(itemSelectMode);
		return true;
	}

	Elm_Genlist_Item_Class *GenlistItem::getItemClass()
	{
		static Elm_Genlist_Item_Class itc = createItemClass("type1");
		return &itc;
	}

	GenlistItem *GenlistItem::fromData(void *data)
	{
		return static_cast<GenlistItem *>(static_cast<WidgetItem *>(data));
	}

	Elm_Genlist_Item_Class GenlistItem::createItemClass(const char *style)
	{
		Elm_Genlist_Item_Class itc = { ELM_GEN_ITEM_CLASS_HEADER, 0 };
		itc.item_style = style;
		itc.decorate_item_style = NULL;
		itc.decorate_all_item_style = NULL;
		itc.func.text_get = [](void *data, Evas_Object *obj, const char *part) -> char * {
				RETVM_IF(!data, nullptr, "Data is null!");
				return fromData(data)->getText(part);
			};

		itc.func.content_get = [](void *data, Evas_Object *obj, const char *part) -> Evas_Object * {
				RETVM_IF(!data, nullptr, "Data is null!");
				return fromData(data)->getContent(obj, part);
			};

		itc.func.state_get = [](void *data, Evas_Object *obj, const char *part) -> Eina_Bool {
				RETVM_IF(!data, false, "Data is null!");
				return fromData(data)->getState(part);
			};

		itc.func.del = NULL;
		return itc;
	}

	void GenlistItem::onSelected()
	{
		elm_genlist_item_selected_set(m_pEOItem, EINA_FALSE);

		if (m_selectHandler.assigned()) {
			m_selectHandler();
		}
	}
}
