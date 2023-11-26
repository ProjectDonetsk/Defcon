#pragma once
#include "Main.hpp"

struct game_console {
	bool bopen;
	int start;
	char szBuffer[4096];
	int edit_tick;
	void Render();
	void KeyboardHandle(WPARAM wParam, int key, char chKey);
};


extern game_console g_console;