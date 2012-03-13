/*
Copyright (c) 2008-2011, Skyler Kehren
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
	namespace Preferencesv1 {
		// Branch for foo_snarl component settings.
		// Sub branch of Advance -> Tools
		static const GUID guid_branch_foosnarl = {0x375cbf41, 0xc5a3, 0x478b, {0x93, 0xf3, 0xb9, 0xb6, 0x7c, 0xcc, 0xb8, 0x43}};
		static advconfig_branch_factory foosnarl_branch(
			//Name
			COMPONENT_TITLE,
			//GUID
			guid_branch_foosnarl,
			//Parent GUID
			advconfig_branch::guid_branch_tools,
			//Priority
			0.0);

		//Message title titleformat string for the foosnarl component
		static const GUID guid_advconfig_string_title_format = {0xf6d66cdc, 0x872c, 0x48bc, {0xa7, 0x98, 0x1a, 0x6a, 0xab, 0x64, 0xcf, 0x45} };
		advconfig_string_factory g_advconfig_string_title_format(
			//display name
			"Title formatting string",
			//GUID of setting
			guid_advconfig_string_title_format,
			//GUID of the parent branch
			//advconfig_branch::guid_branch_display,
			guid_branch_foosnarl,
			//sorting
			0.0,
			//
			"$if(%isplaying%,$if(%ispaused%,Paused,Now Playing),Stopped)"
		);



		//Message body Titleformat string for the foosnarl component
		static const GUID guid_advconfig_string_format = { 0x76c6cd47, 0xf1b9, 0x49a7, { 0x89, 0x44, 0x45, 0x90, 0x9a, 0x96, 0x7a, 0x7f } };
		advconfig_string_factory g_advconfig_string_format(
			// display name
			"Body formatting string",
			// GUID of our setting
			guid_advconfig_string_format,
			// GUID of the parent branch
			//advconfig_branch::guid_branch_display,
			guid_branch_foosnarl,
			// sorting priority (we leave it at 0.0)
			0.0,
			// initial value
			//"[%album artist% - ]['['%album%[ CD%discnumber%][ #%tracknumber%]']' ]%title%[ '//' %track artist%]"
			"[%album artist%$crlf()]%title%"
		);

		//Icon title format string for foo_snarl component
		static GUID guid_advconfig_icon = {0xe141d065, 0x9f7d, 0x400a, {0x9f, 0xac, 0x4d, 0x38, 0x51, 0xda, 0x0a, 0xc6}};
		advconfig_string_factory g_advconfig_icon(
			"Image formatting string",
			guid_advconfig_icon,
			guid_branch_foosnarl,
			0.0,
			"$directory_path(%path%)\\folder.jpg"
		);

		//Defaul Image to display in message if album art is unavailable.  Also displays on stop message
		//a1ac14d7-f2f4-4a8d-9ba4-fcf72466af03
		static GUID guid_advconfig_icon_default = {0xa1ac14d7, 0xf2f4, 0x4a8d, {0x9b, 0xa4, 0xfc, 0xf7, 0x24, 0x66, 0xaf, 0x03}};
		advconfig_string_factory g_advconfig_icon_default(
			"Default Image if no Album Art available",
			guid_advconfig_icon_default,
			guid_branch_foosnarl,
			0.0,
			""
		);
	

		//Time for notification to be displayed
		static GUID guid_advconfig_time = {0xb224f26b, 0x9799, 0x40c0, {0x9f, 0x56, 0x87, 0x27, 0x70, 0x90, 0x1e, 0x9b}};
		advconfig_integer_factory g_advconfig_time(
			//Name
			"Message Display Time",
			//GUID
			guid_advconfig_time,
			//Parent GUID
			guid_branch_foosnarl,
			//Priority
			0.0,
			//Initial Value
			5,
			//Min
			1,
			//Max
			30
		);

		// {AC0EE854-AD81-4E50-80E5-AB5914EA21D0}
		static const GUID guid_foosnarl_mainmenu_maingroup = 
		{ 0xac0ee854, 0xad81, 0x4e50, { 0x80, 0xe5, 0xab, 0x59, 0x14, 0xea, 0x21, 0xd0 } };
	}
}
