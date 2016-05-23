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
			m_pGenlist(nullptr),
			m_pNoContentLayout(nullptr)
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

	gui::Genlist &GenlistView::getGenlist()
	{
		return *m_pGenlist;
	}

	void GenlistView::showEmptyView(util::TString mainText, util::TString helpText)
	{
		if (!m_pNoContentLayout) {
			m_pNoContentLayout = Widget::create<Layout>(*m_pViewLayout, "layout", "nocontents", "default");
			RETM_IF(!m_pNoContentLayout, "Failed to create no content layout");
		}

		m_pNoContentLayout->setPartText("elm.text", mainText);
		m_pNoContentLayout->setPartText("elm.help.text", helpText);

		m_pNoContentLayout->emitSignal("align.center", "elm");
		m_pNoContentLayout->emitSignal("text,disabled", "");

		m_pViewLayout->unsetContent("elm.swallow.content");
		m_pGenlist->hide();
		m_pNoContentLayout->show();
		m_pViewLayout->setContent("elm.swallow.content", *m_pNoContentLayout);
	}

	void GenlistView::showListView()
	{
		if (m_pNoContentLayout) {
			m_pViewLayout->unsetContent("elm.swallow.content");
			m_pNoContentLayout->hide();
		}

		m_pViewLayout->setContent("elm.swallow.content", *m_pGenlist);
		m_pGenlist->show();
	}

}}
