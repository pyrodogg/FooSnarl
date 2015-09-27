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

#pragma once

#include "../../foobar2000/sdk/foobar2000.h"
#include "../../foobar2000/helpers/helpers.h"
#include "../../SnarlInterface_V42/SnarlInterface.h"
#include "../../foobar2000/ATLHelpers/ATLHelpers.h"
#pragma comment(lib, "../../foobar2000/shared/shared.lib")

#include "resource.h"
#include "config.h"
#include "preferences.h"
#include <map>
#include <strsafe.h>
#include <time.h>

#define _SECURE_ATL 1

namespace FooSnarl{
	class FooSnarl{
	public:
		void on_playback_event(int alertClass);
		LONG32 FSAddActions();
	};
}

enum FSMsgClass : int {
	Stop = 0,
	Play,
	Pause,
	Seek
};

static FooSnarl::FooSnarl foo_snarl;

#define COMPONENT_TITLE "FooSnarl"
#define COMPONENT_VERSION = "2.0.0"
#define COMPONENT_NAME = "foo_snarl"
#define COMPONENT_DESCRIPTION = "Snarl notification interface for Foobar2000 Developed by: Skyler Kehren (Pyrodogg) foosnarl at pyrodogg.com Copyright (C) 2008-2010 Skyler Kehren Released under BSD License"