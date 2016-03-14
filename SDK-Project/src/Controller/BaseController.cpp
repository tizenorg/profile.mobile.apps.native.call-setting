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
#include "Controller/BaseController.h"
#include "Utils/Logger.h"

namespace Controller {

	BaseController::BaseController(App::AppCore &core, BaseController *parent) :
		m_Core (core),
		m_pParent(parent),
		m_pView(nullptr),
		m_ViewState(VIEW_STATE_HIDDEN),
		m_IsUpdateNeeded(false),
		m_IsDestroying(false),
		m_UpdateFlag(0)

	{
	}

	BaseController:: BaseController(App::AppCore &core) :
		BaseController(core, nullptr)
	{
	}

	BaseController::~BaseController()
	{
	}

	void BaseController::onPause()
	{
		if (m_Core.getViewManager().isTopView(m_pView)) {
			DBG("Top view is going to pause");
			switch (m_ViewState) {
			case VIEW_STATE_ACTIVATED :
				m_pView->deactivate();
				m_pView->hide();
				break;
			case VIEW_STATE_DEACTIVATED:
			case VIEW_STATE_SHOWN :
				m_pView->hide();
				break;
			case VIEW_STATE_HIDDEN:
				DBG("View is already hidden");
				break;
			}
			m_ViewState = VIEW_STATE_HIDDEN;
		}
	}

	void BaseController::onResume()
	{
		if (m_Core.getViewManager().isTopView(m_pView)) {
			DBG("Top view is going to resume");
			if (m_IsUpdateNeeded) {
				upadateView(m_UpdateFlag);
				m_IsUpdateNeeded = false;
				m_UpdateFlag = 0;
			}
			switch (m_ViewState) {
			case VIEW_STATE_ACTIVATED :
				DBG("View is already activated");
				break;
			case VIEW_STATE_SHOWN :
				m_pView->activate();
				break;
			case VIEW_STATE_DEACTIVATED:
				m_pView->activate();
				break;
			case VIEW_STATE_HIDDEN:
				m_pView->show();
				m_pView->activate();
				break;
			}
			m_ViewState = VIEW_STATE_ACTIVATED;
		}
	}

	void BaseController::onUpdate(int updateFlag)
	{
		if (m_Core.getViewManager().isTopView(m_pView)
				&& m_ViewState == VIEW_STATE_SHOWN) {
			upadateView(updateFlag);
		} else {
			m_IsUpdateNeeded = true;
			m_UpdateFlag |= updateFlag;
		}
	}

	void BaseController::onViewChangeBegin()
	{
		if (m_Core.getViewManager().isTopView(m_pView)) {
			if (m_IsUpdateNeeded) {
				upadateView(m_UpdateFlag);
				m_IsUpdateNeeded = false;
				m_UpdateFlag = 0;
			}

			if (m_ViewState == VIEW_STATE_HIDDEN) {
				DBG("View show");
				m_pView->show();
				m_ViewState = VIEW_STATE_SHOWN;
			}
		} else {
			if (m_ViewState == VIEW_STATE_SHOWN) {
				DBG("View deactivate");
				m_pView->deactivate();
				m_ViewState = VIEW_STATE_DEACTIVATED;
			}
		}
	}

	void BaseController::onViewChangeEnd()
	{
		if (m_Core.getViewManager().isTopView(m_pView)) {
			if (m_ViewState == VIEW_STATE_SHOWN) {
				DBG("View activated");
				m_pView->activate();
				m_ViewState = VIEW_STATE_ACTIVATED;
			}
		} else {
			if (m_ViewState == VIEW_STATE_DEACTIVATED) {
				DBG("View hide");
				m_pView->hide();
				m_ViewState = VIEW_STATE_HIDDEN;
			}
		}
	}

	void BaseController::makeDestroyReqeuest()
	{
		if (m_pParent) {
			m_pParent->destroyChild(this);
			m_IsDestroying = true;
		} else {
			ERR("Parent is null, Controller is root controller!");
		}
	}

	bool BaseController::destroyChild(BaseController *child)
	{
		if (child) {
			child->onDestroyRequestCb();
			return true;
		}

		return false;
	}

	void BaseController::onDestroyRequestCb()
	{
		RETM_IF(!m_IsDestroying, "Destroying is forbidden!");

		if (!m_Core.getViewManager().isTopView(m_pView)) {
			View::BaseView::destroy(m_pView);
		}

		delete this;
	}
}
