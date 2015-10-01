/*
Copyright (c) 2012-2015, Skyler Kehren
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

class playbackmenu_commands_foosnarl : public mainmenu_commands {
public:	
	enum {
		cmd_nowplaying = 0,
		cmd_total
	};

	t_uint32 get_command_count(){
		return cmd_total;
	}

	GUID get_command(t_uint32 p_index){
		// {E8D1828F-1AB8-4AC9-BDDC-F6DBB1456961}
		static const GUID guid_foo_snarl_showplaying = { 0xe8d1828f, 0x1ab8, 0x4ac9, { 0xbd, 0xdc, 0xf6, 0xdb, 0xb1, 0x45, 0x69, 0x61 } };
		
		switch (p_index) {
			case cmd_nowplaying: return guid_foo_snarl_showplaying;
			default: uBugCheck();
		}
	}

	//Set p_out to the name of the n-th command.
	//This name is used to identify the command and determines
	// the default position of the command in the menu.
	void get_name(t_uint32 p_index, pfc::string_base & p_out){
		switch (p_index) {
			case cmd_nowplaying: p_out = "Snarl Now Playing"; break;
			default: uBugCheck();
		}
	}

	// Set p_out to the description for the n-th command.
	bool get_description(t_uint32 p_index, pfc::string_base & p_out){
		switch (p_index) {
			case cmd_nowplaying: p_out = "Displays current playback state as Snarl notification."; return true;
			default: uBugCheck();
		}
	}

	//Every set of commands needs to declare which group it  belongs to.
	GUID get_parent(){
		//return FooSnarl::Preferencesv1::guid_foosnarl_mainmenu_maingroup;
		return mainmenu_groups::playback;
	}

	// Execute n-th command.
	void execute(t_uint32 p_index, service_ptr_t<service_base> p_callback){
		switch (p_index) {
			case cmd_nowplaying:
				foo_snarl.on_playback_event(FSMsgClass::Auto); break;
			default: uBugCheck();
		}
	}
};

static mainmenu_commands_factory_t<playbackmenu_commands_foosnarl> foo_snarl_menu;
//static mainmenu_group_popup_factory mainmenu_group(FooSnarl::Preferencesv1::guid_foosnarl_mainmenu_maingroup, mainmenu_groups::playback,mainmenu_commands::sort_priority_dontcare,"FooSnarl");