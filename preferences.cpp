/*
Copyright (c) 2008-2015, Skyler Kehren
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted 
provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of 
	conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of 
	conditions and the following disclaimer in the documentation and/or other materials provided 
	with the distribution.
    * Neither the name of the author nor the names of its contributors may be used to endorse 
	or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF 
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "stdafx.h"

namespace FooSnarl {
	namespace Preferencesv2 {
		static const GUID guid_titleformat_data = { 0xf6d66cdc, 0x872c, 0x48bc,{ 0xa7, 0x98, 0x1a, 0x6a, 0xab, 0x64, 0xcf, 0x45 } };
		static const char* titleformat_default = "$if(%isplaying%,$if(%ispaused%,Paused,Now Playing),Stopped)";
		cfg_string titleformat_data(guid_titleformat_data, titleformat_default);

		static const GUID guid_textformat_data = { 0x76c6cd47, 0xf1b9, 0x49a7,{ 0x89, 0x44, 0x45, 0x90, 0x9a, 0x96, 0x7a, 0x7f } };
		static const char* textformat_default = "[%album artist%$crlf()]%title%";
		cfg_string textformat_data(guid_textformat_data, textformat_default);

	/*	static const GUID guid_timeout = { 0xb224f26b, 0x9799, 0x40c0,{ 0x9f, 0x56, 0x87, 0x27, 0x70, 0x90, 0x1e, 0x9b } };
		static const int32_t timeout_default = 5;
		cfg_int timeout_data(guid_timeout, timeout_default);*/
	}

	class CMyPreferences : public CDialogImpl<CMyPreferences>, public preferences_page_instance {
	public:
		CMyPreferences(preferences_page_callback::ptr callback) : m_callback(callback){}

		//dialog resource ID
		enum {IDD = IDD_FOOSNARL_PREFS};

		//WTL Message Map
		BEGIN_MSG_MAP_EX(CMyPreferences)
			MSG_WM_INITDIALOG(OnInitDialog)
			COMMAND_HANDLER_EX(IDC_TITLEFORMAT_DATA,EN_UPDATE, OnChanged)
			COMMAND_HANDLER_EX(IDC_TEXTFORMAT_DATA, EN_UPDATE, OnChanged)
			COMMAND_HANDLER_EX(IDC_TITLEFORMAT_DATA, EN_SETFOCUS, OnSetFocus)
			COMMAND_HANDLER_EX(IDC_TEXTFORMAT_DATA, EN_SETFOCUS, OnSetFocus)
			COMMAND_HANDLER_EX(IDC_TESTBUTTON,BN_CLICKED,OnTestButtonClick)
			NOTIFY_HANDLER_EX(IDC_SYNTAXHELP,NM_CLICK, OnSyntaxHelpClick)
		END_MSG_MAP()

	private:
		CEdit titleformat;
		CEdit textformat;
		const preferences_page_callback::ptr m_callback;

		BOOL OnInitDialog(CWindow, LPARAM) {
			//console::info("Call InitDialog");
			titleformat = GetDlgItem(IDC_TITLEFORMAT_DATA);
			textformat = GetDlgItem(IDC_TEXTFORMAT_DATA);
			
			uSetWindowText(titleformat, Preferencesv2::titleformat_data);
			uSetWindowText(textformat, Preferencesv2::textformat_data);

			return FALSE;
		}

		bool has_changed(){
			pfc::string8 temp;

			uGetWindowText(titleformat,temp);
			if(Preferencesv2::titleformat_data != temp) return true;
			uGetWindowText(textformat,temp);
			if(Preferencesv2::textformat_data != temp) return true;
			
			return false;
		}

		t_uint32 get_state(){
			t_uint32 state = preferences_state::resettable;
			if(has_changed()) state |= preferences_state::changed;
			return state;
		}

		void apply(){
			uGetWindowText(titleformat, Preferencesv2::titleformat_data);
			uGetWindowText(textformat, Preferencesv2::textformat_data);
		}

		void on_change(){
			//tell the host that our state has changed to enable/disable the apply button appropriately.
			m_callback->on_state_changed();
		}

		void reset(){
			uSetWindowText(titleformat, Preferencesv2::titleformat_default);
			uSetWindowText(textformat, Preferencesv2::textformat_default);
		}

		void OnChanged(UINT, int, HWND c){
			on_change();
			if (c == titleformat || c == textformat) {
				UpdatePreview(c);
			}
		}

		void OnSetFocus(UINT, int, HWND c) {
			UpdatePreview(c);
		}

		void OnTestButtonClick(UINT, int, HWND) {
			foo_snarl.send_snarl_message(MessageClass::Auto, uGetWindowText(titleformat), uGetWindowText(textformat)); //uGetDlgItemInt(IDC_TIMEOUT, NULL, FALSE)
		}

		void UpdatePreview(HWND c) {
			pfc::string formatstring = uGetWindowText(c);

			static_api_ptr_t<playback_control> pc;
			static_api_ptr_t<playlist_manager> pm;
			metadb_handle_ptr handle;
			service_ptr_t<titleformat_object> script;
			pfc::string_formatter formattedtext;

			if (!pc->get_now_playing(handle)) {
				pm->activeplaylist_get_focus_item_handle(handle);
			}
			if (handle.is_empty()) return;

			//Process title format string
			static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(script, formatstring.ptr(), "Invalid format script");
			pc->playback_format_title_ex(handle, NULL, formattedtext, script, NULL, play_control::display_level_titles);
			uSetWindowText(GetDlgItem(IDC_FORMATPREVIEW), formattedtext.toString());
		}

		LRESULT OnSyntaxHelpClick(NMHDR *pNotifyStruct) {
			//Open syntax help file or browser window...
			ShellExecuteA(NULL, "open", "http://wiki.hydrogenaud.io/index.php?title=Foobar2000:Titleformat_Reference", NULL, NULL, SW_SHOWNORMAL);
			return 0;
		}
	};

	class Preferences_Page_FooSnarl : public preferences_page_impl<CMyPreferences> {
	public:
		const char * get_name(){
			return COMPONENT_TITLE;
		}

		GUID get_guid(){
			// {B30D44EB-D37C-4F6A-B589-39A707158641}
			static const GUID guid = { 0xb30d44eb, 0xd37c, 0x4f6a, { 0xb5, 0x89, 0x39, 0xa7, 0x7, 0x15, 0x86, 0x41 } };
			return guid;
		}

		GUID get_parent_guid(){
			return preferences_page::guid_tools;
		}

		bool get_help_url(pfc::string_base & out){
			out = "https://github.com/pyrodogg/FooSnarl";
			return true;
		}

	};

	preferences_page_factory_t<Preferences_Page_FooSnarl> _;
}