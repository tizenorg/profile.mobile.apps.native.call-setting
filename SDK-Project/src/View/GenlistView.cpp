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

#include "View/GenlistView.h"

namespace CallSettings { namespace View {

	using namespace gui;

	GenlistView::GenlistView(NaviItem *naviItem) :
			BaseView(naviItem),
			m_pGenlist(nullptr)
	{
	}

	GenlistView::~GenlistView()
	{
	}

	bool GenlistView::initialize()
	{
		RETVM_IF(!BaseView::initialize(), false, "Failed to initialize BaseView!");

		m_pGenlist = Widget::create<Genlist>(*m_pViewLayout);
		RETVM_IF(!m_pGenlist, false, "Failed to create genlist, unknown error");

		return setViewContent(*m_pGenlist);
	}

}}
