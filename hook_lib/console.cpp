#include "console.h"


void game_console::Render() {
	if (szBuffer[0]) {
		float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float outer_color[4] = { 0.12f, 0.12f, 0.10f, 1.0f };
		float inner_color[4] = { 0.25f, 0.25f, 0.20f, 1.0f };

		// 0x4ED973885856E206 = "white"
		// 0xB160909B712F8F9 = "lui_loader_no_offset"
		// uintptr_t shader = Material_RegisterHandle(0x4ED973885856E206, 1, -1); //*reinterpret_cast<uintptr_t*>(0x15024F278_g);
		uintptr_t shader = *reinterpret_cast<uintptr_t*>(g_Addrs.shader_white); // 0x101BEDF8_b on 1.34.0.15931218
		if (shader == NULL)
		{
			return;
		}

		R_AddCmdDrawStretchPic(32, 40, 1852, 2, 0, 0, 1, 1, outer_color, shader); //top bar
		R_AddCmdDrawStretchPic(32, 67, 1852, 2, 0, 0, 1, 1, outer_color, shader); //bottom bar
		
		R_AddCmdDrawStretchPic(30, 40, 2, 29, 0, 0, 1, 1, outer_color, shader); //left bar
		R_AddCmdDrawStretchPic(1882, 42, 2, 27, 0, 0, 1, 1, outer_color, shader); //right bar
		
		R_AddCmdDrawStretchPic(32, 42, 1850, 25, 0, 0, 1, 1, inner_color, shader);
		float scale = 1.0f;

		uintptr_t font = UI_GetFontHandle(ScrPlace_GetViewUIContext(0), 7, scale);
		float width = R_TextWidth(0, szBuffer, 0x7FFFFFFF, font);

		CL_DrawTextPhysical(szBuffer, 0x7FFFFFFF, font, 35, 67, scale, scale, 0.0f, white, 0, 0);
		if (edit_tick > 20 && edit_tick < 40) {
			CL_DrawTextPhysical("|", 0x7FFFFFFF, font, 35 + width, 67, scale, scale, 0.0f, white, 0, 0);
		}
		edit_tick++;

		if (edit_tick > 41) {
			edit_tick = 0;
		}
		//for (int i = 0; i < 8;i++) {
		//	float scale = 1.0f;
		//	CL_DrawTextPhysical(szBuffer, 0x7FFFFFFF, UI_GetFontHandle(ScrPlace_GetViewUIContext(0), 7, scale), 35, 65 + (i * 50), scale, scale, 0.0f, white, 0, 0);
		//}
	}
}

void game_console::KeyboardHandle(WPARAM wParam, int key, char chKey)
{	
	if (key == VK_RETURN)
	{
		if (g_Addrs.Cbuf_AddText == 0x0)
		{
			Cbuf_AddText(&g_console.szBuffer[g_console.start]);
		}
		else
		{
			auto _Cbuf_AddText = reinterpret_cast<void(*)(int clientNum, const char* cmd)>(g_Addrs.Cbuf_AddText);

			_Cbuf_AddText(0, &g_console.szBuffer[g_console.start]);
		}
		printf("T9> %s\n", &g_console.szBuffer[g_console.start]);

		g_console.bopen = false;
		memset(szBuffer, 0, 4096);
		return;
	}

	if (key == VK_BACK)
	{
		if (szBuffer[start])
		{
			szBuffer[strlen(szBuffer) - 1] = 0;
		}
		return;
	}

	if (strlen(szBuffer) < 4096 && chKey != NULL)
	{
		unsigned short vkey = NULL;
		vkey = chKey;
		strcat(szBuffer, (const char*)&vkey);
	}
}

game_console g_console;