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


VALIDATE_COMPONENT_FILENAME("foo_snarl.dll");
DECLARE_COMPONENT_VERSION(
		"FooSnarl",
		"1.2.0", 
		"Snarl notification interface for Foobar2000\n"
		"Developed by: Skyler Kehren (Pyrodogg)\n"
		"foosnarl at pyrodogg.com\n"
		"Copyright (C) 2008-2015 Skyler Kehren\n"
		"Released under BSD License\n"
		"Contributions by: Max Battcher");

Snarl::V42::SnarlInterface sn42;


#pragma region Declarations
inline char base64_char(unsigned char in);
void base64_encode(pfc::string_base & out, const unsigned char * data, unsigned size);
#pragma endregion 

#pragma region Callback Window
LRESULT CALLBACK WndProcFooSnarl(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_SHOWWINDOW:
		if (wParam == TRUE) {
			standard_commands::main_activate();
		}
	case WM_CLOSE:
		DestroyWindow(hwnd);
	
	case WM_USER:
		switch (LOWORD(wParam))
		{
		case Snarl::V42::SnarlEnums::SnarlQuit:
		{
			foo_snarl.try_unregister();
			return 0;
		}
		case Snarl::V42::SnarlEnums::NotifyAction:
			int action = HIWORD(wParam);

			switch (action) {
			case 1:
				//Back action
				standard_commands::main_previous();
				return 0;
			case 2:
				//Next action
				standard_commands::main_next();
				return 0;
			case 3:
				//Stop
				standard_commands::main_stop();
				return 0;
			case 4:
				//Play
				standard_commands::main_play();
				return 0;
			case 5:
				//Pause
				standard_commands::main_pause();
				return 0;
			}
		}
	default:
		if (message == Snarl::V42::SnarlInterface::Broadcast())
		{
			switch (wParam)
			{
			case Snarl::V42::SnarlEnums::SnarlLaunched:
			case Snarl::V42::SnarlEnums::SnarlStarted:
				foo_snarl.try_register();
				return 0;

			case Snarl::V42::SnarlEnums::SnarlQuit:
			case Snarl::V42::SnarlEnums::SnarlStopped:
				foo_snarl.try_unregister();
				return 0;
			}
		}
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
#pragma endregion

namespace FooSnarl {

	LPSTR FSClass(int intclass) {
		switch (intclass) {
		case MessageClass::Play:
			return "Play";
			break;
		case MessageClass::Pause:
			return "Pause";
			break;
		case MessageClass::Stop:
			return "Stop";
			break;
		default:
			return "";
			break;
		}
	}

	void FooSnarl::register_snarl_class(int intClass) {
		LONG32 ret = sn42.AddClass(FSClass(intClass), FSClass(intClass),0,0,0,0,5);
		if (ret < 0 && ret != -Snarl::V42::SnarlEnums::ErrorAlreadyRegistered)
		{
			console::formatter() << "[FooSnarl] Unable to register class " << intClass;
		}
	}

	void FooSnarl::send_snarl_message(int pAlertClass, pfc::string pTitleFormat, pfc::string pBodyFormat) {
		static_api_ptr_t<playback_control> pc;
		static_api_ptr_t<playlist_manager> pm;
		service_ptr_t<titleformat_object> script;
		metadb_handle_ptr handle;
		pfc::string8 format;
		pfc::string_formatter text;
		pfc::string snarl_title;
		pfc::string snarl_msg;
		pfc::string snarl_icon;
		pfc::string8 snarl_icon_data;
		static metadb_handle_ptr lastSong;
		static int FSLastMsgClass = 0;

		//Get and store now playing. If false, then retrieve last playing.
		if (pc->get_now_playing(handle)) {
			lastSong.copy(handle);
		}
		else {
			handle.copy(lastSong);
		}

		//If not playing and no stored handle, get focused playlist item.
		if (handle.is_empty()) pm->activeplaylist_get_focus_item_handle(handle);

		//If no handle is identified, then exit.
		if (handle.is_empty()) return;

		if (pAlertClass == MessageClass::Auto) {
			if (pc->is_paused()) {
				pAlertClass = MessageClass::Pause;
			}
			else if (pc->is_playing()) {
				pAlertClass = MessageClass::Play;
			}
			else {
				pAlertClass = MessageClass::Stop;
			}
		}

		//Process title format string for message body
		static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(script, pBodyFormat.get_ptr(), "Invalid format script");
		pc->playback_format_title_ex(handle, NULL, text, script, NULL, play_control::display_level_titles);
		snarl_msg = text.toString();

		//Process title format string for message title
		static_api_ptr_t<titleformat_compiler>()->compile_safe_ex(script, pTitleFormat.get_ptr(), "Invalid format script");
		pc->playback_format_title_ex(handle, NULL, text, script, NULL, play_control::display_level_titles);
		snarl_title = text.toString();

		metadb_handle_list handle_list;
		pfc::list_t<GUID> guid_list;
		handle_list.add_item(handle);
		guid_list.add_item(album_art_ids::cover_front);
		try
		{
			//Get front cover album art if available
			abort_callback_impl moo;
			album_art_extractor_instance_v2::ptr art_instance = static_api_ptr_t<album_art_manager_v2>()->open(handle_list, guid_list, moo);
			album_art_data_ptr art = art_instance->query(album_art_ids::cover_front, moo);
			if (art->get_size())
			{
				base64_encode(snarl_icon_data, art->get_ptr(), art->get_size());
				snarl_icon = "";
			}
		}
		catch (...)
		{
			//Use foobar2000 icon if album art isn't available.
			snarl_icon = foobarIcon;
		}

		//Send Snarl Message
		if (FSLastMsgClass != pAlertClass) {
			sn42.Hide(sn42.GetLastMsgToken());
		}
		FSLastMsgClass = pAlertClass;

		if (sn42.IsVisible(sn42.GetLastMsgToken()) == Snarl::V42::SnarlEnums::Success)
		{
			sn42.Update(sn42.GetLastMsgToken(), FSClass(pAlertClass), snarl_title.get_ptr(), snarl_msg.get_ptr(), -1, snarl_icon.get_ptr(), snarl_icon_data.get_ptr());
		}
		else
		{
			LONG32 ret = sn42.Notify(FSClass(pAlertClass), snarl_title.get_ptr(), snarl_msg.get_ptr(), -1, snarl_icon.get_ptr(), snarl_icon_data.get_ptr());
			if (ret > 0)
			{
				switch (pAlertClass) {
				case MessageClass::Play:
					sn42.AddAction(ret, "Pause", "@5");
					sn42.AddAction(ret, "Next", "@2");
					sn42.AddAction(ret, "Previous", "@1");
					break;
				case MessageClass::Pause:
				case MessageClass::Stop:
					sn42.AddAction(ret, "Play", "@4");
					sn42.AddAction(ret, "Next", "@2");
					sn42.AddAction(ret, "Previous", "@1");
					break;
				}
			}
		}
	}

	void FooSnarl::on_playback_event(int alertClass) {
		send_snarl_message(alertClass, Preferencesv2::titleformat_data, Preferencesv2::textformat_data);
	}

	void FooSnarl::try_register()
	{
		WNDCLASSEX wcex = { 0 };

		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.lpfnWndProc = (WNDPROC)WndProcFooSnarl;
		wcex.hInstance = core_api::get_my_instance();
		wcex.lpszClassName = _T("FooSnarlMsg");

		RegisterClassEx(&wcex);

		hwndFooSnarlMsg = CreateWindowEx(0, _T("FooSnarlMsg"), _T("FooSnarl Msg"), 0, 0, 0, 0, 0, GetDesktopWindow(), 0, 0, 0);

		if (hwndFooSnarlMsg == NULL)
		{
			console::formatter() << "[FooSnarl] Unable to create message window (Error 0x" << pfc::format_int(GetLastError(), 0, 16) << ")";
		}

		uGetModuleFileName(NULL, foobarIcon);
		foobarIcon += ",105";

		//Register Foobar2000 with Snarl
		pfc::string8 snarl_password;
		service_ptr_t<genrand_service> g_rand = genrand_service::g_create();
		g_rand->seed(time(NULL));
		pfc::array_t<unsigned> junk;
		junk.set_count(4);
		for (unsigned i = 0; i < 4; i++) junk[i] = g_rand->genrand(~0);
		base64_encode(snarl_password, junk.get_ptr(), 16);

		LONG32 ret = sn42.Register("Foobar2000", "Foobar2000", foobarIcon, snarl_password.get_ptr(), hwndFooSnarlMsg, WM_USER);

		if (ret > 0)
		{
			foo_snarl.register_snarl_class(MessageClass::Play);
			foo_snarl.register_snarl_class(MessageClass::Pause);
			foo_snarl.register_snarl_class(MessageClass::Stop);
		}
		else
		{
			if (ret != -Snarl::V42::SnarlEnums::ErrorNotRunning)
			{
				console::formatter() << "[FooSnarl] Unable to register with Snarl";
			}
		}
	}

	void FooSnarl::try_unregister()
	{
		//Unregister foosnarl
		LONG32 ret = sn42.ClearClasses();
		if (ret < 0 && ret != -Snarl::V42::SnarlEnums::ErrorNotRunning)
		{
			console::formatter() << "[FooSnarl] Failed to remove registered classes";
		}

		ret = sn42.Unregister("Foobar2000");
		if (ret < 0 && ret != -Snarl::V42::SnarlEnums::ErrorNotRunning)
		{
			console::formatter() << "[FooSnarl] Failed to unregister with Snarl";
		}

		DestroyWindow(hwndFooSnarlMsg);
		UnregisterClass(_T("FooSnarlMsg"), core_api::get_my_instance());
	}
}

#pragma region Utility Functions
inline char base64_char(unsigned char in)
{
	static const char base64_chars[64] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
										  'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
										  'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
										  'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'};
	return base64_chars[in];
}

void base64_encode(pfc::string_base & out, const unsigned char * data, unsigned size)
{
	out.reset();

	while (size >= 3)
	{
		out.add_byte( base64_char( data[0] >> 2 ) );
		out.add_byte( base64_char( ( ( data[0] & 3 ) << 4 ) + ( data[1] >> 4 ) ) );
		out.add_byte( base64_char( ( ( data[1] & 15 ) << 2 ) + ( data[2] >> 6 ) ) );
		out.add_byte( base64_char( data[2] & 63 ) );
		data += 3;
		size -= 3;
	}
	if (size == 2)
	{
		out.add_byte( base64_char( data[0] >> 2 ) );
		out.add_byte( base64_char( ( ( data[0] & 3 ) << 4 ) + ( data[1] >> 4 ) ) );
		out.add_byte( base64_char( ( data[1] & 15 ) << 2 ) );
		out.add_byte( '%' );
	}
	else if (size == 1)
	{
		out.add_byte( base64_char( data[0] >> 2 ) );
		out.add_byte( base64_char( ( data[0] & 3 ) << 4 ) );
		out.add_byte( '%' );
		out.add_byte( '%' );
	}
}
#pragma endregion 


