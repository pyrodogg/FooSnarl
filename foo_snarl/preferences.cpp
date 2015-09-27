/*
Copyright (c) 2008-2012, Skyler Kehren
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
		// {BD8A45C8-37DE-4285-9A39-A4B3D5A4FACC}
		static const GUID guid_titleformat_data = { 0xbd8a45c8, 0x37de, 0x4285, { 0x9a, 0x39, 0xa4, 0xb3, 0xd5, 0xa4, 0xfa, 0xcc } };
		cfg_string titleformat_data(guid_titleformat_data, "$if(%isplaying%,$if(%ispaused%,Paused,Now Playing),Stopped)");

		// {03C469B9-D5F5-46B0-B217-97DF2BF44A2F}
		static const GUID guid_textformat_data = { 0x3c469b9, 0xd5f5, 0x46b0, { 0xb2, 0x17, 0x97, 0xdf, 0x2b, 0xf4, 0x4a, 0x2f } };
		cfg_string textformat_data(guid_textformat_data, "[%album artist%$crlf()]%title%");

		// {B211CA86-2C01-4B4B-9DF1-72CE742C3123}
		static const GUID guid_timeout = { 0xb211ca86, 0x2c01, 0x4b4b, { 0x9d, 0xf1, 0x72, 0xce, 0x74, 0x2c, 0x31, 0x23 } };
		cfg_int timeout_data(guid_timeout, 5);
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
			COMMAND_HANDLER_EX(IDC_TIMEOUT,EN_UPDATE,OnChanged)
		END_MSG_MAP()

	private:
		CEdit titleformat;
		CEdit textformat;
		CEdit timeout;
		const preferences_page_callback::ptr m_callback;

		BOOL OnInitDialog(CWindow, LPARAM) {
			console::info("Call InitDialog");
			titleformat = GetDlgItem(IDC_TITLEFORMAT_DATA);
			textformat = GetDlgItem(IDC_TEXTFORMAT_DATA);
			timeout = GetDlgItem(IDC_TIMEOUT);

			pfc::string timeout_str = pfc::toString<t_int32>(Preferencesv2::timeout_data);

			uSetWindowText(titleformat, Preferencesv2::titleformat_data);
			uSetWindowText(textformat, Preferencesv2::textformat_data);
			uSetWindowText(timeout,timeout_str.get_ptr());

			titleformat.EnableWindow(true);
			textformat.EnableWindow(true);
			timeout.EnableWindow(true);

			return FALSE;
		}

		bool has_changed(){
			pfc::string8 temp;

			uGetWindowText(titleformat,temp);
			if(Preferencesv2::titleformat_data != temp) return true;
			uGetWindowText(textformat,temp);
			if(Preferencesv2::textformat_data != temp) return true;
			uGetWindowText(timeout,temp);
			if(Preferencesv2::timeout_data != atoi(temp)) return true;

			return false;
		}

		t_uint32 get_state(){
			t_uint32 state = preferences_state::resettable;
			if(has_changed()) state |= preferences_state::changed;
			return state;
		}

		void apply(){
			pfc::string8 temp;
			uGetWindowText(titleformat, Preferencesv2::titleformat_data);
			uGetWindowText(textformat, Preferencesv2::textformat_data);
			uGetWindowText(timeout, temp);
			Preferencesv2::timeout_data = atoi(temp);
		}

		void on_change(){
			//tell the host that our state has changed to enable/disable the apply button appropriately.
			m_callback->on_state_changed();
		}

		void reset(){
			uSetWindowText(titleformat,"$if(%isplaying%,$if(%ispaused%,Paused,Now Playing),Stopped)");
			uSetWindowText(textformat, "[%album artist%$crlf()]%title%");
			uSetWindowText(timeout, "5");
		}

		void OnChanged(UINT, int, HWND){
			on_change();
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
			out = "http://www.pyrodogg.com/foosnarl";
			return true;
		}

	};

	preferences_page_factory_t<Preferences_Page_FooSnarl> _;
}