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

#include "gui/Widgets/DoubleTextListItem.h"

#include "Controller/CallFwController.h"

namespace CallSettings { namespace Controller {

	using namespace gui;

	namespace {
		enum {
			ITEM_COUNT = 4
		};

		const util::TString ITEM_LABELS[ITEM_COUNT] = {
				"IDS_CST_BODY_ALWAYS_FORWARD",
				"IDS_CST_MBODY_FORWARD_WHEN_BUSY",
				"IDS_COM_BODY_FORWARD_WHEN_UNANSWERED",
				"IDS_CST_MBODY_FORWARD_WHEN_UNREACHABLE"
		};
	}

	CallFwController::CallFwController(Application &app, NotiHandler handler) :
		ViewController(app, handler),
		m_app(app),
		m_pView(nullptr)
	{
	}

	CallFwController::~CallFwController()
	{
	}

	bool CallFwController::initialize()
	{
		RETVM_IF(!ViewController::initialize(), false, "Failed to initialize ViewController!");

		m_pView = m_app.getViewManager().pushView<View::GenlistView>();
		RETVM_IF(!m_pView, false, "Failed to create view");

		setBaseView(m_pView);

		return true;
	}

	void CallFwController::updateView(int updateFlag)
	{
		if (updateFlag | UF_INITIAL) {

			m_pView->getItem().setTitleText("IDS_CST_BODY_CALL_FORWARDING");

			Genlist &gl = m_pView->getGenlist();

			for (int i = 0; i < ITEM_COUNT; ++i) {
				DoubleTextListItem *item = gl.appendItem<DoubleTextListItem>(ITEM_LABELS[i]);
				if (item) {
					item->setCheckboxStyle(CHECKBOX_SWITCHER);
					item->setCheckMode(CheckboxListItem::SKIP_EVENTS);
				} else {
					ERR("Item create failed!");
				}
			}
		}
	}

}}
