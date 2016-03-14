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

#include "View/ViewManager/BaseView.h"
#include "View/ViewManager/ViewManager.h"
#include "Controller/ViewController.h"

namespace Controller {

	ViewController::ViewController(AppCore &core, DestroyRequestHandler destroyRequestHandler) :
		m_Core (core),
		m_destroyRequestHandler(destroyRequestHandler),
		m_pView(nullptr),
		m_isActivated(false),
		m_isVisible(false),
		m_isDestroying(false),
		m_updateFlag(0)
	{
	}

	ViewController::~ViewController()
	{
		finalize();
	}

	bool ViewController::initialize()
	{
		m_Core.getSystemEventManager().addSystemEventHandler(
				makeHandler(SystemEventHandler, ViewController, &ViewController::onSystemEvent, this));

		m_Core.getViewManager().addViewEventHandler(
				makeHandler(ViewEventHandler, ViewController, &ViewController::onViewEvent, this));

		return createView();
	}

	void ViewController::finalize()
	{
		m_Core.getSystemEventManager().removeSystemEventHandler(
				makeHandler(SystemEventHandler, ViewController, &ViewController::onSystemEvent, this));

		m_Core.getViewManager().removeViewEventHandler(
				makeHandler(ViewEventHandler, ViewController, &ViewController::onViewEvent, this));
	}

	void ViewController::onSystemEvent(SystemEvent event)
	{
		switch(event) {
		case SYS_EVENT_LANGUAGE_CHANGE:
			m_updateFlag &= UPDATE_LANGUAGE;
			doUpdate();
			break;
		case SYS_EVENT_REGION_FMT_CHANGE:
			m_updateFlag &= UPDATE_REGION_FORMAT;
			doUpdate();
			break;
		case SYS_EVENT_PAUSE:
			onPause();
			break;
		case SYS_EVENT_RESUME:
			onResume();
			break;
		}
	}

	void ViewController::onViewEvent(ViewEvent event)
	{
		switch(event) {
		case VIEW_CHANGE_BEGIN:
			onViewChangeBegin();
			break;
		case VIEW_CHANGE_END:
			onViewChangeEnd();
			break;
		case MENU_BUTTON_PRESSED:
			onMenuKeyPressed();
			break;
		case BACK_BUTTON_PRESSED:
			onBackKeyPressed();
			break;
		case ORIENTATION_CHANGED:
			m_updateFlag &= UPDATE_ORIENTATION;
			doUpdate();
			break;
		}
	}

	void ViewController::onPause()
	{
		if (m_Core.getViewManager().isTopView(m_pView)) {
			doDeactivate();
			doHide();
		}
	}

	void ViewController::onResume()
	{
		if (m_Core.getViewManager().isTopView(m_pView)) {
			doShow();
			doActivate();
		}
	}

	void ViewController::onViewChangeBegin()
	{
		if (m_Core.getViewManager().isTopView(m_pView)) {
			doShow();
		} else {
			doDeactivate();
		}
	}

	void ViewController::onViewChangeEnd()
	{
		if (m_Core.getViewManager().isTopView(m_pView)) {
			doActivate();
		} else {
			doHide();
		}
	}

	void ViewController::doActivate()
	{
		if (!m_isActivated && !m_isDestroying) {
			m_isActivated = true;
			onActivate();
		}
	}

	void ViewController::doDeactivate()
	{
		if (!m_isActivated) {
			m_isActivated = false;
			onDeactivate();
		}
	}

	void ViewController::doShow()
	{
		if (!m_isVisible) {
			m_isVisible = true;
			doUpdate();
			onShow();

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
		if (m_updateFlag && m_isVisible && !m_isDestroying) {
			onUpdate(m_updateFlag);
			m_updateFlag = 0;
		}
	}

	void ViewController::makeDestroyReqeuest()
	{
		m_isDestroying = true;
		m_destroyRequestHandler(this);
	}

}
