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

#ifndef _GUI_SELECTABLE_ITEM_H_
#define _GUI_SELECTABLE_ITEM_H_

#include "OptionItem.h"
#include "Checkbox.h"

namespace gui {

	class SelectableItem : public OptionItem {
	public:
		typedef util::Delegate <void(SelectableItem *)> ItemSelectHandler;

		void enableSelectMode();
		void disableSelectMode();
		void markAsSelected(bool selectState);
		bool getSelectedState();
		void setSelectHandler(ItemSelectHandler handler) { m_selectHandler = handler;}

	protected:
		SelectableItem();
		virtual ~SelectableItem();
		virtual Evas_Object *getContent(Evas_Object *genlist, const char *part) override;
		virtual void onSelected() override;
		void onCheckChanged();
		void onCheckboxDestroyed();

	protected:
		Checkbox *m_pCheckbox;
		bool m_isItemSelected;
		bool m_isSelectModeActive;
		ItemSelectHandler m_selectHandler;

	private:
		friend class WidgetItem;
	};

	class SelectableGroupItem : public SelectableItem {
	protected:
		SelectableGroupItem() {}
		virtual ~SelectableGroupItem() {}
		virtual Elm_Genlist_Item_Class *getItemClass() override;
		virtual char * getText(const char *part) override;

	private:
		friend class WidgetItem;
	};
}

#endif /* _GUI_CHECK_OPTION_ITEM_H_ */
