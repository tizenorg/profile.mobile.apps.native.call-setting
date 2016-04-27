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

#ifndef _VIEW_CALL_FW_VIEW_
#define _VIEW_CALL_FW_VIEW_

#include "gui/Widgets/DoubleTextListItem.h"

#include "GenlistView.h"

namespace CallSettings { namespace View {

	class CallFwView : public GenlistView {
	public:
		typedef enum {
			ITEM_TYPE_ALWAYS_FORWARD,
			ITEM_TYPE_FORWARD_WHEN_BUSY,
			ITEM_TYPE_FORWARD_WHEN_UNANSWERED,
			ITEM_TYPE_FORWARD_WHEN_UNREACHABLE,
			ITEM_TYPE_COUNT
		} ItemType;

	public:
		gui::DoubleTextListItem *getItem(ItemType itemType);

	private:
		friend class BaseView;
		CallFwView(gui::NaviItem *naviItem);
		virtual ~CallFwView();

		bool initialize();

	private:
		gui::DoubleTextListItem *m_pItems[ITEM_TYPE_COUNT];
	};

}}

#endif /* _VIEW_CALL_FW_VIEW_ */
