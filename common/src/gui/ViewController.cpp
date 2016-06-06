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

#include "gui/ViewController.h"
#include "gui/ViewManager.h"

namespace gui {
	using namespace appfw;

	ViewController::ViewController(BaseApplication &core, NotiHandler handler) :
		m_Core (core),
		m_destroyRequestHandler(handler),
		m_isActivated(false),
		m_isVisible(false),
		m_isDestroying(false),
		m_updateFlag(0),
		m_pView(nullptr)
	{
	}

	ViewController::~ViewController()
	{
		m_Core.getSystemEventManager().removeSystemEventHandler(
				SystemEventHandler::wrap<ViewController, &ViewController::onSystemEvent>(this));
		m_Core.getViewManager().removeViewEventHandler(
				ViewEventHandler::wrap<ViewController, &ViewController::onViewEvent>(this));

		if (m_pView) {
			ViewManager &viewManager = m_Core.getViewManager();
			if(viewManager.isTopView(m_pView)) {
				viewManager.popView();
			} else {
				BaseView::destroy(m_pView);
			}
		}
	}

	bool ViewController::initialize()
	{
		m_Core.getSystemEventManager().addSystemEventHandler(
				SystemEventHandler::wrap<ViewController, &ViewController::onSystemEvent>(this));

		m_Core.getViewManager().addViewEventHandler(
				ViewEventHandler::wrap<ViewController, &ViewController::onViewEvent>(this));

		m_updateFlag |= UF_INITIAL;

		return true;
	}

	void ViewController::setBaseView(BaseView *view)
	{
		m_pView = view;
	}

	void ViewController::onSystemEvent(SystemEvent event)
	{
		switch(event) {
		case SYS_EVENT_LANGUAGE_CHANGE:
			m_updateFlag |= UF_LANGUAGE;
			doUpdate();
			break;
		case SYS_EVENT_REGION_FMT_CHANGE:
			m_updateFlag |= UF_REGION_FORMAT;
			doUpdate();
			break;
		case SYS_EVENT_PAUSE:
			handlePause();
			break;
		case SYS_EVENT_RESUME:
			handleResume();
			break;
		}
	}

	void ViewController::onViewEvent(ViewEvent event)
	{
		switch(event) {
		case VIEW_CHANGE_BEGIN:
			handleViewChangeBegin();
			break;
		case VIEW_CHANGE_END:
			handleViewChangeEnd();
			break;
		case MENU_BUTTON_PRESSED:
			handleMenuKeyPress();
			break;
		case BACK_BUTTON_PRESSED:
			handleBackKeyPress();
			break;
		case ORIENTATION_CHANGED:
			m_updateFlag |= UF_ORIENTATION;
			doUpdate();
			break;
		}
	}

	void ViewController::handlePause()
	{
		if (m_Core.getViewManager().isTopView(m_pView)) {
			doDeactivate();
			doHide();
		}

		m_updateFlag |= UF_WAS_PAUSED;
	}

	void ViewController::handleResume()
	{
		if (m_Core.getViewManager().isTopView(m_pView)) {
			doShow();
			doActivate();
		}
	}

	void ViewController::handleViewChangeBegin()
	{
		if (m_Core.getViewManager().isTopView(m_pView)) {
			doShow();
		} else {
			doDeactivate();
		}
	}

	void ViewController::handleViewChangeEnd()
	{
		if (m_Core.getViewManager().isTopView(m_pView)) {
			doActivate();
		} else {
			doHide();
			m_updateFlag |= UF_IS_POPPING;
		}
	}

	void ViewController::doActivate()
	{
		if (!m_isActivated && !m_isDestroying) {
			m_isActivated = true;
			m_pView->enableInputEvents();
			onActivate();
		}
	}

	void ViewController::doDeactivate()
	{
		if (m_isActivated) {
			m_isActivated = false;
			m_pView->disableInputEvents();
			onDeactivate();
		}
	}

	void ViewController::doShow()
	{
		if (!m_isVisible) {
			m_isVisible = true;
			onShow();
			doUpdate();
		}
	}

	void ViewController::doHide()
	{
		if (m_isVisible) {
			m_isVisible = false;
			onHide();
		}
	}

	void ViewController::doUpdate()
	{
		if (m_updateFlag && m_isVisible) {
			updateView(m_updateFlag);
			m_updateFlag = 0;
		}
	}

	void ViewController::handleMenuKeyPress()
	{
		if (m_Core.getViewManager().isTopView(m_pView) && m_isActivated) {
			onMenuKeyPressed();
		}
	}

	void ViewController::handleBackKeyPress()
	{
		if (m_Core.getViewManager().isTopView(m_pView) && m_isActivated) {
			onBackKeyPressed();
		}
	}

	void ViewController::makeDestroyReqeuest()
	{
		if (!m_isDestroying) {
			m_isDestroying = true;
			if (m_destroyRequestHandler.assigned()) {
				m_destroyRequestHandler();
			}
		}
	}

	void ViewController::onBackKeyPressed()
	{
		makeDestroyReqeuest();
	}

	void ViewController::addCustomUpdateFlags(int flags)
	{
		m_updateFlag |= ~(~flags | UF_BASE_FLAGS);
		doUpdate();
	}

	void ViewController::removeCustomUpdateFlags(int flags)
	{
		m_updateFlag &= (~flags | UF_BASE_FLAGS);
	}

	bool ViewController::isUpdateFlagsSet(int flags) const
	{
		if ((m_updateFlag & flags) == flags) {
			return true;
		} else {
			return false;
		}
	}
}
