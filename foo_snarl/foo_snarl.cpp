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


VALIDATE_COMPONENT_FILENAME("foo_snarl.dll");
DECLARE_COMPONENT_VERSION(
		"FooSnarl",
		"2.0 ", 
		"Snarl notification interface for Foobar2000\n"
		"Developed by: Skyler Kehren (Pyrodogg)\n"
		"foosnarl at pyrodogg.com\n"
		"Copyright (C) 2008-2012 Skyler Kehren\n"
		"Released under BSD License\n"
		"Contributions by: Max Battcher");

pfc::string8 foobarIcon;

Snarl::V42::SnarlInterface sn42;
pfc::string8 snarl_password;
HWND hwndFooSnarlMsg;
std::map<int,char *> FSMsgClassDecode;
LONG32 lastClassMsg[4] = {0,0,0,0};
LONG32 lastMsg = 0;

int FSLastMsgClass = 0;
int FSMsgClassCount = 4;
metadb_handle_ptr lastSong;

enum FSMsgClass : int {
	Stop = 0,
	Play,
	Pause,
	Seek
};

#pragma region Declarations
static void try_register();
static void try_unregister();
inline char base64_char(unsigned char in);
void base64_encode(pfc::string_base & out, const unsigned char * data, unsigned size);
#pragma endregion 

LPSTR FSClass(int intclass){
	switch(intclass){
		case FSMsgClass::Play:
			return "Play";
			break;
		case FSMsgClass::Pause:
			return "Pause";
			break;
		case FSMsgClass::Stop:
			return "Stop";
			break;
		case FSMsgClass::Seek:
			return "Seek";
			break;
	}
	return "";
}

void FSRegisterClass(int intClass){
	LONG32 ret = sn42.AddClass(FSMsgClassDecode[intClass], FSMsgClassDecode[intClass], snarl_password.get_ptr());
	if ( ret < 0 && ret != -Snarl::V42::SnarlEnums::ErrorAlreadyRegistered )
	{
		console::formatter() << "[FooSnarl] Unable to register class " << intClass;
	}
}

#pragma region Callback Window
LRESULT CALLBACK WndProcFooSnarl(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch(message)
	{
	case WM_SHOWWINDOW:
		if(wParam == TRUE){
			standard_commands::main_activate();
		}
	case WM_CLOSE:
		DestroyWindow(hwnd);
	default:
		if (message == sn42.Broadcast())
		{
			switch(wParam)
			{
			case Snarl::V42::SnarlEnums::SnarlLaunched:
			case Snarl::V42::SnarlEnums::SnarlStarted:
				try_register();
				return 0;

			case Snarl::V42::SnarlEnums::SnarlQuit:
			case Snarl::V42::SnarlEnums::SnarlStopped:
				try_unregister();
				return 0;
			}
		}
		break;
	case WM_USER:
		switch(LOWORD(wParam))
		{
		case Snarl::V42::SnarlEnums::SnarlQuit:
			{
				try_unregister();
				return 0;
			}
		//case Snarl::V42::SnarlEnums::NotificationClicked:
		case Snarl::V42::SnarlEnums::CallbackInvoked:
			{
				static_api_ptr_t<ui_control>()->activate();
				return 0;
			}
	/*	case Snarl::V42::SnarlEnums::NotificationAck:
			case Snarl::V42::SnarlEnums::call
			{
				return 0;
			}*/
		//case Snarl::V42::SnarlEnums::NotificationTimedOut:
		case Snarl::V42::SnarlEnums::CallbackTimedOut:
			{
				return 0;
			}
		case Snarl::V42::SnarlEnums::NotifyAction:
			int action = HIWORD(wParam);
			switch(action){
			case 1:
				//Back action
				static_api_ptr_t<playback_control>()->start(playback_control::track_command_prev,false);
				return 0;
			case 2:
				//Next action
				static_api_ptr_t<playback_control>()->start(playback_control::track_command_next,false);
				return 0;
			case 3:
				//Stop
				static_api_ptr_t<playback_control>()->stop();
				return 0;
			}
		}
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}
#pragma endregion

#pragma  region Play Callback 
class play_callback_foosnarl : public play_callback {
protected:
	void on_playback_event(int alertClass){
		static_api_ptr_t<playback_control> pc;
		metadb_handle_ptr handle;
		pfc::string8 format;
		service_ptr_t<titleformat_object> script;
		pfc::string_formatter text;
		pfc::string snarl_title;
		pfc::string snarl_msg;
		pfc::string snarl_icon;
		pfc::string8 snarl_icon_data;
		long snarl_time;

		if(pc->get_now_playing(handle)){
			lastSong.copy(handle);
		} else {	
			handle.copy(lastSong);
		}

		if (handle.is_empty()) return;
	
		//Process title format string for message body
		FooSnarl::Preferencesv1::g_advconfig_string_format.get_static_instance().get_state(format);
		static_api_ptr_t<titleformat_compiler>()->compile_safe(script, format);
		pc->playback_format_title_ex(handle, NULL, text, script, NULL, play_control::display_level_titles);
		snarl_msg = text.toString();

		//Process title format string for message title
		FooSnarl::Preferencesv1::g_advconfig_string_title_format.get_static_instance().get_state(format);
		static_api_ptr_t<titleformat_compiler>()->compile_safe(script, format);
		pc->playback_format_title_ex(handle, NULL, text, script, NULL, play_control::display_level_titles);
		snarl_title = text.toString();

		metadb_handle_list handle_list;
		pfc::list_t<GUID> guid_list;
		handle_list.add_item(handle);
		guid_list.add_item( album_art_ids::cover_front );
		try
		{
			//Get front cover album art if available
			abort_callback_impl moo;
			album_art_extractor_instance_v2::ptr art_instance = static_api_ptr_t<album_art_manager_v2>()->open( handle_list, guid_list, moo );
			album_art_data_ptr art = art_instance->query( album_art_ids::cover_front, moo );
			if ( art->get_size() )
			{
				base64_encode( snarl_icon_data, art->get_ptr(), art->get_size() );
				snarl_icon = "";	
			}
		}
		catch (...)
		{
			//Process title format string for message icon location
			FooSnarl::Preferencesv1::g_advconfig_icon.get_static_instance().get_state(format);
			static_api_ptr_t<titleformat_compiler>()->compile_safe(script, format);
			pc->playback_format_title_ex(handle, NULL, text, script, NULL, play_control::display_level_titles);
			snarl_icon = text.toString();
		}

		//Test for existance of folder.jpg picture file
		/*DWORD attrib = GetFileAttributes(stringcvt::string_os_from_utf8(snarl_icon.get_ptr()));
		if((snarl_icon.get_ptr()=="")||(0xFFFFFFFF == attrib)){
			snarl_icon = foobarIcon;
		}*/

		//Get display timeout from user settings. If invalid, send error. Shouldn't happen max and min are set. 
		snarl_time = (long) FooSnarl::Preferencesv1::g_advconfig_time.get_static_instance().get_state_int();
		if((snarl_time==NULL)||(snarl_time==0)){
			snarl_time = 5;
			snarl_title = "ERROR";
			snarl_msg = "Set valid display time in settings";
		}

		//Send Snarl Message
		if(FSLastMsgClass != alertClass){
			sn42.Hide(sn42.GetLastMsgToken());
		}
		FSLastMsgClass = alertClass;

		if (sn42.IsVisible(lastClassMsg[alertClass]) == Snarl::V42::SnarlEnums::Success)
		{
			sn42.Update(lastClassMsg[alertClass], FSClass(alertClass),snarl_title.get_ptr(),snarl_msg.get_ptr(),snarl_time, snarl_icon.get_ptr(),snarl_icon_data.get_ptr(),0,0,0,0);
		}
		else
		{
			LONG32 ret = sn42.Notify(FSClass(alertClass), snarl_title.get_ptr(), snarl_msg.get_ptr(), snarl_time, snarl_icon.get_ptr(), snarl_icon_data.get_ptr(), 0, 0, 0,0);
			if (ret > 0)
			{
				FSAddActions();
			}
			lastClassMsg[alertClass] = ret;
		}
	}

	LONG32 FSAddActions(){
	LONG32 token = sn42.GetLastMsgToken();
	sn42.AddAction(token,"Back","@1");
	sn42.AddAction(token,"Next","@2");
	sn42.AddAction(token,"Stop","@3");
	return 0;
}

public:
	// play_callback methods
	void on_playback_starting(play_control::t_track_command p_command,bool p_paused) {}
	void on_playback_new_track(metadb_handle_ptr p_track) {
		on_playback_event(FSMsgClass::Play);
}
	void  on_playback_stop(play_control::t_stop_reason p_reason) {
	if(p_reason == play_control::stop_reason_eof || p_reason == play_control::stop_reason_user){
		on_playback_event(Stop);
	}
}
	void  on_playback_seek(double p_time) {}
	void  on_playback_pause(bool p_state) {
	if(p_state == true){
		on_playback_event(FSMsgClass::Pause);
	} else {
		on_playback_event(FSMsgClass::Play);
	}
}
	void  on_playback_edited(metadb_handle_ptr p_track) {}
	void  on_playback_dynamic_info(const file_info & p_info) {}
	void  on_playback_dynamic_info_track(const file_info &p_info) {
	on_playback_event(FSMsgClass::Play);
}
	void  on_playback_time(double p_time) {}
	void  on_volume_change(float p_new_val) {}
};

static play_callback_foosnarl pcb_foosnarl;

#pragma endregion

#pragma region InitQuit
class initquit_foosnarl : public initquit {

public:
	void on_init()
	{
		pfc::string8 image;
		
		static_api_ptr_t<ui_control> uiMain;
		WNDCLASSEX wcex = {0};
	
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.lpfnWndProc = (WNDPROC) WndProcFooSnarl;
		wcex.hInstance = core_api::get_my_instance();
		wcex.lpszClassName = _T("FooSnarlMsg");

		FSMsgClassDecode[FSMsgClass::Play] = "Play";
		FSMsgClassDecode[FSMsgClass::Pause] = "Pause";
		FSMsgClassDecode[FSMsgClass::Stop] = "Stop";
		FSMsgClassDecode[FSMsgClass::Seek] = "Seek";

		RegisterClassEx(&wcex);
	
		uGetModuleFileName(NULL, foobarIcon);
		foobarIcon += ",105";
		
		hwndFooSnarlMsg = CreateWindowEx(0,_T("FooSnarlMsg"),_T("FooSnarl Msg"),0,0,0,0,0,GetDesktopWindow(),0,0,0);

		if(hwndFooSnarlMsg == NULL)
		{
			console::formatter() << "[FooSnarl] Unable to create message window (Error 0x" << pfc::format_int(GetLastError(),0,16) << ")";
		}

		try_register();	
	}

	void on_quit()
	{
		try_unregister();

		lastSong = 0;
		DestroyWindow(hwndFooSnarlMsg);
		UnregisterClass(_T("FooSnarlMsg"),core_api::get_my_instance());
	}

};
#pragma endregion

#pragma region Main Menu
class mainmenu_commands_foosnarl : public mainmenu_commands {
	
	virtual t_uint32 get_command_count(){
		return 1;
	}

	virtual GUID get_command(t_uint32 p_index){
		// {E8D1828F-1AB8-4AC9-BDDC-F6DBB1456961}
		static const GUID guid_foosnarl_main_showplaying = { 0xe8d1828f, 0x1ab8, 0x4ac9, { 0xbd, 0xdc, 0xf6, 0xdb, 0xb1, 0x45, 0x69, 0x61 } };
		
		if(p_index == 0 )
			return guid_foosnarl_main_showplaying;
		return pfc::guid_null;
	}

	//Set p_out to the name of the n-th command.
	//This name is used to identify the command and determines
	// the default position of the command in the menu.
	virtual void get_name(t_uint32 p_index, pfc::string_base & p_out){
		if(p_index == 0)
			p_out = "Snarl Now Playing";
	}

	// Set p_out to the description for the n-th command.
	virtual bool get_description(t_uint32 p_index, pfc::string_base & p_out){
		if(p_index == 0)
			p_out = "Snarl Now Playing";
		else 
			return false;
		return true;
	}

	//Every set of commands needs to declare which group it  belongs to.
	virtual GUID get_parent(){
		return FooSnarl::Preferencesv1::guid_foosnarl_mainmenu_maingroup;
	}

	// Execute n-th command.
	virtual void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback){
		if(p_index == 0 && core_api::assert_main_thread()){
			//Execute Snarl Command here
			metadb_handle_ptr np;
			static_api_ptr_t<playback_control>()->get_now_playing(np);
			pcb_foosnarl.on_playback_new_track(np);
		}
	}
};
#pragma endregion

static void try_register()
{
	//Register Playcallback module
	static_api_ptr_t<play_callback_manager> playCBM;
	playCBM->register_callback(&pcb_foosnarl,play_callback_foosnarl::flag_on_playback_new_track| 
								/*flag_on_playback_dynamic_info | */
								play_callback_foosnarl::flag_on_playback_dynamic_info_track| 
								play_callback_foosnarl::flag_on_playback_pause| 
								play_callback_foosnarl::flag_on_playback_stop,true);

	//Register Foobar2000 with Snarl
	service_ptr_t<genrand_service> g_rand = genrand_service::g_create();
	g_rand->seed( time( NULL ) );
	pfc::array_t<unsigned> junk;
	junk.set_count( 4 );
	for ( unsigned i = 0; i < 4; i++ ) junk[ i ] = g_rand->genrand( ~0 );
	base64_encode( snarl_password, junk.get_ptr(), 16 );

	LONG32 ret = sn42.Register("Foobar2000", "Foobar2000", foobarIcon,snarl_password.get_ptr(),hwndFooSnarlMsg, WM_USER);

	if (ret > 0)
	{
		FSRegisterClass(Play);
		FSRegisterClass(Pause);
		FSRegisterClass(Stop);
		FSRegisterClass(Seek);
	}
	else
	{
		if (ret != -Snarl::V42::SnarlEnums::ErrorNotRunning)
		{
			console::formatter() << "[FooSnarl] Unable to register with Snarl";
		}
	}
}

static void try_unregister()
{
	//Unregister playcallback
	static_api_ptr_t<play_callback_manager>()->unregister_callback(&pcb_foosnarl);

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

//Register initquit, menu
static initquit_factory_t< initquit_foosnarl > foo_snarl_initquit;
static mainmenu_commands_factory_t<mainmenu_commands_foosnarl> foo_snarl_menu;
static mainmenu_group_popup_factory mainmenu_group(FooSnarl::Preferencesv1::guid_foosnarl_mainmenu_maingroup, mainmenu_groups::view,mainmenu_commands::sort_priority_dontcare,"FooSnarl");