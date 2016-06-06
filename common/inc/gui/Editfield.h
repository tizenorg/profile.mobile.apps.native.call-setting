/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Widget.h"

#ifndef _EDITFIELD_H_
#define _EDITFIELD_H_

namespace gui {

	class Editfield : public Widget {
	public:
		typedef enum {
			EF_MULTILINE = 1,
			EF_CLEAR_BTN = 2,
			EF_PASSWORD = 4,
		} EditfieldFlags;

		typedef enum {
			SIP_TYPE_NORMAL = 0, /** Default layout. */
			SIP_TYPE_NUMBER, /** Number layout. */
			SIP_TYPE_EMAIL, /** Email layout. */
			SIP_TYPE_URL, /** URL layout. */
			SIP_TYPE_PHONENUMBER, /** Phone Number layout. */
			SIP_TYPE_IP, /** IP layout. */
			SIP_TYPE_MONTH, /** Month layout. */
			SIP_TYPE_NUMBERONLY, /** Number Only layout. */
			SIP_TYPE_HEX, /** Hexadecimal layout. */
			SIP_TYPE_TERMINAL, /** Command-line terminal layout including * esc, alt, ctrl key, so on */
			SIP_TYPE_PASSWORD, /** Like normal, but no auto-correct */
			SIP_TYPE_DATETIME, /** Date and time layout*/
			SIP_TYPE_EMOTICON, /** Emoticon layout*/
		} SipType;

		typedef enum {
			SIP_RETURN_KEY_DEFAULT,
			SIP_RETURN_KEY_DONE,
			SIP_RETURN_KEY_GO,
			SIP_RETURN_KEY_JOIN,
			SIP_RETURN_KEY_LOGIN,
			SIP_RETURN_KEY_NEXT,
			SIP_RETURN_KEY_SEARCH,
			SIP_RETURN_KEY_SEND,
			SIP_RETURN_KEY_SIGNIN,
		} SipReturnKeyType;

		typedef enum
		{
			AUTOCAPITAL_TYPE_NONE = 0, /** No auto-capitalization when typing. */
			AUTOCAPITAL_TYPE_WORD, /** Autocapitalize each word typed. */
			AUTOCAPITAL_TYPE_SENTENCE, /** Autocapitalize the start of each sentence */
			AUTOCAPITAL_TYPE_ALLCHARACTER /** Autocapitalize all letters. */
		} AutocapitalType;

		typedef enum {
			COPYPASTE_MODE_MARKUP,   /**< copy & paste text with markup tag */
			COPYPASTE_MODE_NO_IMAGE, /**< copy & paste text without item(image) tag */
			COPYPASTE_MODE_PLAINTEXT /**< copy & paste text without markup tag */
		} CopyPasteMode;

		typedef util::Delegate <bool(std::string &)> InputFilterHandler;

		void setFocusedHandler(NotiHandler handler) { m_focusEventHandler = handler; }
		void setUnfocusedHandler(NotiHandler handler) { m_unfocusEventHandler = handler; }
		void setInputEventHandler(NotiHandler handler) { m_inputEventHandler = handler; }

		void setInputFilterHandler(InputFilterHandler handler) { m_inputFilterHandler = handler;}
		void setInputLimitHandler(NotiHandler handler) { m_inputLimitHandler = handler; }

		void setInputLimit(int maxCharCount, int maxByteCount = 0);
		void resetInputLimit();

		void setInputCharRestriction(const std::string &acceptedChars, const std::string &rejectedChars);
		void resetInputCharRestriction();

		void setGuideText(const util::TString &text);
		void setEntryRawText(const std::string &);
		void setEntryMarkupText(const std::string &);
		std::string getEntryMarkupText();
		std::string getEntryRawText();

		void setTextStyle(const char *textStyle);
		void setEditfieldDisabled(bool disabledState);
		void setAutocapitalType(AutocapitalType type);
		void setCopyPasteMode(CopyPasteMode mode);
		void allowInputPrediction(bool allowPrediction);
		void showPasswordInput(bool showPassword);

		void setSipReturnKeydDisabled(bool disabledState);
		void sipTypeSet(SipType type);
		void sipReturnKeyTypeSet(SipReturnKeyType keyType);
		void setSipReturnClickHandler(NotiHandler handler) { m_sipReturnKeyHandler = handler; }
		void showSip();
		void hideSip();

	protected:
		Editfield ();
		virtual ~Editfield() {}

		bool initialize(const Widget &parent, int flags = 0, const util::TString &guideText = "");
		void updateClearBtnState();
		void onFocused(Evas_Object *obj, void *eventInfo);
		void onUnfocused(Evas_Object *obj, void *eventInfo);
		void onInputEvent(Evas_Object *obj, void *eventInfo);
		void onInputFilterEvent(Evas_Object *obj, char **inputText);
		void onMaxInputReached(Evas_Object *obj, void *eventInfo);
		void onClearBtnClick(Evas_Object *obj, void *eventInfo);
		void onSipReturnClicked(Evas_Object *obj, void *eventInfo);

	private:
		friend class Widget; // to be used in Widget::create
		typedef util::Callback<void(Evas_Object *obj, char **inputText)> EntryFilterCb;

		Evas_Object *entry;
		bool m_isInputLimitSet;
		bool m_isCharRestrictionSet;
		Elm_Entry_Filter_Limit_Size m_limitFilterData;
		Elm_Entry_Filter_Accept_Set m_charFilterData;
		Evas_Object *clearBtn;
		NotiHandler m_inputEventHandler;
		InputFilterHandler m_inputFilterHandler;
		NotiHandler m_focusEventHandler;
		NotiHandler m_unfocusEventHandler;
		NotiHandler m_inputLimitHandler;
		NotiHandler m_sipReturnKeyHandler;
	};
}

#endif /* __EDITFIELD_H__ */
