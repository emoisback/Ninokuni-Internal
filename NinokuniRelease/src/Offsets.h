#pragma once

namespace Offsets
{
	static uintptr_t GEngine_Offset = 0x6532B20;
	static uintptr_t UWorld_Offset = 0x6536060;
	static uintptr_t Base = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));
};

namespace ValueData
{
	static float					slomo = 1;
	static int						movespeed = 550;
	static int						stateAtk = 1;
	static int						jumpCount = 1;
	static bool						b_AtkSpd = false;
	static bool						b_console = false;
	static bool						b_invis = false;
	static bool						b_reconnect = false;
	static bool						b_isinvis = false;
	static bool						b_hasEnableConsole = false;
	static bool						b_Stuck = false;
	static bool						b_NoAnimation = false;
	static bool						b_noclip = false;
	static int						AtkSpd = 10;
	static char						prevCollitionVal = (char)0;
};

