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
		//on_playback_event(FSMsgClass::Play);
		foo_snarl.on_playback_event(FSMsgClass::Play);
	}
	void  on_playback_stop(play_control::t_stop_reason p_reason) {
		if(p_reason == play_control::stop_reason_eof || p_reason == play_control::stop_reason_user){
			//on_playback_event(Stop);
			foo_snarl.on_playback_event(FSMsgClass::Stop);
		}
	}
	void  on_playback_seek(double p_time) {}
	void  on_playback_pause(bool p_state) {
		if(p_state == true){
			foo_snarl.on_playback_event(FSMsgClass::Pause);
		} else {
			foo_snarl.on_playback_event(FSMsgClass::Play);
		}
	}
	void  on_playback_edited(metadb_handle_ptr p_track) {}
	void  on_playback_dynamic_info(const file_info & p_info) {}
	void  on_playback_dynamic_info_track(const file_info &p_info) {
		foo_snarl.on_playback_event(FSMsgClass::Play);
	}
	void  on_playback_time(double p_time) {}
	void  on_volume_change(float p_new_val) {}
};

static play_callback_static_factory_t<play_callback_foosnarl> pcb_foosnarl;