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
	/*
	 * @brief Represent Editfield input widget
	 */
	class Editfield : public Widget {
	public:
		/*
		 * @brief Editfield widget flags which determine widget appearance
		 */
		typedef enum {
			EF_MULTILINE = 1,	/** Editfield displayed in multiple lines */
			EF_CLEAR_BTN = 2,	/** Editfield displayed with clear button which can be used for clear inputed text */
			EF_PASSWORD = 4,	/** Editfield displayed in password mode so inputed text will be shown dotted */
		} EditfieldFlags;

		/*
		 * @brief Editfield keypad types. Determine style of keyboard which will be shown when input provided
		 */
		typedef enum {
			SIP_TYPE_NORMAL = 0,	/** Default layout. */
			SIP_TYPE_NUMBER,		/** Number layout. */
			SIP_TYPE_EMAIL,			/** Email layout. */
			SIP_TYPE_URL, 			/** URL layout. */
			SIP_TYPE_PHONENUMBER,	/** Phone Number layout. */
			SIP_TYPE_IP,			/** IP layout. */
			SIP_TYPE_MONTH,			/** Month layout. */
			SIP_TYPE_NUMBERONLY,	/** Number Only layout. */
			SIP_TYPE_HEX,			/** Hexadecimal layout. */
			SIP_TYPE_TERMINAL,		/** Command-line terminal layout including * esc, alt, ctrl key, so on */
			SIP_TYPE_PASSWORD,		/** Like normal, but no auto-correct */
			SIP_TYPE_DATETIME,		/** Date and time layout */
			SIP_TYPE_EMOTICON,		/** Emoticon layout */
		} SipType;

		/*
		 * @brief Editfield keypad return key types. Determine text on return keypad button
		 */
		typedef enum {
			SIP_RETURN_KEY_DEFAULT,		/** Enter symbol */
			SIP_RETURN_KEY_DONE,		/** DONE */
			SIP_RETURN_KEY_GO,			/** GO */
			SIP_RETURN_KEY_JOIN,		/** JOIN */
			SIP_RETURN_KEY_LOGIN,		/** LOGIN */
			SIP_RETURN_KEY_NEXT,		/** NEXT */
			SIP_RETURN_KEY_SEARCH,		/** SEARCH */
			SIP_RETURN_KEY_SEND,		/** SEND */
			SIP_RETURN_KEY_SIGNIN,		/** SIGN IN */
		} SipReturnKeyType;

		/*
		 * @brief Editfield autocapitalize type. Determain which way to change register during user input
		 */
		typedef enum
		{
			AUTOCAPITAL_TYPE_NONE = 0,		/** No auto-capitalization when typing */
			AUTOCAPITAL_TYPE_WORD, 			/** Autocapitalize each word typed. */
			AUTOCAPITAL_TYPE_SENTENCE, 		/** Autocapitalize the start of each sentence */
			AUTOCAPITAL_TYPE_ALLCHARACTER 	/** Autocapitalize all letters. */
		} AutocapitalType;

		/*
		 * @brief Editfield copy-paste type.
		 */
		typedef enum {
			COPYPASTE_MODE_MARKUP,			/** copy & paste text with markup tag */
			COPYPASTE_MODE_NO_IMAGE,		/** copy & paste text without item(image) tag */
			COPYPASTE_MODE_PLAINTEXT		/** copy & paste text without markup tag */
		} CopyPasteMode;

		/*
		 * @brief Handler type for Input filter event. This handler invokes in process of user input befor text will be shown in entry
		 * @param[in]	text which tried to be entered into entry
		 * @return	true if text is allowed to be set into entry, otherwise false.
		 */
		typedef util::Delegate <bool(std::string &)> InputFilterHandler;

		/*
		 * @brief Set focus event handler
		 * @param[in]	handler		Handler which will be invoked
		 */
		void setFocusedHandler(NotiHandler handler) { m_focusEventHandler = handler; }

		/*
		 * @brief Set unfocus event handler
		 * @param[in]	handler		Handler which will be invoked
		 */
		void setUnfocusedHandler(NotiHandler handler) { m_unfocusEventHandler = handler; }

		/*
		 * @brief Set input event handler
		 * @param[in]	handler		Handler which will be invoked when input event happend
		 */
		void setInputEventHandler(NotiHandler handler) { m_inputEventHandler = handler; }

		/*
		 * @brief Set input filter event handler
		 * @param[in]	handler		Handler which will be invoked when user input text before text will be shown in entry.
		 */
		void setInputFilterHandler(InputFilterHandler handler) { m_inputFilterHandler = handler;}

		/*
		 * @brief Set input limit event handler
		 * @param[in]	handler		Handler which will be invoked when input limit is reached
		 */
		void setInputLimitHandler(NotiHandler handler) { m_inputLimitHandler = handler; }

		/*
		 * @brief Set input limit for entry
		 * @param[in]	maxCharCount		max input character count
		 * @param[in]	maxByteCount		max input byte count, 0 by default
		 */
		void setInputLimit(int maxCharCount, int maxByteCount = 0);

		/*
		 * @brief Reset input limit for entry to default value
		 */
		void resetInputLimit();

		/*
		 * @brief Set restriction for characters input
		 * @param[in]	acceptedChars	string which consists from allowed for input characters
		 * @param[in]	rejectedChars	string which consists from rejected for input characters
		 */
		void setInputCharRestriction(const std::string &acceptedChars, const std::string &rejectedChars);

		/*
		 * @brief Reset input cahr restriction for entry to default value
		 */
		void resetInputCharRestriction();

		/*
		 * @brief Set guide text which will be shown in empty entry as a tip
		 * @param[in]	text	Guide text
		 */
		void setGuideText(const util::TString &text);

		/*
		 * @brief Set raw text into entry field.
		 * @param[in]	text	Input text
		 * @note Using this function bypasses text filters
		 */
		void setEntryRawText(const std::string &);

		/*
		 * @brief Converts a UTF-8 string into markup text and set it into entry field.
		 * @param[in]	text	Input text
		 * @note Using this function bypasses text filters
		 */
		void setEntryMarkupText(const std::string &);


		/*
		 * @brief Gets the text currently shown in entry
		 * @return	Inputed text
		 */
		std::string getEntryMarkupText();

		/*
		 * @brief Gets the text currently shown in entry converted from markup to UTF-8
		 * @return	Inputed text
		 */
		std::string getEntryRawText();

		/*
		 * @brief Set the text style for displaying in entry widget
		 * @param[in]	textStyle	text style
		 */
		void setTextStyle(const char *textStyle);

		/*
		 * @brief Disabel/Enable entry input
		 * @param[in]	disabledState	If true input will be disabled, otherwise enabled
		 */
		void setEditfieldDisabled(bool disabledState);

		/*
		 * @brief Set autocapitalize type
		 * @param[in]	type	autocapitalize type
		 */
		void setAutocapitalType(AutocapitalType type);

		/*
		 * @brief Set Copy/Paste mode
		 * @param[in]	mode	Copy/Paste mode
		 */
		void setCopyPasteMode(CopyPasteMode mode);

		/*
		 * @brief Allow/Disable prediction at input text
		 * @param[in]	allowPrediction		If true prediction will be allowed, otherwise - disabled
		 */
		void allowInputPrediction(bool allowPrediction);

		/*
		 * @brief Show/Hide password input
		 * @param[in]	showPassword		If true password will be shown as text, otherwise - dotted
		 */
		void showPasswordInput(bool showPassword);

		/*
		 * @brief Enable/Disable SIP return key
		 * @param[in]	disabledState		If true  SIP return key will be disabled, otherwise enabled
		 */
		void setSipReturnKeydDisabled(bool disabledState);

		/*
		 * @brief set SIP type
		 * @param[in]	type		SIP type
		 */
		void sipTypeSet(SipType type);

		/*
		 * @brief set SIP return key type
		 * @param[in]	keyType		SIP return key type
		 */
		void sipReturnKeyTypeSet(SipReturnKeyType keyType);

		/*
		 * @brief set handler which will be invoked by click on SIP return key
		 * @param[in]	handler		SIP return click handler
		 */
		void setSipReturnClickHandler(NotiHandler handler) { m_sipReturnKeyHandler = handler; }

		/*
		 * @brief show SIP
		 */
		void showSip();

		/*
		 * @brief show SIP
		 */
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
