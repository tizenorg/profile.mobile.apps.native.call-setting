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

#ifndef _CONTROLLER_REJECT_MSG_EDIT_CONTROLLER_H_
#define _CONTROLLER_REJECT_MSG_EDIT_CONTROLLER_H_

#include "gui/ViewController.h"

#include "App/Application.h"
#include "View/RejectMsgEditView.h"

namespace CallSettings { namespace Controller {

	/*
	 * @brief Represents Reject Messages Editor View
	 */
	class RejectMsgEditorController : public gui::ViewController {
	public:
		virtual ~RejectMsgEditorController();

	protected:
		friend class ViewController;

		RejectMsgEditorController(Application &app, NotiHandler handler);

		/*
		 * @brief initialization of RejectMsgEditView in Create mode
		 * @return true on success, otherwise false
		 */
		bool initialize();

		/*
		 * @brief initialization of RejectMsgEditView in Edit mode
		 * @param[in]	editableMsgId	Reject message Id which will be modified
		 * @return true on success, otherwise false
		 */
		bool initialize(Model::StringKey editableMsgId);

		/*
		 * @brief RejectMsgEditView initialization is provided here
		 */
		bool initializeView();

		/*
		 * @brief Called when Done title button clicked.
		 * @param[in]	sender		Done button widget
		 */
		void onDoneBtnClick(gui::Widget &sender);

		/*
		 * @brief Called when Cancel title button clicked.
		 * @param[in]	sender		Cancel button widget
		 */
		void onCancelBtnClick(gui::Widget &sender);

		/*
		 * @brief Callback for input new text into entry
		 * @note This callbacj is invoked after onInputFilterEvent() when text is already displayed in entry
		 * Update inputted text characters count is provided here
		 */
		void onInputEvent();

		/*
		 * @brief Callback for input filter event. Called when user try to input text into entry before text is shown
		 * @param[in]	msgText		Text which user try to input
		 * @return true if text is valid or false otherwise
		 * @note if return value is false text will not be placed into entry.
		 * This callback needed for check SMS character count based on encoding
		 */
		bool onInputFilterEvent(std::string &msgText);

		/*
		 * @brief Update reject message text settings
		 * @return true on success, otherwise false
		 */
		bool updateMsgSettings();

		/*
		 * @brief Check whether edited message is duplicated with other Reject messages or not
		 * @param[in]	msg		Current editted message
		 * @return	true if message is duolicated, otherwise false
		 */
		bool isMsgDuplicated(const std::string &msg);

		/*
		 * @brief Calculate SMS text length and max SMS text length based to encoding
		 * @param[in]	smsText			Reject message SMS text
		 * @param[out]	smsCharCount	SMS Characters count for current text encoding
		 * @param[out]	smsMaxCount		SMS Max Characters count for current text encoding
		 * @return true on success, otherwise false
		 * @note SMS length depends on language text. Thus for English characters only max SMS length is 160 chars
		 * For Macedonian language for example it is 140.
		 */
		static bool calculateSMSTextLength(const std::string &smsText, int &smsCharCount, int &smsMaxCount);

	private:
		typedef enum {
			VIEW_MODE_CREATE,
			VIEW_MODE_EDIT
		} ViewMode;

		Application &m_app;
		View::RejectMsgEditView *m_pMsgEditView;
		ViewMode m_viewMode;
		Model::StringKey m_editableMsgId;
		int m_smsInputedCharCount;
		int m_smsMaxCharCount;
	};

} }

#endif /* _CONTROLLER_REJECT_MSG_EDIT_CONTROLLER_H_ */
