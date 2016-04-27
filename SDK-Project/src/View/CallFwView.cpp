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

#include "View/CallFwView.h"

namespace CallSettings { namespace View {

	using namespace gui;

	static const util::TString ITEM_LABELS[CallFwView::ITEM_TYPE_COUNT] = {
			"IDS_CST_BODY_ALWAYS_FORWARD",
			"IDS_CST_MBODY_FORWARD_WHEN_BUSY",
			"IDS_COM_BODY_FORWARD_WHEN_UNANSWERED",
			"IDS_CST_MBODY_FORWARD_WHEN_UNREACHABLE"
	};

	CallFwView::CallFwView(NaviItem *naviItem) :
			GenlistView(naviItem)
	{
		for (int i = 0; i < ITEM_TYPE_COUNT; ++i) {
			m_pItems[i] = nullptr;
		}
	}

	CallFwView::~CallFwView()
	{
	}

	bool CallFwView::initialize()
	{
		RETVM_IF(!GenlistView::initialize(), false, "Failed to initialize GenlistView!");

		for (int i = 0; i < ITEM_TYPE_COUNT; ++i) {
			m_pItems[i] = m_pGenlist->appendItem<DoubleTextListItem>(ITEM_LABELS[i]);
			m_pItems[i]->setCheckboxStyle(CHECKBOX_SWITCHER);
		}

		m_pNaviItem->setTitleText("IDS_CST_BODY_CALL_FORWARDING");

		return true;
	}

	DoubleTextListItem *CallFwView::getItem(ItemType itemType)
	{
		RETVM_IF(((itemType < 0 )|| (itemType >= ITEM_TYPE_COUNT)), nullptr, "Invalid item type: %d", (int)itemType);
		return m_pItems[itemType];
	}

}}
