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

#ifndef _GUI_RADIO_OPTION_ITEM_H_
#define _GUI_RADIO_OPTION_ITEM_H_

#include "BaseListItems.h"

namespace gui {

	class RadioOptionItem : public SimpleTextItem {
	public:
		RadioOptionItem();
		virtual ~RadioOptionItem();

	protected:
		bool initialize(ItemAddMethod createItem, Evas_Object *m_pRadioGroup, int value, const char *text = nullptr, bool isLocalized = true,
						ItemSelectionMode selectMode = GENLIST_ITEM_SELECT_MODE_ALWAYS);
		virtual Evas_Object *getContent(Evas_Object *genlist, const char *part) override;
		virtual void onSelected() override;

	protected:
		Evas_Object *m_pRadioGroup;
		int m_value;

	private:
		friend class WidgetItem;
	};
}

#endif /* _GUI_RADIO_OPTION_ITEM_H_ */
