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

class play_callback_foosnarl : public play_callback_static {
public:
	//play_callback_static
	unsigned get_flags(){
		return play_callback_foosnarl::flag_on_playback_new_track | 
			play_callback_foosnarl::flag_on_playback_dynamic_info_track | 
			play_callback_foosnarl::flag_on_playback_pause| 
			play_callback_foosnarl::flag_on_playback_stop;
	}

	// play_callback methods
	void on_playback_starting(play_control::t_track_command p_command,bool p_paused) {}
	void on_playback_new_track(metadb_handle_ptr p_track) {
		foo_snarl.on_playback_event(FooSnarl::MessageClass::Play);
	}
	void  on_playback_stop(play_control::t_stop_reason p_reason) {
		if(p_reason == play_control::stop_reason_eof || p_reason == play_control::stop_reason_user){
			foo_snarl.on_playback_event(FooSnarl::MessageClass::Stop);
		}
	}
	void  on_playback_seek(double p_time) {}
	void  on_playback_pause(bool p_state) {
		if(p_state == true){
			foo_snarl.on_playback_event(FooSnarl::MessageClass::Pause);
		} else {
			foo_snarl.on_playback_event(FooSnarl::MessageClass::Play);
		}
	}
	void  on_playback_edited(metadb_handle_ptr p_track) {}
	void  on_playback_dynamic_info(const file_info & p_info) {}
	void  on_playback_dynamic_info_track(const file_info &p_info) {
		foo_snarl.on_playback_event(FooSnarl::MessageClass::Play);
	}
	void  on_playback_time(double p_time) {}
	void  on_volume_change(float p_new_val) {}
};

static play_callback_static_factory_t<play_callback_foosnarl> pcb_foosnarl;