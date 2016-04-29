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

#include "gui/Widgets/PendingPopupContent.h"

namespace gui {

	PendingPopupContent::PendingPopupContent() :
			m_pLayout(nullptr),
			m_pProcessing(nullptr)
	{
	}

	PendingPopupContent::~PendingPopupContent()
	{
	}

	bool PendingPopupContent::initialize(const Widget &parent, const util::TString &label, ProcessingType type)
	{
		m_pLayout = Widget::create<Layout>(parent, "layout", "common", "pending.popup");
		RETVM_IF(!m_pLayout, false, "Failed to create Layout");

		m_pEvasObject = m_pLayout->getEvasObject();

		m_pProcessing = Widget::create<ProcessingCircle>(*m_pLayout, type);
		RETVM_IF(!m_pProcessing, false, "Failed to create ProcessingCircle");

		m_pLayout->setPartContent("elm.swallow.content", m_pProcessing);

		if (label.isNotEmpty()) {
			m_pLayout->setPartText("elm.text", label);
		}

		return true;
	}

	void PendingPopupContent::setLabel(const util::TString &label)
	{
		m_pLayout->setPartText("elm.text", label);
	}

	void PendingPopupContent::setStyle(ProcessingType type)
	{
		m_pProcessing->setStyle(type);
	}

}
