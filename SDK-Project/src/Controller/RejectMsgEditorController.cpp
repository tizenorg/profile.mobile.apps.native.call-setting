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

#include "Controller/RejectMsgEditorController.h"

#include "msg.h"

namespace CallSettings { namespace Controller {

	using namespace Model;
	using namespace View;
	using namespace gui;

	RejectMsgEditorController::RejectMsgEditorController(Application &app, NotiHandler handler) :
		ViewController(app, handler),
		m_app(app),
		m_pMsgEditView(nullptr),
		m_viewMode(VIEW_MODE_CREATE),
		m_editableMsgId(STRING_KEY_REJECT_MSG_ITEM_1),
		m_smsInputedCharCount(0),
		m_smsMaxCharCount(0)
	{
	}

	RejectMsgEditorController::~RejectMsgEditorController()
	{
		m_pMsgEditView->setDoneClickHandler(nullptr);
		m_pMsgEditView->setCancelClickHandler(nullptr);
		m_pMsgEditView->setInputFilterHandler(nullptr);
		m_pMsgEditView->setInputEventHandler(nullptr);
	}

	bool RejectMsgEditorController::initialize()
	{
		RETVM_IF(!initializeView(), false, "Internal error!");

		m_viewMode = VIEW_MODE_CREATE;
		m_pMsgEditView->setViewTitle("IDS_CST_SK3_CREATE");
		m_pMsgEditView->setDisabledDoneBtn(true);
		return true;

	}

	bool RejectMsgEditorController::initialize(Model::StringKey msgKey)
	{
		RETVM_IF(!initializeView(), false, "Internal error!");

		std::string msgText;
		SettingResultCode res = m_app.getSettingsManager().getProperty(msgKey, msgText);
		RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to get mgs text");

		m_viewMode = VIEW_MODE_EDIT;
		m_editableMsgId = msgKey;
		m_pMsgEditView->setMsgText(_(msgText.c_str()));
		m_pMsgEditView->setViewTitle("IDS_CST_BODY_EDIT");
		return true;
	}

	bool  RejectMsgEditorController::initializeView()
	{
		RETVM_IF(!ViewController::initialize(), false, "Failed to initialize ViewController!");

		m_pMsgEditView = m_app.getViewManager().pushView<View::RejectMsgEditView>();
		RETVM_IF(!m_pMsgEditView, false, "Failed to create view instance!");

		m_pMsgEditView->setDoneClickHandler(
				WidgetNotiHandler::wrap<RejectMsgEditorController, &RejectMsgEditorController::onDoneBtnClick>(this));

		m_pMsgEditView->setCancelClickHandler(
				WidgetNotiHandler::wrap<RejectMsgEditorController, &RejectMsgEditorController::onCancelBtnClick>(this));

		m_pMsgEditView->setInputEventHandler(
				NotiHandler::wrap<RejectMsgEditorController, &RejectMsgEditorController::onInputEvent>(this));

		m_pMsgEditView->setInputFilterHandler(
				Editfield::InputFilterHandler::wrap<RejectMsgEditorController, &RejectMsgEditorController::onInputFilterEvent>(this));

		setBaseView(m_pMsgEditView);
		return true;
	}

	bool RejectMsgEditorController::updateMsgSettings()
	{
		std::string msgText = m_pMsgEditView->getMsgText();
		SettingResultCode res = SETTINGS_RES_SUCCESS;
		if (m_viewMode == VIEW_MODE_EDIT) {
			m_app.getSettingsManager().setProperty(m_editableMsgId, msgText);
			RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to save message, error: %d", res);
		} else {
			int msgCount = 0;
			res = m_app.getSettingsManager().getProperty(INT_KEY_REJECT_MSG_COUNT, msgCount);
			RETVM_IF(res != SETTINGS_RES_SUCCESS, false, "Failed to get mgs count");

			for (int i = STRING_KEY_REJECT_MSG_ITEM_1 + msgCount; i > STRING_KEY_REJECT_MSG_ITEM_1; i--) {
				std::string oldMsg;

				m_app.getSettingsManager().getProperty(static_cast<StringKey>(i-1), oldMsg);
				m_app.getSettingsManager().setProperty(static_cast<StringKey>(i), oldMsg);
			}

			m_app.getSettingsManager().setProperty(m_editableMsgId, msgText);
			msgCount++;
			m_app.getSettingsManager().setProperty(INT_KEY_REJECT_MSG_COUNT, msgCount);

		}
		return true;

	}

	bool RejectMsgEditorController::isMsgDuplicated(const std::string &msgText)
	{
		int msgCount = 0;
		m_app.getSettingsManager().getProperty(INT_KEY_REJECT_MSG_COUNT, msgCount);

		for (int i = STRING_KEY_REJECT_MSG_ITEM_1; i < STRING_KEY_REJECT_MSG_ITEM_1 + msgCount; i++) {
			std::string existedMsgText;
			m_app.getSettingsManager().getProperty(static_cast<StringKey>(i), existedMsgText);
			if (msgText.compare(existedMsgText) == 0) {
				DBG("Message already in use!");
				return true;
			}
		}

		return false;
	}

	void RejectMsgEditorController::onDoneBtnClick(Widget &sender)
	{
		std::string msgText = m_pMsgEditView->getMsgText();
		if (isMsgDuplicated(msgText)) {
			m_app.getViewManager().showToastNotification("IDS_CST_TPOP_THIS_CALL_REJECT_MESSAGE_ALREADY_EXISTS");
		} else {
			if (updateMsgSettings()) {
				makeDestroyReqeuest();
			}
		}
	}

	void RejectMsgEditorController::onCancelBtnClick(Widget &sender)
	{
		makeDestroyReqeuest();
	}

	void RejectMsgEditorController::onInputEvent()
	{
		std::string smsText = m_pMsgEditView->getMsgText();
		int inputCount = 0;
		int maxInputCount = 0;

		RETM_IF(!calculateSMSTextLength(smsText, inputCount, maxInputCount), "Failed to calculate sms length");

		if (inputCount == 0) {
			m_pMsgEditView->setDisabledDoneBtn(true);
		} else if ((inputCount != 0 && m_smsInputedCharCount == 0) || (inputCount > maxInputCount)) {
				m_pMsgEditView->setDisabledDoneBtn(false);
		}

		m_smsInputedCharCount = inputCount;
		m_smsMaxCharCount = maxInputCount;
		m_pMsgEditView->updateInputCharCount(m_smsInputedCharCount, m_smsMaxCharCount);
	}

	bool RejectMsgEditorController::onInputFilterEvent(std::string &msgText)
	{
		int smsCharCount = 0;
		int smsMaxCharCount = 0;

		std::string entryText = m_pMsgEditView->getMsgText() + msgText;
		if (!calculateSMSTextLength(entryText, smsCharCount, smsMaxCharCount)) {
			return false;
		}

		if (smsCharCount > smsMaxCharCount) {
			DBG("Input limit reached!");
			return false;
		} else {
			return true;
		}
	}

	bool RejectMsgEditorController::calculateSMSTextLength(const std::string &smsText, int &charCount, int &maxCharCount)
	{
		const int maxGsm7Len = 160;
		const int maxUnicodeLen = 70;

		msg_encode_type_t encode = MSG_ENCODE_GSM7BIT;
		unsigned textLen = 0;
		unsigned segmentLen = 0;
		int bytesInChar = 1;

		if (!smsText.empty()) {
			int resultCode = msg_util_calculate_text_length(smsText.c_str(), MSG_ENCODE_AUTO, &textLen, &segmentLen, &encode);
			if (resultCode != MSG_SUCCESS) {
				ERR("msg_calculate_text_length is failed ret = %d", resultCode);
				return false;
			}

			switch(encode) {
			case MSG_ENCODE_UCS2:
				bytesInChar = 2;
				break;

			case MSG_ENCODE_GSM7BIT:
			case MSG_ENCODE_AUTO:
				bytesInChar = 1;
				break;

			default:
				ERR("Unknown encode type: ", encode);
				break;
			}

			textLen /= bytesInChar;
			segmentLen /= bytesInChar;
		}

		if (segmentLen == 0) {
			segmentLen = encode == MSG_ENCODE_UCS2 ? maxUnicodeLen : maxGsm7Len;
		}

		charCount = textLen;
		maxCharCount = segmentLen;

		return true;
	}


} }
