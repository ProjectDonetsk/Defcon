#include "Main.hpp"
#include <tlhelp32.h>

bool IsProcessRunning(const char* processName)
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return false;
	}

	do
	{
		if(strcmp(processName, pe32.szExeFile) == 0)
		{
			CloseHandle(hProcessSnap);
			return true;
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);
	return false;
}

void backtrace(const char * func) {
	const int trace_count = 15;
	void* trace_back[trace_count];
	DWORD hash;
	RtlCaptureStackBackTrace(1, trace_count, trace_back, &hash);
	printf(("%s callstack: "), func);
	for (int i = 0; i < trace_count; i++) {
		if (i == trace_count - 1) {
			printf(("%p\n"), (uintptr_t)trace_back[i]);
		}
		else {

			printf(("%p:"), (uintptr_t)trace_back[i]);
		}
	}
}

uintptr_t GetFunctionHandle(uintptr_t file, uint32_t nspace, uint32_t fname) {
	int count = g_assetPool[ASSET_TYPE_SCRIPTPARSETREE].itemAllocCount;
	for (int i = 0; i < count; i++) {
		if (g_assetPool[ASSET_TYPE_SCRIPTPARSETREE].pool.scriptParseTreeOld->__padding0000 == 0) {
			GSC_OBJ* gsc = (GSC_OBJ*)(g_assetPool[ASSET_TYPE_SCRIPTPARSETREE].pool.scriptParseTreeOld[i].data);
			if (gsc != 0 && g_assetPool[ASSET_TYPE_SCRIPTPARSETREE].pool.scriptParseTreeOld[i].name == file) {
				printf("0x%p found!\n", file);
				GSC_EXPORT_ITEM* gsc_exports = (GSC_EXPORT_ITEM*)((uintptr_t)gsc + gsc->exports_offset);
				for (int i = 0; i < gsc->exports_count; i++) {
					if (gsc_exports[i].name_space == nspace && gsc_exports[i].name == fname) {
						printf("0x%X - 0x%X found!\n", nspace, fname);
						return (uintptr_t)(gsc)+gsc_exports[i].address;
					}
				}
			}
		}
		else {
			GSC_OBJ* gsc = (GSC_OBJ*)(g_assetPool[ASSET_TYPE_SCRIPTPARSETREE].pool.scriptParseTreeNew[i].data);
			if (gsc != 0 && g_assetPool[ASSET_TYPE_SCRIPTPARSETREE].pool.scriptParseTreeNew[i].name == file) {
				printf("0x%p found!\n", file);
				GSC_EXPORT_ITEM* gsc_exports = (GSC_EXPORT_ITEM*)((uintptr_t)gsc + gsc->exports_offset);
				for (int i = 0; i < gsc->exports_count; i++) {
					if (gsc_exports[i].name_space == nspace && gsc_exports[i].name == fname) {
						printf("0x%X - 0x%X found!\n", nspace, fname);
						return (uintptr_t)(gsc)+gsc_exports[i].address;
					}
				}
			}
		}
	}
	return 0;
}

LONG WINAPI TopLevelExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	if (pExceptionInfo && pExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_ACCESS_VIOLATION)
	{
		if (pExceptionInfo->ContextRecord->Rip == 0x1417794C3_g ||
			pExceptionInfo->ContextRecord->Rip == 0x1410DF720_g) {
			NtContinue(pExceptionInfo->ContextRecord, 0);
			return EXCEPTION_EXECUTE_HANDLER;
		}
	}
	// backtrace("TopLevelExceptionHandler");
	printf("%p - %X - %p\n", pExceptionInfo->ContextRecord->Rip, pExceptionInfo->ExceptionRecord->ExceptionCode, _ReturnAddress());
	return EXCEPTION_CONTINUE_SEARCH;
}

uintptr_t render_hook_original;
uintptr_t load_script_original;
uintptr_t client_think_original;
uintptr_t some_stub;

void Render(uintptr_t a1) {

	printf("hooked!\n");

	auto _stub = reinterpret_cast<void(*)(uintptr_t)>(some_stub);
	_stub(a1);
}

bool is_script_ready;

extern "C" __declspec(dllexport) int DiscordCreate() //unsigned int version, uintptr_t params, void** result
{
	entry_point();

	if (*reinterpret_cast<int*>(0x1411017B0_g) != 0x70657DE9) {
		SETUP_HOOK(render_hook_original, g_Addrs.dvar_show_over_stack, RENDERER, ("Render hook"));
		// SETUP_HOOK(load_script_original, g_Addrs.dvar_load_script, LOAD_SCRIPT, ("Load Script hook"));
	}

	return 1;
}

uintptr_t set_zombie_var_handle;
//reversed for someone off of a dll they sent
void test_thread() {
	uintptr_t mbase = (uintptr_t)GetModuleHandleA(0);
	
	auto Cinematic_StopPlayback = reinterpret_cast<bool(*)(int, bool)>(mbase + 0x9C0F5A0);
	auto LobbyBase_SetNetworkMode = reinterpret_cast<void(*)(int)>(mbase + 0xAF652D0);
	auto Com_SessionMode_SetMode = reinterpret_cast<void(*)(int)>(mbase + 0xC1C78A0);
	auto SetScreen = reinterpret_cast<void(*)(int, bool)>(mbase + 0xB1FAB70);
	auto CL_Disconnect = reinterpret_cast<void(*)(int, bool, const char*)>(mbase + 0x5CDDBB0);
	auto LobbyData_SetMap = reinterpret_cast<void(*)(int, const char *)>(mbase + 0xAF72A10);
	auto LobbySession_GetControllingLobbySession = reinterpret_cast<int(*)(int)>(mbase + 0xA3CDEF0);
	auto LobbyHostBots_AddBotsToLobby = reinterpret_cast<void(*)(int, int, bool, int, uintptr_t, int)>(mbase + 0x77AB7C0);

	AllocConsole();
	FILE* Dummy;
	freopen_s(&Dummy, ("CONIN$"), ("r"), stdin);
	freopen_s(&Dummy, ("CONOUT$"), ("w"), stdout);
	freopen_s(&Dummy, ("CONOUT$"), ("w"), stderr);
	puts("ZeusOffline v1.9 for game version 1.26.0.11629285\n");
	puts("Waiting for UI...");
	while ((*reinterpret_cast<char*>(mbase + 0x101BABB0) & 1) == 0) {
		Sleep(150);
	}
	puts("UI arrived!");
	*reinterpret_cast<char*>(mbase + 0x1A5F7B97) = 1;
	Cinematic_StopPlayback(0, true);
	LobbyBase_SetNetworkMode(1);
	Com_SessionMode_SetMode(0);
	SetScreen(11, 0);
	//some pattern scan stuff
	
	puts("\n");
	puts("NUMPAD1 = disconnect\n");
	puts("NUMPAD2 = Change map of multiplayer lobby\n");
	puts("NUMPAD3 = Start Campaign\n");
	puts("NUMPAD4 = Start Multiplayer\n");
	puts("NUMPAD5 = Start Zombies\n");
	puts("NUMPAD6 = Add bot to lobby\n");

	while (true) {
		if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
			puts("Disconnecting...");
			CL_Disconnect(0, 0, "");
		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
			LobbyData_SetMap(LobbySession_GetControllingLobbySession(0) != 0, "zm_silver");
		}
		if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
			puts("Changing lobby type to Campaign...");
			Com_SessionMode_SetMode(2);
			CL_Disconnect(0, 0, "");
		}
		if (GetAsyncKeyState(VK_NUMPAD4) & 1) {
			puts("Changing lobby type to Multiplayer...");
			Com_SessionMode_SetMode(1);
			CL_Disconnect(0, 0, "");
		}
		if (GetAsyncKeyState(VK_NUMPAD5) & 1) {
			puts("Changing lobby type to Zombies...");
			Com_SessionMode_SetMode(0);
			CL_Disconnect(0, 0, "");
		}
		if (GetAsyncKeyState(VK_NUMPAD6) & 1) {
			LobbyHostBots_AddBotsToLobby(0, 1, true, 0, 0, 0);
		}
	}
}

void set_zombie_var(uintptr_t zvar, int value) {
	ScrVm_AddInt(0, value);
	uintptr_t _hash = zvar;
	ScrVm_AddString(0, (uintptr_t)&_hash);
	
	ScrVm_FreeThread(0, ScrVm_ExecThread(0, set_zombie_var_handle, 2, 0, 0));
}

BOOL WINAPI QueryPerformanceCounter_Detour(LARGE_INTEGER* lpPerformanceCount) {

	//printf("%s\n", __FUNCTION__);
	//if (is_script_ready) {
	//	if (Com_IsInGame() && (uintptr_t)(_ReturnAddress()) == g_Addrs.clientthink_ret) {
	//		if (set_zombie_var_handle) {
	//			set_zombie_var(zombie_powerup_drop_max_per_round, 100);
	//			set_zombie_var(zombie_drop_item, 1);
	//			set_zombie_var(zombie_perk_cost, 10);
	//			set_zombie_var(zombie_insta_kill, 1);
	//			set_zombie_var(zombie_point_scalar, 10);
	//			printf("called\n");
	//		}
	//	}
	//}

	return QueryPerformanceCounter(lpPerformanceCount);
}

char mod_buffer[0x300];

void Renderer()
{
	if (g_console.bopen) {
		g_console.Render();
	}

	uintptr_t g_auth_manager = *reinterpret_cast<uintptr_t*>(g_Addrs.g_auth_manager);
	if (g_auth_manager) {
		switch (*reinterpret_cast<int*>(0x1411017B0_g)) {
		case 0xB8ECE9C1: //1.9.1
		for (int i = 0; i < *reinterpret_cast<int*>(g_auth_manager + 0x10); i++) {
			uintptr_t temp = *reinterpret_cast<uintptr_t*>(g_auth_manager + 0x18);
			if (temp) {
				*reinterpret_cast<int*>(temp + (0xD8 * i) + 0x24) = 0;
				*reinterpret_cast<int*>(temp + (0xD8 * i) + 0x28) = 0;
				*reinterpret_cast<int*>(temp + (0xD8 * i) + 0x50) = 2;
				*reinterpret_cast<int*>(temp + (0xD8 * i) + 0x54) = 3;
			}
		}
		break;
		case 0xE9CC1D0E: //1.33.2.15030131
			for (int i = 0; i < *reinterpret_cast<int*>(g_auth_manager + 0x8); i++) {
				uintptr_t temp = *reinterpret_cast<uintptr_t*>(g_auth_manager + 0x10);
				if (temp) {
					*reinterpret_cast<int*>(temp + (0xD0 * i) + 0x14) = 0;
					*reinterpret_cast<int*>(temp + (0xD0 * i) + 0x48) = 2;
					*reinterpret_cast<int*>(temp + (0xD0 * i) + 0x4C) = 3;
				}
			}
		break;
		case 0x8348B74E: //1.34.0.15931218
			for (int i = 0; i < *reinterpret_cast<int*>(g_auth_manager + 0x8); i++) {
				uintptr_t temp = *reinterpret_cast<uintptr_t*>(g_auth_manager + 0x10);
				if (temp) {
					*reinterpret_cast<int*>(temp + (0xD0 * i) + 0x14) = 0;
					*reinterpret_cast<int*>(temp + (0xD0 * i) + 0x48) = 2;
					*reinterpret_cast<int*>(temp + (0xD0 * i) + 0x4C) = 3;
				}
			}
		break;
		//default: printf("0x1411017B0 = 0x%X\n", *reinterpret_cast<int*>(0x1411017B0_g)); break;
		}
	}

	

	float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	uintptr_t font = *reinterpret_cast<uintptr_t*>(g_Addrs.watermark_font);
	if (font) {
		//CL_DrawTextPhysical("T9 Offline - codUPLOADER", 0x7FFFFFFF, font, 20, 35, 0.40f, 0.40f, 0.0f, white, 0, 0);
	}//48 83 EC ?? 48 85 C9 0F 84 ?? ?? ?? ?? 83 79 ?? 01 8B 41 ?? - Dvar_GetBool
}

void GameLoadScript() {
	uintptr_t file = 0x3f9e0dc8454d98e1;
	uint32_t checksum = 0;
	set_zombie_var_handle = Scr_GetFunctionHandle(0, (uintptr_t)&file, 0x2a39b4c3, 0x63977437, &checksum, false);
	if (!set_zombie_var_handle) {
		set_zombie_var_handle = GetFunctionHandle(0x3f9e0dc8454d98e1, 0x2a39b4c3, 0x63977437);
	}
	printf("handle %p - %p\n", set_zombie_var_handle, GetFunctionHandle(0x3f9e0dc8454d98e1, 0x2a39b4c3, 0x63977437));



}

utils::hook::detour dispatch_exception;
bool RtlDispatchException_Detour(PEXCEPTION_RECORD ExceptionRecord, PCONTEXT Context) {
	
	if (ExceptionRecord->ExceptionCode == STATUS_INVALID_HANDLE)
	{
		NtContinue(Context, FALSE);
		return true;
	}

	if ((uintptr_t)ExceptionRecord->ExceptionAddress < (base + 0x100000000)) {
		if (ExceptionRecord->ExceptionCode == STATUS_ACCESS_VIOLATION)
		{
			// backtrace(__FUNCTION__);
			if (Context->Rip == g_Addrs.Dvar_GetBool_cmp) { //for any type of dvar

				switch (Context->Rcx) {
					case HOOK_TYPE(RENDERER):
						Context->Rcx = render_hook_original;
						Renderer();
					break;
					case HOOK_TYPE(LOAD_SCRIPT):
						Context->Rcx = load_script_original;
						//GameLoadScript();
					break;
					default:
						MessageBoxA(0, 0, "UNHANDLED EXCEPTION IN DVAR_GETBOOL", 0);
					break;
				}
				NtContinue(Context, FALSE);
				return true;
			}
		}
	}

	return dispatch_exception.invoke<bool>(ExceptionRecord, Context);
}

utils::hook::detour get_thread_context_hook;
BOOL WINAPI get_thread_context_stub(const HANDLE thread_handle, const LPCONTEXT context)
{
	constexpr auto debug_registers_flag = (CONTEXT_DEBUG_REGISTERS & ~CONTEXT_AMD64);
	if (context->ContextFlags & debug_registers_flag)
	{
		auto* source = _ReturnAddress();
		const auto game = utils::nt::library{};
		const auto source_module = utils::nt::library::get_by_address(source);

		if (source_module == game)
		{
			context->ContextFlags &= ~debug_registers_flag;
		}
	}

	return get_thread_context_hook.invoke<BOOL>(thread_handle, context);
}

NTSTATUS ZwQueryInformationThread_Detour(HANDLE ThreadHandle, THREADINFOCLASS ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength, PULONG ReturnLength) {

	//backtrace(__FUNCTION__);

	return _ZwQueryInformationThread(ThreadHandle, ThreadInformationClass, ThreadInformation, ThreadInformationLength, ReturnLength);
}

void hide_being_debugged()
{
	auto* const peb = PPEB(__readgsqword(0x60));
	peb->BeingDebugged = false;
	*reinterpret_cast<PDWORD>(LPSTR(peb) + 0xBC) &= ~0x70;
}

NTSTATUS NtQueryInformationProcess_Detour(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength) {

	//backtrace(__FUNCTION__);
	hide_being_debugged();
	const auto status = _NtQueryInformationProcess(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);

	if (NT_SUCCESS(status))
	{
		if (ProcessInformationClass == ProcessBasicInformation)
		{
			static DWORD explorer_pid = 0;
			if (!explorer_pid)
			{
				auto* const shell_window = GetShellWindow();
				GetWindowThreadProcessId(shell_window, &explorer_pid);
			}

			static_cast<PPROCESS_BASIC_INFORMATION>(ProcessInformation)->Reserved3 = PVOID(DWORD64(explorer_pid));
		}
		else if (ProcessInformationClass == 30) // ProcessDebugObjectHandle
		{
			*static_cast<HANDLE*>(ProcessInformation) = nullptr;

			return 0xC0000353;
		}
		else if (ProcessInformationClass == 7) // ProcessDebugPort
		{
			*static_cast<HANDLE*>(ProcessInformation) = nullptr;
		}
		else if (ProcessInformationClass == 31)
		{
			*static_cast<ULONG*>(ProcessInformation) = 1;
		}
	}

	return status;
}

NTSTATUS NtClose_Detour(HANDLE Handle) {

	char info[16];
	if (NtQueryObject(Handle, OBJECT_INFORMATION_CLASS(4), &info, 2, nullptr) >= 0)
	{
		return _NtClose(Handle);
	}

	return STATUS_INVALID_HANDLE;
}

NTSTATUS ZwTerminateProcess_Detour(HANDLE ProcessHandle, NTSTATUS ExitStatus) {

	//backtrace(__FUNCTION__);
	printf("closing process %p with %i\n", ProcessHandle, ExitStatus);
	return _ZwTerminateProcess(ProcessHandle, ExitStatus);
}

utils::hook::detour enumwindows;
BOOL EnumWindows_Detour(WNDENUMPROC lpEnumFunc, LPARAM lParam) {

	printf("%p - %p\n", lpEnumFunc, lParam);

	return enumwindows.invoke<BOOL>(lpEnumFunc, lParam);
}

void find_and_hook_mapped_syscalls() {
	return;
	uintptr_t zwqueryinformationthread = find_pattern("4C 8B D1 B8 25 00 00 00 F6 04 25 ?? ?? ?? ?? ?? 75 03 0F 05 C3 CD 2E C3");
	while (zwqueryinformationthread != 0) {
		printf("0x%p zwqueryinformationthread mapped function\n", zwqueryinformationthread);
		utils::hook::jump(zwqueryinformationthread, ZwQueryInformationThread_Detour);
		zwqueryinformationthread = find_pattern(zwqueryinformationthread + 0x10, "4C 8B D1 B8 25 00 00 00 F6 04 25 ?? ?? ?? ?? ?? 75 03 0F 05 C3 CD 2E C3");
	}

	uintptr_t ntqueryinformationprocess = find_pattern("4C 8B D1 B8 19 00 00 00 F6 04 25 ?? ?? ?? ?? ?? 75 03 0F 05 C3 CD 2E C3");
	while (ntqueryinformationprocess != 0) {
		printf("0x%p ntqueryinformationprocess mapped function\n", ntqueryinformationprocess);
		utils::hook::jump(ntqueryinformationprocess, NtQueryInformationProcess_Detour);
		ntqueryinformationprocess = find_pattern(ntqueryinformationprocess + 0x10, "4C 8B D1 B8 19 00 00 00 F6 04 25 ?? ?? ?? ?? ?? 75 03 0F 05 C3 CD 2E C3");
	}

	uintptr_t ntclose = find_pattern("4C 8B D1 B8 0F 00 00 00 F6 04 25 ?? ?? ?? ?? ?? 75 03 0F 05 C3 CD 2E C3");
	while (ntclose != 0) {
		printf("0x%p ntclose mapped function\n", ntclose);
		utils::hook::jump(ntclose, NtClose_Detour);
		ntclose = find_pattern(ntclose + 0x10, "4C 8B D1 B8 0F 00 00 00 F6 04 25 ?? ?? ?? ?? ?? 75 03 0F 05 C3 CD 2E C3");
	}

	uintptr_t zwterminateprocess = find_pattern("4C 8B D1 B8 2C 00 00 00 F6 04 25 ?? ?? ?? ?? ?? 75 03 0F 05 C3 CD 2E C3");
	while (zwterminateprocess != 0) {
		printf("0x%p zwterminateprocess mapped function\n", zwterminateprocess);
		utils::hook::jump(zwterminateprocess, ZwTerminateProcess_Detour);
		zwterminateprocess = find_pattern(zwterminateprocess + 0x10, "4C 8B D1 B8 2C 00 00 00 F6 04 25 ?? ?? ?? ?? ?? 75 03 0F 05 C3 CD 2E C3");
	}

	uintptr_t _enumwindows = find_pattern("48 83 EC 48 C7 44 24 ?? ?? ?? ?? ?? 4C 8B CA 83 64 24 ?? ?? 4C 8B C1 83 64 24 ?? ?? 33 D2 33 C9 E8 ?? ?? ?? ?? 48 83 C4 48 C3");
	while (_enumwindows != 0) {
		printf("0x%p enumwindows mapped function\n", _enumwindows);
		utils::hook::jump(zwterminateprocess, ZwTerminateProcess_Detour);
		_enumwindows = find_pattern(_enumwindows + 0x10, "48 83 EC 48 C7 44 24 ?? ?? ?? ?? ?? 4C 8B CA 83 64 24 ?? ?? 4C 8B C1 83 64 24 ?? ?? 33 D2 33 C9 E8 ?? ?? ?? ?? 48 83 C4 48 C3");
	}

}

const char* getBuildNumStr()
{
	switch (*reinterpret_cast<int*>(0x1411017B0_g)) {
	case 0xB8ECE9C1:
		return "1.9.1";
	case 0xE9CC1D0E:
		return "1.33.2.15030131";
	case 0x8348B74E:
		return "1.34.0.15931218";
	default:
		return nullptr;
	}
	return nullptr;
}

void BuildWarning()
{
	const char* warning = "The Cold War build your using isn't fully supported.\nThe fully supported build is 1.34.0.15931218";
	const char* buildNum = getBuildNumStr();
	if (buildNum != nullptr)
	{
		char detailedWarning[256];
		sprintf(detailedWarning, "The Cold War build your using isn't fully supported. (%s)\nThe fully supported build is 1.34.0.15931218", buildNum);
		warning = detailedWarning;
	}

	MessageBoxA(NULL, warning, "T9 - WARNING", MB_ICONWARNING | MB_OK | MB_DEFBUTTON1);
}

void conMenu()
{
	bool s_inited = *reinterpret_cast<bool*>(g_Addrs.s_inited);
	while (s_inited == false)
	{
		Sleep(100);
		s_inited = *reinterpret_cast<bool*>(g_Addrs.s_inited);
	}

	puts("--------------------------------------------");
	puts("Use the game Main Menu to switch game modes!");
	puts("--------------------------------------------");
	puts("RCTRL + NUMPAD 1 = Add 1 bot to lobby");
	puts("RCTRL + NUMPAD 2 = Set Username");
	puts("RCTRL + NUMPAD 3 = Command Buffer");
	puts("RCTRL + NUMPAD 4 = Set Map");
	puts("RCTRL + NUMPAD 5 = Set Gametype");
	puts("--------------------------------------------");

	uintptr_t noDW = *reinterpret_cast<unsigned long long*>(g_Addrs.dvar_noDW);
	Dvar_SetBoolFromSource(noDW, 1, 0); // unlocks campaign and zombies

	if (*reinterpret_cast<int*>(0x1411017B0_g) != 0x8348B74E)
	{
		CreateThread((LPSECURITY_ATTRIBUTES)0, 0, (LPTHREAD_START_ROUTINE)BuildWarning, (LPVOID)0, 0, (LPDWORD)0x0);
	}
	else
	{
		static cmd_function_t cmd_addtestclient;
		Cmd_AddCommandInternal("addtestclient", AddTestClient, &cmd_addtestclient);

		Dvar_SetIntByHash(0x60CDC482A7D159F8, 1); // loot_fakeAll
	}

	CL_Disconnect(0, false, "");

	while (true)
	{
		if (GetAsyncKeyState(VK_RCONTROL) & 0x8000)
		{
			Sleep(100);

			if (GetAsyncKeyState(VK_NUMPAD1) < NULL)
			{
				LobbyHostBots_AddBotsToLobby(0, 1, false, 0, 0);
				puts("Added 1 bot to lobby...");
			}

			if (GetAsyncKeyState(VK_NUMPAD2) < NULL)
			{
				printf("Enter username: ");
				std::string input;
				std::getline(std::cin, input);
				if (input.length() > 1)
				{
					const char* name = input.c_str();
					set_username(LiveUser_GetUserDataForController(0), name);
				}
			}

			if (GetAsyncKeyState(VK_NUMPAD3) < NULL)
			{
				if (g_Addrs.Cbuf_AddText == 0x0)
				{
					if (g_Addrs.CbufAddtextFunc == 0x0)
					{
						puts("Cbuf not available...");
					}
					else
					{
						printf("Enter command: ");
						std::string input;
						std::getline(std::cin, input);
						if (input.length() > 1)
						{
							const char* cmd = input.c_str();
							Cbuf_AddText(cmd);
						}
					}
				}
				else
				{
					puts("1.34 build doesn't have this anyway, so no need to add that now...");
					puts("not gonna add it lol");
				}
			}

			if (GetAsyncKeyState(VK_NUMPAD4) < NULL)
			{
				printf("Enter map name: ");
				std::string input;
				std::getline(std::cin, input);
				if (input.length() > 1)
				{
					const char* mapname = input.c_str();
					LobbyData_SetMap(LobbySession_GetControllingLobbySession(0), mapname);
				}
			}

			if (GetAsyncKeyState(VK_NUMPAD5) < NULL)
			{
				printf("Enter gametype: ");
				std::string input;
				std::getline(std::cin, input);
				if (input.length() > 1)
				{
					const char* gametype = input.c_str();
					Com_GametypeSettings_SetGametype(gametype, true);
				}
			}
		}
	}
}

void setupCbufStuff()
{
	DWORD64 temp;

	const char* main_module = "BlackOpsColdWar.exe";

	g_Addrs.Cbuf_AddText = find_pattern(main_module, "48 89 5C 24 08 57 48 83 EC 30 45 33 C0 48 63 F9 48 8B DA 48 8D 4C 24 ?? 41 8D 5 0 ??");

	if (g_Addrs.Cbuf_AddText == 0)
	{
		g_Addrs.CbufAddtextFunc = find_pattern(main_module, "48 89 5C 24 08 55 56 57 41 54 41 55 41 56 41 57 48 8B EC 48 81 EC ?? ?? ?? ?? 48 8D 05 ?? ?? ?? ?? 48 89 45 ??");

		g_Addrs.cusCbufBuffer1 = find_pattern(main_module, "68 6F 73 74 6D 69 67 72 61 74 69 6F 6E 5F 73 74 61 72 74 0A"); // "hostmigration_start\n"

		g_Addrs.cusCbufBuffer2 = find_pattern(main_module, "63 68 65 63 6B 70 6F 69 6E 74 5F 72 65 73 74 61 72 74 0A"); // "checkpoint_restart\n"

		g_Addrs.cusCbufBuffer3 = find_pattern(main_module, "6D 69 73 73 69 6F 6E 5F 72 65 73 74 61 72 74 0A"); // "mission_restart\n"
	}
	temp = find_pattern(main_module, "41 B0 01 BA 01 00 00 00 8B 8D ?? ?? ?? ?? E8 ?? ?? ?? ?? ?? 8B ?? 41 8B ??");
	g_Addrs.LobbyHostBots_AddBotsToLobby = (DWORD64)(*(int*)(temp + 0xF) + 5) + temp + 0xE; // LobbyHostBots_AddBotsToLobby + 0xE
}

bool called_once;
void entry_point() {
	if (!called_once) {
		{
			puts("Initializing offsets, this will take a few seconds...");

			const char* main_module = "BlackOpsColdWar.exe";

			CreateThread((LPSECURITY_ATTRIBUTES)0, 0, (LPTHREAD_START_ROUTINE)setupCbufStuff, (LPVOID)0, 0, (LPDWORD)0x0);

			g_Addrs.Dvar_SetBoolFromSource = find_pattern(main_module, ("48 85 C9 0F 84 ?? ?? ?? ?? 48 89 5C 24 ?? 57 48 83 EC 60 0F 57 C0 44 0F B6 CA 48 B8 ?? ?? ?? ?? ?? ?? ?? ?? 41 8B F8 48 8B D9 0F 11 44 24 ?? 0F 11 44 24 ?? 48 85 01 0F 84 ?? ?? ?? ??"));
			if (g_Addrs.Dvar_SetBoolFromSource == NULL)
			{
				g_Addrs.Dvar_SetBoolFromSource = find_pattern(main_module, "48 85 C9 0F 84 ?? ?? ?? ?? 4C 8B DC 56 57 48 83 EC ?? 0F 57 C0 44 0F B6 CA 48 B8 ?? ?? ?? ?? ?? ?? ?? ?? 41 8B F0 48 8B F9 0F 11 44 24 ?? 0F 11 44 24 ?? 48 85 01 0F 84 ?? ?? ?? ??");
			}

			g_Addrs.s_uiScreen = find_pattern(main_module, ("8B 05 ?? ?? ?? ?? 3B C1 0F 45 C1 B9 ?? ?? ?? ?? 89 05 ?? ?? ?? ?? E8 ?? ?? ?? ??"));
			g_Addrs.s_uiScreen += *reinterpret_cast<unsigned int*>(g_Addrs.s_uiScreen + 2) + 6;
			
			g_Addrs.Dvar_SetStringFromSource = find_pattern(main_module, ("E8 ?? ?? ?? ?? 48 8B 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8D 4C 24 ?? 48 89 44 24 ?? E8 ?? ?? ?? ?? 48 8B 54 24 ?? 45 33 C0"));
			g_Addrs.Dvar_SetStringFromSource += *reinterpret_cast<signed int*>(g_Addrs.Dvar_SetStringFromSource + 1) + 5;
			
			g_Addrs.s_networkMode = find_pattern(main_module, ("89 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B CB E8 ?? ?? ?? ?? 8B C8 48 83 C4 20"));
			g_Addrs.s_networkMode += *reinterpret_cast<unsigned int*>(g_Addrs.s_networkMode + 2) + 6;

			g_Addrs.sSessionModeState = find_pattern(main_module, ("8B 05 ?? ?? ?? ?? C1 E0 18 C1 F8 1C 3B C1 0F 94 C0 C3"));
			g_Addrs.sSessionModeState += *reinterpret_cast<unsigned int*>(g_Addrs.sSessionModeState + 2) + 6;

			g_Addrs.config[0] = find_pattern(main_module, ("80 3D ?? ?? ?? ?? ?? 48 8D 15 ?? ?? ?? ?? 48 8B 3F 48 8B C8 48 0F 45 3D ?? ?? ?? ?? E8 ?? ?? ?? ??"));
			g_Addrs.config[0] += *reinterpret_cast<unsigned int*>(g_Addrs.config[0] + 2) + 7;

			g_Addrs.config[1] = find_pattern(main_module, ("80 3D ?? ?? ?? ?? ?? 75 58 33 C9 48 89 5C 24 ??"));
			g_Addrs.config[1] += *reinterpret_cast<unsigned int*>(g_Addrs.config[1] + 2) + 7;

			g_Addrs.CL_Disconnect = find_pattern(main_module, ("40 53 56 41 54 41 55 41 56 41 57 48 81 EC ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 84 24 ?? ?? ?? ?? 4D 8B F8"));
			g_Addrs.set_username = find_pattern(main_module, ("48 89 5C 24 ?? 57 48 83 EC 20 48 8B D9 4C 8B C2 BA ?? ?? ?? ?? 48 81 C1 ?? ?? ?? ??"));

			g_Addrs.LiveUser_GetUserDataForController = find_pattern(main_module, ("E8 ?? ?? ?? ?? 48 8B E8 33 C0 48 89 85 ?? ?? ?? ?? 48 8D 4D 10 48 89 85 ?? ?? ?? ?? 48 89 85 ?? ?? ?? ?? 89 85 ?? ?? ?? ??"));
			g_Addrs.LiveUser_GetUserDataForController += *reinterpret_cast<unsigned int*>(g_Addrs.LiveUser_GetUserDataForController + 1) + 5;

			g_Addrs.SetScreen = find_pattern(main_module, ("48 89 5C 24 ?? 48 89 74 24 ?? 55 57 41 54 41 56 41 57 48 8D 6C 24 ?? 48 81 EC ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 45 27 0F B6 FA 48 63 D9 8B 05 ?? ?? ?? ??"));
			g_Addrs.LobbyBase_SetNetworkMode = find_pattern(main_module, ("40 53 48 83 EC 20 8B D9 89 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B CB E8 ?? ?? ?? ??"));
			g_Addrs.Com_SessionMode_SetNetworkMode = find_pattern(main_module, ("8B 05 ?? ?? ?? ?? 8B D0 33 D1 83 E2 0F 33 C2 89 05 ?? ?? ?? ?? C3"));

			g_Addrs.LobbySession_GetControllingLobbySession = find_pattern(main_module, ("E8 ?? ?? ?? ?? 48 8B D7 8B C8 E8 ?? ?? ?? ?? 48 8B 7C 24 ?? E8 ?? ?? ?? ?? 8B CB E8 ?? ?? ?? ??"));
			g_Addrs.LobbySession_GetControllingLobbySession += *reinterpret_cast<int*>(g_Addrs.LobbySession_GetControllingLobbySession + 1) + 5;

			g_Addrs.LobbyData_SetMap = find_pattern(main_module, ("E8 ?? ?? ?? ?? 8B CB E8 ?? ?? ?? ?? 84 C0 74 13 8B CB E8 ?? ?? ?? ?? 8B C8 BA ?? ?? ?? ??"));
			g_Addrs.LobbyData_SetMap += *reinterpret_cast<signed int*>(g_Addrs.LobbyData_SetMap + 1) + 5;
			
			g_Addrs.LobbyData_SetGameType = find_pattern(main_module, ("E8 ?? ?? ?? ?? B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 C0 74 2D 33 D2 41 B8 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 33 D2 41 B8 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ??"));
			g_Addrs.LobbyData_SetGameType += *reinterpret_cast<signed int*>(g_Addrs.LobbyData_SetGameType + 1) + 5;

			g_Addrs.ScrStr_ConvertToString = find_pattern(main_module, ("E8 ?? ?? ?? ?? 8B 7B 30 45 33 C0 41 8D 54 24 ?? 48 8D 4C 24 ?? E8 ?? ?? ?? ?? 90"));
			g_Addrs.ScrStr_ConvertToString += *reinterpret_cast<signed int*>(g_Addrs.ScrStr_ConvertToString + 1) + 5;
			g_Addrs.VM_OP_Table = find_pattern(main_module, ("41 FF 94 FC ?? ?? ?? ??"));
			g_Addrs.VM_OP_Table = g_Addrs.ModuleBase + *reinterpret_cast<unsigned int*>(g_Addrs.VM_OP_Table + 4);
			g_Addrs.VM_OP_Notify_Handler = *reinterpret_cast<uintptr_t*>(g_Addrs.VM_OP_Table + 0x488);

			g_Addrs.Dvar_GetBool_cmp = find_pattern(main_module, ("83 79 ?? 01 8B 41 ?? 48 89 5C 24 ??"));
			
			g_Addrs.watermark_font = find_pattern(main_module, ("48 8B 05 ?? ?? ?? ?? 48 8B 5C 24 ?? 48 83 C4 30 5F C3"));
			g_Addrs.watermark_font += *reinterpret_cast<int*>(g_Addrs.watermark_font + 3) + 7;

			g_Addrs.dvar_show_over_stack = find_pattern(main_module, ("48 8B 0D ?? ?? ?? ?? 41 0F 28 F2 F3 0F 5E F0 C7 44 24 ?? ?? ?? ?? ?? 0F 28 05 ?? ?? ?? ?? 0F 28 FE 44 0F 28 C6 F3 0F 59 3D ?? ?? ?? ?? 44 0F 28 CE F3 0F 59 35 ?? ?? ?? ??"));
			g_Addrs.dvar_show_over_stack += *reinterpret_cast<int*>(g_Addrs.dvar_show_over_stack + 3) + 7;

			g_Addrs.CL_DrawTextPhysical = find_pattern(main_module, ("48 83 EC 78 F3 0F 10 84 24 ?? ?? ?? ?? F3 0F 10 8C 24 ?? ?? ?? ?? 8B 84 24 ?? ?? ?? ?? F3 0F 11 44 24 ?? F3 0F 10 84 24 ?? ?? ?? ?? C6 44 24 ?? ?? C7 44 24 ?? ?? ?? ?? ?? 89 44 24 48 48 8B 84 24 ?? ?? ?? ?? 48 89 44 24 ?? F3 0F 11 44 24 ?? F3 0F 10 84 24 ?? ?? ?? ?? F3 0F 11 4C 24 ?? F3 0F 10 8C 24 ?? ?? ?? ?? F3 0F 11 44 24 ?? F3 0F 11 4C 24 ?? E8 ?? ?? ?? ?? 48 83 C4 78 C3"));

			g_Addrs.ScrVm_ExecThread = find_pattern(main_module, ("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 41 56 41 57 48 83 EC 20 49 8B E9 48 63 F9 45 8B F0 48 8B F2 0F 18 0A 8B 54 24 60"));
			g_Addrs.ScrVar_ReleaseVariable = find_pattern(main_module, ("40 56 41 57 48 83 EC 48 44 8B FA 48 63 F1 85 D2 75 0A 33 C0 48 83 C4 48 41 5F 5E C3"));
			
			g_Addrs.Scr_GetFunctionHandle = find_pattern(main_module, ("E8 ?? ?? ?? ?? ?? ?? ?? 48 85 C0 75 0A B9 BF F3 CD 32 E8 ?? ?? ?? ??"));
			g_Addrs.Scr_GetFunctionHandle += *reinterpret_cast<signed int*>(g_Addrs.Scr_GetFunctionHandle + 1) + 5;
			
			g_Addrs.ScrVm_AddString = find_pattern(main_module, ("E8 ?? ?? ?? ?? EB 4A 45 33 C9 4C 8D 05 ?? ?? ?? ?? 8B D3 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? EB 32"));
			g_Addrs.ScrVm_AddString += *reinterpret_cast<signed int*>(g_Addrs.ScrVm_AddString + 1) + 5;

			g_Addrs.ScrVm_AddInt = find_pattern(main_module, ("48 89 5C 24 ?? 57 48 83 EC 20 48 63 D9 48 8B FA 8B CB E8 ?? ?? ?? ?? 4C 69 C3 ?? ?? ?? ?? 48 8B 5C 24 ?? 48 8D 05 ?? ?? ?? ?? 4C 03 C0 49 8B 00 48 8B 48 08 C7 41 ?? 06 00 00 00 49 8B 00 48 8B 48 08 48 89 39 48 83 C4 20 5F C3"));
			g_Addrs.Com_IsInGame = find_pattern(main_module, ("48 83 EC 28 E8 ?? ?? ?? ?? 84 C0 75 20 48 8B 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 C0 74 09 80 3D ?? ?? ?? ?? ?? 75 07 B0 01 48 83 C4 28 C3"));
			
			g_Addrs.clientthink_ret = find_pattern(main_module, ("41 8D 46 FF 48 8B CB 3B E8 49 8B C7 41 0F 94 C0"));

			g_Addrs.dvar_load_script = find_pattern(main_module, ("48 8B 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 C0 74 ?? E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 41 83 ?? 01"));
			g_Addrs.dvar_load_script += *reinterpret_cast<int*>(g_Addrs.dvar_load_script + 3) + 7;
			
			g_Addrs.Sys_IsServerThread = find_pattern(main_module, ("E8 ?? ?? ?? ?? 84 C0 75 42 F6 C3 10 74 09 E8 ?? ?? ?? ?? 84 C0 75 34"));
			g_Addrs.Sys_IsServerThread += *reinterpret_cast<signed int*>(g_Addrs.Sys_IsServerThread + 1) + 5;
			
			g_Addrs.Com_GametypeSettings_SetGametype = find_pattern(main_module, ("E9 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? 45 33 C0 B2 01 48 83 C4 28 E9 ?? ?? ?? ??"));
			g_Addrs.Com_GametypeSettings_SetGametype += *reinterpret_cast<signed int*>(g_Addrs.Com_GametypeSettings_SetGametype + 1) + 5;
			
			g_Addrs.Material_RegisterHandle = find_pattern(main_module, ("E8 ?? ?? ?? ?? 48 89 05 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 F8 0D 74 59 E8 ?? ?? ?? ?? 83 F8 0E 74 4F E8 ?? ?? ?? ??"));
			g_Addrs.Material_RegisterHandle += *reinterpret_cast<signed int*>(g_Addrs.Material_RegisterHandle + 1) + 5;

			g_Addrs.ScrPlace_GetViewUIContext = find_pattern(main_module, ("E8 ?? ?? ?? ?? F3 0F 10 0D ?? ?? ?? ?? BA ?? ?? ?? ?? F3 0F 10 05 ?? ?? ?? ?? 48 8B 1D ?? ?? ?? ?? 48 8B 35 ?? ?? ?? ?? F3 0F 59 C1 F3 0F 2D C8"));
			g_Addrs.ScrPlace_GetViewUIContext += *reinterpret_cast<signed int*>(g_Addrs.ScrPlace_GetViewUIContext + 1) + 5;
			
			g_Addrs.UI_GetFontHandle = find_pattern(main_module, ("48 89 5C 24 ?? 57 48 83 EC 30 8B DA 48 8B F9 83 FA 02 75 12 48 8B 05 ?? ?? ?? ?? 48 8B 5C 24 ?? 48 83 C4 30 5F C3"));
			
			g_Addrs.R_TextWidth = find_pattern(main_module, ("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 57 48 83 EC 30 8B E9 49 8B D9 49 8B C9 41 8B F8 48 8B F2 E8 ?? ?? ?? ??"));
			
			g_Addrs.R_AddCmdDrawStretchPic = find_pattern(main_module, ("E8 ?? ?? ?? ?? 48 8B 8B ?? ?? ?? ?? 4C 8D 8B ?? ?? ?? ?? 4C 8D 43 04 0F B7 81 ?? ?? ?? ?? 66 0F 6E C8 0F 5B C9 0F B7 81 ?? ?? ?? ?? 66 0F 6E C0 0F 5B C0"));
			g_Addrs.R_AddCmdDrawStretchPic += *reinterpret_cast<signed int*>(g_Addrs.R_AddCmdDrawStretchPic + 1) + 5;

			//g_Addrs.ClientEndFrame = find_pattern(main_module, ("E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? FF C7 48 81 C3 ?? ?? ?? ?? 3B F9 7C CF 4C 8B 05 ?? ?? ?? ??"));
			//g_Addrs.ClientEndFrame += *reinterpret_cast<int*>(g_Addrs.ClientEndFrame + 1) + 5;
			
			uintptr_t assetPool = find_pattern(main_module, ("48 8D 0D ?? ?? ?? ?? 48 03 C1 44 89 48 0C 41 FF C9 48 89 10 44 89 40 08 C6 40 10 00 44 89 50 14 48 89 50 18 45 85 C9 7E 1C 4D 63 C0 41 8B C9 49 8D 04 10 0F 1F 00"));
			assetPool += *reinterpret_cast<int*>(assetPool + 3) + 7;
			g_assetPool = (XAssetPool*)(assetPool);

			g_Addrs.g_auth_manager = find_pattern(main_module, ("2C 08 4E 3C AC 67 D3 78")) - 2;
			
			while (true) {
				g_Addrs.g_auth_manager--;
				if (*(unsigned char*)(g_Addrs.g_auth_manager) == 0x48 && *(unsigned char*)(g_Addrs.g_auth_manager + 1) == 0x89 && *(unsigned char*)(g_Addrs.g_auth_manager + 2) == 0x05) {
					break;
				}
			}
			g_Addrs.g_auth_manager += *reinterpret_cast<signed int*>(g_Addrs.g_auth_manager + 3) + 7;

			g_Addrs.dvar_noDW = find_pattern(main_module, "48 8B 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 F8 01 75 ?? 48 8B 0F 8D 68 ?? 44 8B F8 48 85 C9");
			g_Addrs.dvar_noDW = g_Addrs.dvar_noDW + (*(int*)(g_Addrs.dvar_noDW + 3) + 7);

			g_Addrs.s_inited = find_pattern(main_module, "38 ?? ?? ?? ?? ?? 0F 85 ?? ?? ?? ?? 48 ?? 4F B6 E2 AE 25 2E C1 17");
			g_Addrs.s_inited = g_Addrs.s_inited + (uintptr_t)(*(int*)(g_Addrs.s_inited + 2) + 6);

			g_Addrs.shader_white = find_pattern(main_module, "48 B9 06 E2 56 58 88 73 D9 4E 41 B8 FF FF FF FF B2 01 E8 ?? ?? ?? ?? 48");
			g_Addrs.shader_white = g_Addrs.shader_white + 40 + (uintptr_t)*(int*)(g_Addrs.shader_white + 36);
			
			CreateThread((LPSECURITY_ATTRIBUTES)0x0, 0, (LPTHREAD_START_ROUTINE)conMenu, (LPVOID)0x0, 0, (LPDWORD)0x0);

			set_username(LiveUser_GetUserDataForController(0), ("Player1"));

			//find_and_hook_mapped_syscalls();

			LPCSTR cmdline = GetCommandLineA();
			if (strstr(cmdline, ("--zombies")) || strstr(cmdline, ("--multiplayer")))
			{
				if (strstr(cmdline, ("--zombies")))
				{
					SetMode(0, 11);
					LobbyData_SetMap(LobbySession_GetControllingLobbySession(0), ("zm_silver"));

					Com_GametypeSettings_SetGametype(("zclassic"), true);
				}
				else
				{
					LobbyData_SetMap(LobbySession_GetControllingLobbySession(0), ("mp_moscow"));
					Com_GametypeSettings_SetGametype(("dm"), true);
					SetMode(1, 11);
				}
			}
			else
			{
				SetMode(0, 10);
			}

			//for (int i = 0; i < 55; i++) {
			//	*reinterpret_cast<uintptr_t*>(g_Addrs.VM_OP_Table + vm_notify_tables[i]) = (uintptr_t)(VM_OP_Notify_Handler_Hook);
			//}

			puts("Press dismiss on the next error message that comes up!");

			//uintptr_t present = find_pattern(("dxgi.dll"), ("55 57 41 56 48 8D 6C 24 ?? 48 81 EC ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 45 60"));

			//uintptr_t commandQueue = find_pattern(main_module, ("48 8B 05 ?? ?? ?? ?? 41 B8 ?? ?? ?? ?? 48 85 C0 74 0B 48 8B 16 48 8B C8 E8 ?? ?? ?? ??"));
			//commandQueue += *reinterpret_cast<int*>(commandQueue + 3) + 7;
			//g_pCommandQueue = *(ID3D12CommandQueue**)(commandQueue);

			g_Addrs.KeyboardInput = find_pattern(main_module, ("48 89 5C 24 ?? 48 89 6C 24 ?? 48 89 74 24 ?? 48 89 7C 24 ?? 41 56 48 83 EC 40 49 8B D8 4C 8B F2 8B E9 85 C9 0F 85 ?? ?? ?? ??"));

			uintptr_t exception_dispatcher = find_pattern(("ntdll.dll"), ("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ?? ?? ?? ?? 48 8D 6C 24 ?? 48 89 9D ?? ?? ?? ?? 48 8B 05 ?? ?? ?? ?? 48 33 C5 48 89 85 ?? ?? ?? ?? 65 48 8B 04 25 ?? ?? ?? ?? 33 DB"));
#ifdef DEV
			printf(("%p - exception_dispatcher\n"), exception_dispatcher);
#endif
			dispatch_exception.create(exception_dispatcher, RtlDispatchException_Detour);
			is_script_ready = true;
			//present_stub.create(present - 10, Present_Detour);
		}
		called_once = true;
	}
}

LRESULT WinProc_KeyboardInput(int code, WPARAM wParam, tagKBDLLHOOKSTRUCT* lParam)
{
	BYTE keyboardState[256];
	GetKeyboardState(keyboardState);
	WORD charCode;
	char pressedChar = NULL;
	if (ToAscii(lParam->vkCode, lParam->scanCode, keyboardState, &charCode, 0) == 1)
	{
		pressedChar = static_cast<char>(charCode);
	}

	if (wParam == WM_KEYDOWN)
	{
		if (lParam->scanCode == 41) {
			g_console.bopen = !g_console.bopen;
			memset(g_console.szBuffer, 0, 4096);
			const char* wmark = ("T9> ");
			g_console.start = strlen(wmark);
			strcpy(g_console.szBuffer, wmark);
		}

		if (g_console.bopen) {
			g_console.KeyboardHandle(wParam, lParam->vkCode, pressedChar);
		}

		// printf("WinProc_KeyboardInput: %p - %p - %p - %s - %i\n", code, wParam, lParam, &lParam->vkCode, lParam->vkCode);
	}

	if (!vars.bMenuOpen && !g_console.bopen) {
		if (g_Addrs.KeyboardInput) {
			return KeyboardInput(code, wParam, (LPARAM)lParam);
		}
		else {
			return 0;
		}
	}
	return 0;
}

HHOOK __stdcall SetWindowsHookExA_Detour(int idHook, HOOKPROC lpfn, HINSTANCE hmod, DWORD dwThreadId) {
	if (!is_alpha()) {
		if (idHook == 13) {
			lpfn = (LRESULT(*)(int, WPARAM, LPARAM))WinProc_KeyboardInput;
			// puts("hooked WinProc_KeyboardInput");
		}
	}
	return SetWindowsHookExA(idHook, lpfn, hmod, dwThreadId);
}

char s_word[64];
const char s_vowels[5] = { 'a','e','i','o','u' };
const char s_consonents[19] = { 'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 'n', 'p', 'r', 's', 't', 'v', 'w', 'y', 'z' };
const char* Com_GetRandomWord()
{
	srand(rand());
	for (int i = 0; i < 19;i++) {
		int a = rand();
		s_word[i] = s_consonents[a % 19];
	}
	return s_word;
}

utils::hook::detour createmutexexa;
HANDLE CreateMutexExA_Detour(const LPSECURITY_ATTRIBUTES attributes, const LPCSTR name, const DWORD flags, const DWORD access)
{
#ifdef DEV
	printf("0x%p - %s\n", (uintptr_t)(_ReturnAddress()) - base, name);
#endif
	if (strstr(name, ("$ IDA trusted_idbs")) || strstr(name, ("$ IDA registry mutex $")))
	{
		char* test = (char*)Com_GetRandomWord();
		//printf("%s\n", test);
		return createmutexexa.invoke<HANDLE>(attributes, test, flags, access);
	}

	return createmutexexa.invoke<HANDLE>(attributes, name, flags, access);
}
bool remove_evil_keywords_from_string(const UNICODE_STRING& string)
{
	static const std::wstring evil_keywords[] =
	{
		L"IDA",
		L"ida",
		L"HxD",
		L"cheatengine",
		L"Cheat Engine",
		L"x96dbg",
		L"x32dbg",
		L"x64dbg",
		L"Wireshark",
		L"Debug",
		L"DEBUG",
		L"msvsmon",
	};

	if (!string.Buffer || !string.Length)
	{
		return false;
	}

	const std::wstring_view path(string.Buffer, string.Length / sizeof(string.Buffer[0]));

	bool modified = false;
	for (const auto& keyword : evil_keywords)
	{
		while (true)
		{
			const auto pos = path.find(keyword);
			if (pos == std::wstring::npos)
			{
				break;
			}

			modified = true;

			for (size_t i = 0; i < keyword.size(); ++i)
			{
				string.Buffer[pos + i] = L'a';
			}
		}
	}

	return modified;
}
bool remove_evil_keywords_from_string(wchar_t* str, const size_t length)
{
	UNICODE_STRING unicode_string{};
	unicode_string.Buffer = str;
	unicode_string.Length = static_cast<uint16_t>(length);
	unicode_string.MaximumLength = unicode_string.Length;

	return remove_evil_keywords_from_string(unicode_string);
}
bool remove_evil_keywords_from_string(char* str, const size_t length)
{
	std::string_view str_view(str, length);
	std::wstring wstr(str_view.begin(), str_view.end());

	if (!remove_evil_keywords_from_string(wstr.data(), wstr.size()))
	{
		return false;
	}

	const std::string regular_str(wstr.begin(), wstr.end());
	memcpy(str, regular_str.data(), length);

	return true;
}
int WINAPI GetWindowTextA_Detour(const HWND wnd, const LPSTR str, const int max_count)
{
	std::wstring wstr{};
	wstr.resize(max_count);

	const auto res = GetWindowTextW(wnd, wstr.data(), max_count);
	if (res)
	{
		remove_evil_keywords_from_string(wstr.data(), res);

		const std::string regular_str(wstr.begin(), wstr.end());
		memset(str, 0, max_count);
		memcpy(str, regular_str.data(), res);
	}

	return res;
}

extern "C" __declspec(dllexport) BOOL WINAPI MiniDumpWriteDump(HANDLE hProcess, DWORD ProcessId, HANDLE hFile, uintptr_t DumpType, uintptr_t ExceptionParam, uintptr_t UserStreamParam, uintptr_t CallbackParam) {
	printf("%s\n", __FUNCTION__);
	return TRUE;
}

INT WINAPI get_system_metrics(int nIndex)
{
	if (!nIndex) {
#ifdef DEV
		printf("get_system_metrics %i\n", nIndex);
#endif
		//entry_point();
		//integrity::search_and_patch_integrity_checks();
	}
	return GetSystemMetrics(nIndex);
}

utils::hook::detour regopenkeya;
LSTATUS APIENTRY RegOpenKeyA_Detour( HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult) {

	printf("%s\n", lpSubKey);

	return regopenkeya.invoke<LSTATUS>(hKey, lpSubKey, phkResult);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD Reason, LPVOID lpVoid) {
	if (Reason == DLL_PROCESS_ATTACH)
	{
		if (IsProcessRunning("Battle.net.exe"))
		{
			MessageBoxA(NULL,
				"DO NOT launch the game via Battle.net or have Battle.net opened while launching the game with the custom discord_game_sdk.dll applied\n\n"
				"You are risking a BAN, so if you want to play Cold War normally via Battle.net: you need to delete the discord_game_sdk.dll in your game folder and replace it with the original one.\n\n"
				"But to launch Cold War with the custom discord_game_sdk.dll applied, you need to close Battle.net completely and just run 'BlackOpsColdWar.exe'.\n"
				"Not the 'Black Ops Cold War Launcher.exe'. This will open Battle.net!\n\n"
				"Press 'OK' to close the game", "T9 - WARNING", MB_ICONERROR | MB_OK | MB_DEFBUTTON1);

			ExitProcess(0x0);
			return TRUE;
		}

		AllocConsole();
		FILE* Dummy;
		freopen_s(&Dummy, ("CONOUT$"), ("w"), stdout);
		freopen_s(&Dummy, ("CONIN$"), ("r"), stdin);
		SetConsoleTitleW(L"T9 Console");

		utils::nt::library game{};
		utils::nt::library advapi32(("advapi32.dll"));
		utils::nt::library user32(("user32.dll"));
		utils::nt::library ntdll(("ntdll.dll"));
		utils::nt::library kernel32(("kernel32.dll"));
		utils::nt::library kernelbase(("kernelbase.dll"));

		const char* main_module = ("BlackOpsColdWar.exe");
		g_Addrs.ModuleBase = (uintptr_t)(GetModuleHandleA(0));
		printf(("Base Address: %p\n"), base);
		
		// utils::hook::set(game.get_iat_entry(("kernel32.dll"), ("QueryPerformanceCounter")), QueryPerformanceCounter_Detour);
		utils::hook::set(game.get_iat_entry(("user32.dll"), ("SetWindowsHookExA")), SetWindowsHookExA_Detour);
		/*createmutexexa.create(CreateMutexExA, CreateMutexExA_Detour);
		regopenkeya.create(advapi32.get_proc<uintptr_t>("RegOpenKeyA"), RegOpenKeyA_Detour);*/

		/*utils::hook::jump(GetWindowTextA, GetWindowTextA_Detour, true, true);
		utils::hook::move_hook(GetWindowTextA);*/

		//auto* get_thread_context_func = utils::nt::library("kernelbase.dll").get_proc<void*>("GetThreadContext");
		//get_thread_context_hook.create(get_thread_context_func, get_thread_context_stub);

		// patch_import("user32.dll", "GetSystemMetrics", get_system_metrics);
		// printf("hooked\n");
		
		
		//g_Addrs.jmp_rbx = find_pattern(main_module, "FF 23");
		
	}

	return TRUE;
}

uintptr_t* threadFunc;

uintptr_t find_pattern(const char* module_name, const char* pattern) {
	const auto get_module_size = [=](uintptr_t module_base)
	{
		return reinterpret_cast<PIMAGE_NT_HEADERS>(module_base + reinterpret_cast<PIMAGE_DOS_HEADER>(module_base)->e_lfanew)->OptionalHeader.SizeOfImage;
	};
	const auto module_start = (uintptr_t)GetModuleHandleA(module_name);
	if (module_start != 0ULL)
	{
		const auto module_end = module_start + get_module_size(module_start);

		const char* pattern_current = pattern;
		uintptr_t current_match = NULL;

		MEMORY_BASIC_INFORMATION64 page_information = {};
		for (auto current_page = reinterpret_cast<unsigned char*>(module_start); current_page < reinterpret_cast<unsigned char*>(module_end); current_page = reinterpret_cast<unsigned char*>(page_information.BaseAddress + page_information.RegionSize))
		{
			VirtualQuery(reinterpret_cast<LPCVOID>(current_page), reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&page_information), sizeof(MEMORY_BASIC_INFORMATION));
			if (page_information.Protect == PAGE_NOACCESS)
				continue;

			if (page_information.State != MEM_COMMIT)
				continue;

			if (page_information.Protect & PAGE_GUARD)
				continue;

			for (auto current_address = reinterpret_cast<unsigned char*>(page_information.BaseAddress); current_address < reinterpret_cast<unsigned char*>(page_information.BaseAddress + page_information.RegionSize - 0x8); current_address++)
			{
				if (*current_address != GET_BYTE(pattern_current) && *pattern_current != '\?') {
					current_match = 0ULL;
					pattern_current = pattern;
					continue;
				}

				if (!current_match)
					current_match = reinterpret_cast<uintptr_t>(current_address);

				pattern_current += 3;
				if (pattern_current[-1] == NULL)
					return current_match;
					//printf("found %s\n", pattern);
					
			}
		}
	}

	return 0ULL;
}
uintptr_t find_pattern(uintptr_t start, const char* module_name, const char* pattern) {
	const auto get_module_size = [=](uintptr_t module_base)
	{
		return reinterpret_cast<PIMAGE_NT_HEADERS>(module_base + reinterpret_cast<PIMAGE_DOS_HEADER>(module_base)->e_lfanew)->OptionalHeader.SizeOfImage;
	};
	const auto module_start = (uintptr_t)GetModuleHandleA(module_name);
	if (module_start != 0ULL)
	{
		const auto module_end = module_start + get_module_size(module_start);
		const char* pattern_current = pattern;
		uintptr_t current_match = NULL;

		MEMORY_BASIC_INFORMATION64 page_information = {};
		for (auto current_page = reinterpret_cast<unsigned char*>(start); current_page < reinterpret_cast<unsigned char*>(module_end); current_page = reinterpret_cast<unsigned char*>(page_information.BaseAddress + page_information.RegionSize))
		{
			VirtualQuery(reinterpret_cast<LPCVOID>(current_page), reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&page_information), sizeof(MEMORY_BASIC_INFORMATION));
			if (page_information.Protect == PAGE_NOACCESS)
				continue;

			if (page_information.State != MEM_COMMIT)
				continue;

			if (page_information.Protect & PAGE_GUARD)
				continue;

			for (auto current_address = reinterpret_cast<unsigned char*>(page_information.BaseAddress); current_address < reinterpret_cast<unsigned char*>(page_information.BaseAddress + page_information.RegionSize - 0x8); current_address++)
			{
				if (*current_address != GET_BYTE(pattern_current) && *pattern_current != '\?') {
					current_match = 0ULL;
					pattern_current = pattern;
					continue;
				}

				if (!current_match)
					current_match = reinterpret_cast<uintptr_t>(current_address);

				pattern_current += 3;
				if (pattern_current[-1] == NULL)
					return current_match;
			}
		}
	}

	return 0ULL;
}

uintptr_t find_pattern(const char* pattern) {
	const char* pattern_current = pattern;
	uintptr_t current_match = NULL;

	MEMORY_BASIC_INFORMATION64 page_information = {};
	for (auto current_page = reinterpret_cast<unsigned char*>(0); current_page < (unsigned char*)0x00007FFAFFFFFFFF; current_page = reinterpret_cast<unsigned char*>(page_information.BaseAddress + page_information.RegionSize))
	{
		VirtualQuery(reinterpret_cast<LPCVOID>(current_page), reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&page_information), sizeof(MEMORY_BASIC_INFORMATION));
		if (page_information.Protect == PAGE_NOACCESS)
			continue;

		if (page_information.State != MEM_COMMIT)
			continue;

		if (page_information.Protect & PAGE_GUARD)
			continue;

		for (auto current_address = reinterpret_cast<unsigned char*>(page_information.BaseAddress); current_address < reinterpret_cast<unsigned char*>(page_information.BaseAddress + page_information.RegionSize - 0x8); current_address++)
		{
			if (*current_address != GET_BYTE(pattern_current) && *pattern_current != '\?') {
				current_match = 0ULL;
				pattern_current = pattern;
				continue;
			}

			if (!current_match)
				current_match = reinterpret_cast<uintptr_t>(current_address);

			pattern_current += 3;
			if (pattern_current[-1] == NULL)
				return current_match;
		}
	}
	return 0;
}

uintptr_t find_pattern(uintptr_t start, const char* pattern) {
	const char* pattern_current = pattern;
	uintptr_t current_match = NULL;

	MEMORY_BASIC_INFORMATION64 page_information = {};
	for (auto current_page = reinterpret_cast<unsigned char*>(start); current_page < (unsigned char*)0x00007FFAFFFFFFFF; current_page = reinterpret_cast<unsigned char*>(page_information.BaseAddress + page_information.RegionSize))
	{
		VirtualQuery(reinterpret_cast<LPCVOID>(current_page), reinterpret_cast<PMEMORY_BASIC_INFORMATION>(&page_information), sizeof(MEMORY_BASIC_INFORMATION));
		if (page_information.Protect == PAGE_NOACCESS)
			continue;

		if (page_information.State != MEM_COMMIT)
			continue;

		if (page_information.Protect & PAGE_GUARD)
			continue;


		for (auto current_address = current_page; current_address < reinterpret_cast<unsigned char*>(page_information.BaseAddress + page_information.RegionSize - 0x8); current_address++)
		{
			if (*current_address != GET_BYTE(pattern_current) && *pattern_current != '\?') {
				current_match = 0ULL;
				pattern_current = pattern;
				continue;
			}

			if (!current_match)
				current_match = reinterpret_cast<uintptr_t>(current_address);

			pattern_current += 3;
			if (pattern_current[-1] == NULL)
				return current_match;
		}
	}
	return 0;
}

std::pair<void**, void*> patch_import(const std::string& lib, const std::string& func, void* function)
{
	static const utils::nt::library game{};

	const auto game_entry = game.get_iat_entry(lib, func);
	if (!game_entry)
	{
		throw std::runtime_error("Import '" + func + "' not found!");
	}

	const auto original_import = game_entry;
	utils::hook::set(game_entry, function);
	return { game_entry, original_import };
}

menu_variables vars;

uintptr_t cg_entitiesArray;
uintptr_t clients;

XAssetPool* g_assetPool;

extern "C"
{
	int s_read_arc4random(void*, size_t)
	{
		return -1;
	}

	int s_read_getrandom(void*, size_t)
	{
		return -1;
	}

	int s_read_urandom(void*, size_t)
	{
		return -1;
	}

	int s_read_ltm_rng(void*, size_t)
	{
		return -1;
	}
}

size_t operator"" _b(const size_t val)
{
	return base + val;
}

size_t reverse_b(const size_t val)
{
	return val - base;
}

size_t reverse_b(const void* val)
{
	return reverse_b(reinterpret_cast<size_t>(val));
}

size_t operator"" _g(const size_t val)
{
	return base + (val - 0x140000000);
}

size_t reverse_g(const size_t val)
{
	return (val - base) + 0x140000000;
}

size_t reverse_g(const void* val)
{
	return reverse_g(reinterpret_cast<size_t>(val));
}
