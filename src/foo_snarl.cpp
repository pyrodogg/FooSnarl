/*
Copyright (c) 2008, Skyler Kehren
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
#include "SnarlInterface.h"
#include "config.h"

#pragma comment(lib, "../../../shared/shared.lib")


DECLARE_COMPONENT_VERSION(
		"foo_snarl",
		"0.2.0", 
		"Snarl notification interface for Foobar2000\n"
		"Developed by: Skyler Kehren (Pyrodogg)\n"
		"Contributions by:Max Battcher\n"
		"foosnarl at pyrodogg.com\n"
		"Copyright (C) 2008-2010 Skyler Kehren\n"
		"Released under BSD License");

Snarl::V41::SnarlInterface sn;

class FooSnarl : public initquit, public play_callback{ 

public:
// initquit methods
 void on_init()
{
	//Register Playcallback module
	static_api_ptr_t<play_callback_manager>()->register_callback(this,flag_on_playback_new_track | flag_on_playback_dynamic_info | flag_on_playback_dynamic_info_track | flag_on_playback_pause | flag_on_playback_stop, true);
	//Register Foobar2000 with Snarl
	pfc::string8 image;
	g_advconfig_icon_default.get_static_instance().get_state(image);

	if(sn.IsSnarlRunning()){
		//Display appropriate message depending on registration state with Snarl
		if(sn.RegisterApp("Foobar2000","Foobar2000",image.toString(),core_api::get_main_window(),0,Snarl::V41::SnarlEnums::AppDefault)!=0){
			if(sn.AddClass("Play","Play",true)==0)
				sn.EZNotify("","Error","Couldn't create the Play class",5,"",0,0,0);
			if(sn.AddClass("Seek","Seek",true)==0)
				sn.EZNotify("","Error","Couldn't create Seek class",5,"",0,0,0);
			if(sn.AddClass("Pause","Pause",true) == 0)
				sn.EZNotify("","Error","Couldn't create Pause class",5,"",0,0,0);
			if(sn.AddClass("Stop", "Stop",true) == 0)
				sn.EZNotify("","Error","Couldn't create Stop class",5,"",0,0,0);
		}else{
			sn.EZNotify("","FooSnarl Error","FooSnarl Unable to register",5,image.toString(),0,0,0);
		}
	}
}

 void on_quit()
{
	//Unregister playcallback
	static_api_ptr_t<play_callback_manager>()->unregister_callback(this);
	//Unregister foosnarl
	pfc::string8 image;
	g_advconfig_icon_default.get_static_instance().get_state(image);

	if(sn.RemoveAllClasses(false)==0){
		//Failed to remove registered classes
		sn.EZNotify("","Error","FooSnarl failed to remove registered classes",10,"",0,0,0);
	}

	if(sn.UnregisterApp()==0){
		//FooSnarl failed to unregister
		sn.EZNotify("","Error","FooSnarl failed to unregister with Snarl",10,"",0,0,0);
	};
}

protected:
std::string LastTitle;
std::string LastImage;

void on_playback_event(const char *alertClass){
	static_api_ptr_t<play_control> pc;
	metadb_handle_ptr handle;
	pfc::string8 format;
	service_ptr_t<titleformat_object> script;
	pfc::string_formatter text;
	std::string snarl_title;
	std::string snarl_msg;
	std::string snarl_icon;
	WCHAR icon_check[MAX_PATH];
	long snarl_time;

	if (pc->get_now_playing(handle)) {
		//Process title format string for message body
		g_advconfig_string_format.get_static_instance().get_state(format);
		static_api_ptr_t<titleformat_compiler>()->compile_safe(script, format);
		pc->playback_format_title_ex(handle, NULL, text, script, NULL, play_control::display_level_titles);
		snarl_msg = text.toString();

		//Process title format string for message title
		g_advconfig_string_title_format.get_static_instance().get_state(format);
		static_api_ptr_t<titleformat_compiler>()->compile_safe(script, format);
		pc->playback_format_title_ex(handle, NULL, text, script, NULL, play_control::display_level_titles);
		snarl_title = text.toString();

		//Process title format string for message icon location
		g_advconfig_icon.get_static_instance().get_state(format);
		static_api_ptr_t<titleformat_compiler>()->compile_safe(script, format);
		pc->playback_format_title_ex(handle, NULL, text, script, NULL, play_control::display_level_titles);
		snarl_icon = text.toString();
		MultiByteToWideChar(CP_UTF8,0,snarl_icon.c_str(),strlen(snarl_icon.c_str())+1,icon_check,sizeof(icon_check)/sizeof(icon_check[0]));
		
		//Test for existance of folder.jpg picture file
		//If DNE display default image
		DWORD attrib = GetFileAttributes(icon_check);
		if((snarl_icon.c_str()=="" )||(0xFFFFFFFF == attrib)){
			g_advconfig_icon_default.get_static_instance().get_state(format);
			snarl_icon=(const char*)format.toString();
		}
		
		//Get display timeout from user settings. If invalid, send error. Shouldn't happen max and min are set. 
		snarl_time = (long) g_advconfig_time.get_static_instance().get_state_int();
		if((snarl_time==NULL)||(snarl_time==0)){
			snarl_time = 5;
			snarl_title = "ERROR";
			snarl_msg = "Set valid display time in settings";
		}

	    LastImage = snarl_icon;
		LastTitle = snarl_msg;
				
		//Send Snarl Message
		sn.EZNotify(alertClass,snarl_title.c_str(),snarl_msg.c_str(),snarl_time,snarl_icon.c_str(),0,0,0);
		
	}
}

public:
// play_callback methods
void FB2KAPI on_playback_starting(play_control::t_track_command p_command,bool p_paused) {}
void FB2KAPI on_playback_new_track(metadb_handle_ptr p_track) {
	//Check status of notify on playback
	if(g_advconfig_playcheck.get_static_instance().get_state())
		on_playback_event("Play");
}
void  on_playback_stop(play_control::t_stop_reason p_reason) {
	//Check status of notify on stop
	//Cannot get "current track" information when stopped
	if(g_advconfig_stopcheck.get_static_instance().get_state() && p_reason==0){
		//on_playback_event();
		pfc::string8 image;
		if(LastImage.c_str() == ""){
			g_advconfig_icon_default.get_static_instance().get_state(image);
		} else {
			image = LastImage.c_str();
		}
			
		sn.EZNotify("Stop","Stop",LastTitle.c_str(),5,image.toString(),0,0,0);
	}
}
void  on_playback_seek(double p_time) {}
void  on_playback_pause(bool p_state) {
	//Check status of notify on pause and play
	if(g_advconfig_pausecheck.get_static_instance().get_state() && p_state==true){
		on_playback_event("Pause");
	}
	else if(g_advconfig_playcheck.get_static_instance().get_state() && p_state==false){
		on_playback_event("Play");
	}
}
void  on_playback_edited(metadb_handle_ptr p_track) {}
void  on_playback_dynamic_info(const file_info & p_info) {}
void  on_playback_dynamic_info_track(const file_info &p_info) {}
void  on_playback_time(double p_time) {}
void  on_volume_change(float p_new_val) {}
};

//Register initquit
static initquit_factory_t< FooSnarl > foo0;


