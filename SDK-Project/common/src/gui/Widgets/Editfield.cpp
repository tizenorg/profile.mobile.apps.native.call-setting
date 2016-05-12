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

#include "gui/Widgets/Editfield.h"

namespace gui {

	Editfield::Editfield() :
		entry(nullptr),
		clearBtn(nullptr)
	{
	}

	bool Editfield::initialize(const Widget &parent, int flags, const util::TString &guideText)
	{
		Evas_Object *editfieldParent = parent.getEvasObject();
		RETVM_IF(!editfieldParent, false, "Failed to create editfield: Parent in null");

		m_pEvasObject = elm_layout_add(editfieldParent);
		RETVM_IF(!m_pEvasObject, false, "Failed to create editfield: Internal error");

		if (flags & EF_MULTILINE) {
			elm_layout_theme_set(m_pEvasObject, "layout", "editfield", "multiline");
		} else {
			elm_layout_theme_set(m_pEvasObject, "layout", "editfield", "singleline");
		}

		evas_object_size_hint_align_set(m_pEvasObject, EVAS_HINT_FILL, 0.0);
		evas_object_size_hint_weight_set(m_pEvasObject, EVAS_HINT_EXPAND, 0.0);

		entry = elm_entry_add(m_pEvasObject);
		RETVM_IF(!entry, false, "Failed to create editfield: Internal error");

		elm_entry_cnp_mode_set(entry, ELM_CNP_MODE_PLAINTEXT);
		eext_entry_selection_back_event_allow_set(entry, EINA_TRUE);

		if (flags & EF_MULTILINE) {
			elm_entry_single_line_set(entry, EINA_FALSE);
			elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_SENTENCE);
		} else {
			elm_entry_single_line_set(entry, EINA_TRUE);
			elm_entry_scrollable_set(entry, EINA_TRUE);
			elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_NONE);
		}

		if (flags & EF_PASSWORD) {
			elm_entry_prediction_allow_set(entry, EINA_FALSE);
			elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_PASSWORD);
			elm_entry_password_set(entry, EINA_TRUE);
		} else {
			elm_entry_prediction_allow_set(entry, EINA_TRUE);
		}

		if (flags & EF_CLEAR_BTN) {
			clearBtn = elm_button_add(m_pEvasObject);
			RETVM_IF(!clearBtn, false, "Failed to create editfield: Internal error");

			elm_object_focus_allow_set(clearBtn, EINA_FALSE);
			elm_object_style_set(clearBtn, "editfield_clear");
			evas_object_smart_callback_add(clearBtn, "clicked",
					EoSmartCb::make<Editfield, &Editfield::onClearBtnClick>(), this);

			evas_object_show(clearBtn);
			elm_object_part_content_set(m_pEvasObject, "elm.swallow.button", clearBtn);
		}

		evas_object_smart_callback_add(entry, "focused",
				EoSmartCb::make<Editfield, &Editfield::onFocused>(), this);
		evas_object_smart_callback_add(entry, "unfocused",
				EoSmartCb::make<Editfield, &Editfield::onUnfocused>(), this);
		evas_object_smart_callback_add(entry, "changed",
				EoSmartCb::make<Editfield, &Editfield::onInputEvent>(), this);
		evas_object_smart_callback_add(entry, "preedit,changed",
				EoSmartCb::make<Editfield, &Editfield::onInputEvent>(), this);
		evas_object_smart_callback_add(entry, "maxlength,reached",
				EoSmartCb::make<Editfield, &Editfield::onMaxInputReached>(), this);
		evas_object_smart_callback_add(entry, "activated",
				EoSmartCb::make<Editfield, &Editfield::onSipReturnClicked>(), this);

		elm_entry_markup_filter_append(entry,
				EntryFilterCb::make<Editfield, &Editfield::onInputFilterEvent>(), this);

		if (guideText.isNotEmpty()) {
			setGuideText(guideText);
		}

		elm_object_part_content_set(m_pEvasObject, "elm.swallow.content", entry);
		return true;
	}

	void Editfield::updateClearBtnState()
	{
		if (clearBtn) {
			if (elm_object_focus_get(entry) && !elm_entry_is_empty(entry)) {
				elm_object_signal_emit(m_pEvasObject, "elm,action,show,button", "");
			} else {
				elm_object_signal_emit(m_pEvasObject, "elm,action,hide,button", "");
			}
		}
	}

	void Editfield::onFocused(Evas_Object *obj, void *eventInfo)
	{
		elm_object_signal_emit(m_pEvasObject, "elm,state,focused", "");

		updateClearBtnState();

		if (m_focusEventHandler.assigned()) {
			m_focusEventHandler();
		}
	}

	void Editfield::onUnfocused(Evas_Object *obj, void *eventInfo)
	{
		elm_object_signal_emit(m_pEvasObject, "elm,state,unfocused", "");

		updateClearBtnState();

		if (m_unfocusEventHandler.assigned()) {
			m_unfocusEventHandler();
		}
	}

	void Editfield::onClearBtnClick(Evas_Object *obj, void *eventInfo)
	{
		elm_entry_entry_set(entry, "");
	}

	void Editfield::onInputEvent(Evas_Object *obj, void *eventInfo)
	{
		updateClearBtnState();

		if (m_inputEventHandler.assigned()) {
			m_inputEventHandler();
		}
	}

	void Editfield::onInputFilterEvent(Evas_Object *obj, char **inputText)
	{
		RETM_IF(!inputText || !*inputText, "Invalid args!");

		if(m_inputFilterHandler.assigned()) {
			char *newChar = elm_entry_markup_to_utf8(*inputText);
			std::string enteredText(newChar);
			free(newChar);
			free(*inputText);
			if (m_inputFilterHandler(enteredText)) {
				*inputText = elm_entry_utf8_to_markup(enteredText.c_str());
			} else {
				*inputText = nullptr;
			}
		}
	}

	void Editfield::onMaxInputReached(Evas_Object *obj, void *eventInfo)
	{
		if (m_inputLimitHandler.assigned()) {
			m_inputLimitHandler();
		}
	}

	void Editfield::onSipReturnClicked(Evas_Object *obj, void *eventInfo)
	{
		if (m_sipReturnKeyHandler.assigned()) {
			m_sipReturnKeyHandler();
		}
	}

	void Editfield::setInputLimit(int maxCharCount, int maxByteCount)
	{
		m_limitFilterData.max_char_count = maxCharCount;
		m_limitFilterData.max_byte_count = maxByteCount;

		elm_entry_markup_filter_append(entry, elm_entry_filter_limit_size, &m_limitFilterData);
	}

	void Editfield::setInputCharRestriction(const std::string &acceptedChars, const std::string &rejectedChars)
	{

		m_charFilterData.accepted = acceptedChars.empty() ? nullptr : acceptedChars.c_str();
		m_charFilterData.rejected = rejectedChars.empty() ? nullptr : rejectedChars.c_str();

		elm_entry_markup_filter_append(entry, elm_entry_filter_accept_set, &m_charFilterData);
	}

	void Editfield::setEntryRawText(const std::string &text)
	{
		elm_entry_entry_set(entry, text.c_str());
		elm_entry_cursor_end_set(entry);
	}

	void Editfield::setEntryMarkupText(const std::string &text)
	{
		char *markupText = elm_entry_utf8_to_markup(text.c_str());
		elm_entry_entry_set(entry, markupText);
		elm_entry_cursor_end_set(entry);
		free(markupText);
	}

	std::string Editfield::getEntryRawText()
	{
		char *entryText = elm_entry_markup_to_utf8(elm_entry_entry_get(entry));
		std::string outputText(entryText);
		free(entryText);
		return outputText;
	}

	std::string Editfield::getEntryMarkupText()
	{
		std::string outputText(elm_entry_entry_get(entry));
		return outputText;
	}

	void Editfield::setTextStyle(const char *textStyle)
	{
		RETM_IF(!textStyle, "Invalid args");

		elm_entry_text_style_user_push(entry, textStyle);
	}

	void Editfield::setGuideText(const util::TString &text)
	{
		WidgetWrapper(entry).setPartText("elm.guide", text);
	}

	void Editfield::setEditfieldDisabled(bool disabledState)
	{
		if (disabledState) {
			elm_entry_editable_set(entry, EINA_FALSE);
			evas_object_freeze_events_set(entry, EINA_TRUE);
		} else {
			elm_entry_editable_set(entry, EINA_TRUE);
			evas_object_freeze_events_set(entry, EINA_FALSE);
		}
	}

	void Editfield::setAutocapitalType(AutocapitalType capitalizeType)
	{
		switch(capitalizeType) {
		case AUTOCAPITAL_TYPE_NONE :
			elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_NONE);
			break;
		case AUTOCAPITAL_TYPE_WORD :
			elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_WORD);
			break;
		case AUTOCAPITAL_TYPE_SENTENCE :
			elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_SENTENCE);
			break;
		case AUTOCAPITAL_TYPE_ALLCHARACTER :
			elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_ALLCHARACTER);
			break;
		}
	}

	void Editfield::setCopyPasteMode(CopyPasteMode cnpMode)
	{
		switch(cnpMode) {
		case COPYPASTE_MODE_MARKUP :
			elm_entry_cnp_mode_set(entry, ELM_CNP_MODE_MARKUP);
			break;
		case COPYPASTE_MODE_NO_IMAGE :
			elm_entry_cnp_mode_set(entry, ELM_CNP_MODE_NO_IMAGE);
			break;
		case COPYPASTE_MODE_PLAINTEXT :
			elm_entry_cnp_mode_set(entry, ELM_CNP_MODE_PLAINTEXT);
			break;
		}
	}

	void Editfield::allowInputPrediction(bool allowPrediction)
	{
		if (allowPrediction) {
			elm_entry_prediction_allow_set(entry, EINA_TRUE);
		} else {
			elm_entry_prediction_allow_set(entry, EINA_FALSE);
		}
	}

	void Editfield::showPasswordInput(bool showPassword)
	{
		if (showPassword) {
			elm_entry_password_set(entry, EINA_TRUE);
		} else {
			elm_entry_password_set(entry, EINA_FALSE);
		}
	}

	void Editfield::setSipReturnKeydDisabled(bool disabledState)
	{
		if (disabledState) {
			elm_entry_input_panel_return_key_disabled_set(entry, EINA_TRUE);
		} else {
			elm_entry_input_panel_return_key_disabled_set(entry, EINA_FALSE);
		}
	}

	void Editfield::sipReturnKeyTypeSet(SipReturnKeyType returnKeyType)
	{
		switch(returnKeyType) {
		case SIP_RETURN_KEY_DEFAULT:
			elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DEFAULT);
			break;
		case SIP_RETURN_KEY_DONE:
			elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
			break;
		case SIP_RETURN_KEY_GO:
			elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_GO);
			break;
		case SIP_RETURN_KEY_JOIN:
			elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_JOIN);
			break;
		case SIP_RETURN_KEY_LOGIN:
			elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_LOGIN);
			break;
		case SIP_RETURN_KEY_NEXT:
			elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT);
			break;
		case SIP_RETURN_KEY_SEARCH:
			elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_SEARCH);
			break;
		case SIP_RETURN_KEY_SEND:
			elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_SEND);
			break;
		case SIP_RETURN_KEY_SIGNIN:
			elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_SIGNIN);
			break;
		}
	}

	void Editfield::sipTypeSet(SipType keyType)
	{
		switch (keyType) {
		case SIP_TYPE_NORMAL :
			elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_NORMAL);
			break;
		case SIP_TYPE_NUMBER :
			elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_NUMBER);
			break;
		case SIP_TYPE_EMAIL :
			elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_EMAIL);
			break;
		case SIP_TYPE_URL :
			elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_URL);
			break;
		case SIP_TYPE_PHONENUMBER :
			elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_PHONENUMBER);
			break;
		case SIP_TYPE_IP :
			elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_IP);
			break;
		case SIP_TYPE_MONTH :
			elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_MONTH);
			break;
		case SIP_TYPE_NUMBERONLY :
			elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_NUMBERONLY);
			break;
		case SIP_TYPE_HEX :
			elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_HEX);
			break;
		case SIP_TYPE_TERMINAL :
			elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_TERMINAL);
			break;
		case SIP_TYPE_PASSWORD :
			elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_PASSWORD);
			break;
		case SIP_TYPE_DATETIME :
			elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_DATETIME);
			break;
		case SIP_TYPE_EMOTICON :
			elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_EMOTICON);
			break;
		}
	}

	void Editfield::showSip()
	{
		Ecore_IMF_Context *imfContext = static_cast<Ecore_IMF_Context *>(elm_entry_imf_context_get(entry));
		if (imfContext) {
			ecore_imf_context_input_panel_show(imfContext);
		}
	}

	void Editfield::hideSip()
	{
		Ecore_IMF_Context *imfContext = static_cast<Ecore_IMF_Context *>(elm_entry_imf_context_get(entry));
		if (imfContext) {
			ecore_imf_context_input_panel_hide(imfContext);
		}

	}

}
