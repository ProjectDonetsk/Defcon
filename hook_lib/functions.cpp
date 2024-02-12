#include "functions.hpp"

int* s_messageDialoglInfoMode;


void Dvar_SetBoolFromSource(uintptr_t dvar, bool value, int source) {
	auto func = reinterpret_cast<void(*)(uintptr_t, bool, int)>(g_Addrs.Dvar_SetBoolFromSource);
	func(dvar, value, source);
}

void Dvar_SetStringFromSource(uintptr_t dvar, const char * value, int source) {
	auto func = reinterpret_cast<void(*)(uintptr_t, bool, int)>(g_Addrs.Dvar_SetStringFromSource);
	func(dvar, value, source);
}

void CL_Disconnect(int localClientNum, bool deactivateClient, const char* message) {
	auto func = reinterpret_cast<void(*)(int, bool, const char*)>(g_Addrs.CL_Disconnect);
	func(localClientNum, deactivateClient, message);
}

void set_username(uintptr_t a1, const char* username) {
	auto func = reinterpret_cast<void(*)(uintptr_t, const char*)>(g_Addrs.set_username);
	func(a1, username);
}

uintptr_t LiveUser_GetUserDataForController(int controllerIndex) {
	auto func = reinterpret_cast<uintptr_t(*)(int)>(g_Addrs.LiveUser_GetUserDataForController);
	return func(controllerIndex);
}

void SetScreen(int screen, char unk) {
	auto func = reinterpret_cast<void(*)(int, char)>(g_Addrs.SetScreen);
	func(screen, unk);
}

void LobbyBase_SetNetworkMode(LobbyNetworkMode networkMode) {
	auto func = reinterpret_cast<void(*)(LobbyNetworkMode)>(g_Addrs.LobbyBase_SetNetworkMode);
	func(networkMode);
}

void Com_SessionMode_SetMode(int mode) {
	auto func = reinterpret_cast<void(*)(int)>(g_Addrs.Com_SessionMode_SetNetworkMode);
	func(mode);
}

void Com_GametypeSettings_SetGametype(const char* gametype, bool loadDefaultSettings) {
	auto func = reinterpret_cast<void(*)(const char*, bool)>(g_Addrs.Com_GametypeSettings_SetGametype);
	func(gametype, loadDefaultSettings);
}

LRESULT KeyboardInput(int code, WPARAM wParam, LPARAM lParam) {
	auto func = reinterpret_cast<LRESULT(*)(int, WPARAM, LPARAM)>(g_Addrs.KeyboardInput);
	return func(code, wParam, lParam);
}

int LobbySession_GetControllingLobbySession(int lobbyModule) {
	auto func = reinterpret_cast<int(*)(int)>(g_Addrs.LobbySession_GetControllingLobbySession);
	return func(lobbyModule);
}

void LobbyData_SetMap(int lobbyType, const char* mapname) {
	auto func = reinterpret_cast<void(*)(int, const char*)>(g_Addrs.LobbyData_SetMap);
	func(lobbyType, mapname);
}

void LobbyData_SetGameType(int lobbyType, const char* gametype) {
	auto func = reinterpret_cast<void(*)(int, const char*)>(g_Addrs.LobbyData_SetGameType);
	func(lobbyType, gametype);
}

const char* ScrStr_ConvertToString(ScrString_t stringValue) {
	auto func = reinterpret_cast<const char* (*)(ScrString_t)>(g_Addrs.ScrStr_ConvertToString);
	return func(stringValue);
}

void VM_OP_Notify_Handler(int a1, __int64 a2, ScrVmContext_t* a3, __int64 a4) {
	auto func = reinterpret_cast<void(*)(int, __int64, ScrVmContext_t*, __int64)>(g_Addrs.VM_OP_Notify_Handler);
	func(a1, a2, a3, a4);
}

void SetMode(int mode, int menu) {
	SetScreen(menu, 0);
	LobbyBase_SetNetworkMode(LOBBY_NETWORKMODE_LAN);
	Com_SessionMode_SetMode(mode);
	if (!is_alpha()) {
		*reinterpret_cast<char*>(g_Addrs.config[0]) = 1;
		*reinterpret_cast<char*>(g_Addrs.config[1]) = 1;
	}
}

void* RtlAddVectoredExceptionHandler(LONG First, PVECTORED_EXCEPTION_HANDLER Handler) {
	utils::nt::library ntdll(("ntdll.dll"));
	ntdll.invoke<void*>(("RtlAddVectoredExceptionHandler"), First, Handler);
}

NTSTATUS NtContinue(PCONTEXT threadContext, BOOLEAN raiseAlert) {
	utils::nt::library ntdll(("ntdll.dll"));
	return ntdll.invoke<NTSTATUS>(("NtContinue"), threadContext, raiseAlert);
}

bool is_alpha() {
	if (*reinterpret_cast<int*>(base + 0x2602E18) == 0x5F1B8329) {
		return true;
	}
	else {
		return false;
	}
}

void CL_DrawTextPhysical(const char* text, int maxChars, uintptr_t font, float x, float y, float rotation, float xScale, float yScale, const float* color, int style, int padding) {
	auto func = reinterpret_cast<void(*)(const char* text, int maxChars, uintptr_t font, float x, float y, float rotation, float xScale, float yScale, const float* color, int style, int padding)>(g_Addrs.CL_DrawTextPhysical);
	func(text, maxChars, font, x, y, rotation, xScale, yScale, color, style, padding);
}

void R_AddCmdDrawStretchPic(float x, float y, float w, float h, float s0, float t0, float s1, float t1, const float* color, uintptr_t material) {
	auto func = reinterpret_cast<void(*)(float, float, float, float, float, float, float, float, const float*, uintptr_t)>(g_Addrs.R_AddCmdDrawStretchPic);
	func(x, y, w, h, s0, t0, s1, t1, color, material);
}

uintptr_t Material_RegisterHandle(uintptr_t hash, bool a2, int a3) {
	auto func = reinterpret_cast<uintptr_t(*)(uintptr_t, bool, int)>(g_Addrs.Material_RegisterHandle);
	return func((uintptr_t)&hash, a2, a3);
}

uintptr_t ScrPlace_GetViewUIContext(int a1) {
	auto func = reinterpret_cast<uintptr_t(*)(int)>(g_Addrs.ScrPlace_GetViewUIContext);
	return func(a1);
}

uintptr_t UI_GetFontHandle(uintptr_t a1, int a2, float a3) {
	auto func = reinterpret_cast<uintptr_t(*)(uintptr_t, int, float)>(g_Addrs.UI_GetFontHandle);
	return func(a1, a2, a3);
}

float R_TextWidth(int a1, const char* a2, int a3, uintptr_t a4) {
	auto func = reinterpret_cast<float(*)(int, const char*, int, uintptr_t)>(g_Addrs.R_TextWidth);
	return func(a1, a2, a3, a4);
}

int ScrVm_ExecThread(int inst, uintptr_t handle, unsigned int paramcount, ScrVarValue_t* value, unsigned int self) {
	auto func = reinterpret_cast<int(*)(int inst, uintptr_t handle, unsigned int paramcount, ScrVarValue_t * value, unsigned int self)>(g_Addrs.ScrVm_ExecThread);
	return func(inst, handle, paramcount, value, self);
}

void ScrVm_FreeThread(int inst, ScrVarIndex_t handle) {
	ScrVar_ReleaseVariable(inst, handle);
}

void ScrVar_ReleaseVariable(int inst, ScrVarIndex_t handle) {
	auto func = reinterpret_cast<void(*)(int, ScrVarIndex_t)>(g_Addrs.ScrVar_ReleaseVariable);
	func(inst, handle);
}

uintptr_t Scr_GetFunctionHandle(int inst, uintptr_t filename, uint32_t nspace, uint32_t nfunc, uint32_t* checksum, bool errorIfMissing) {
	auto func = reinterpret_cast<uintptr_t(*)(int inst, uintptr_t filename, uint32_t nspace, uint32_t func, uint32_t * checksum, bool errorIfMissing)>(g_Addrs.Scr_GetFunctionHandle);
	return func(inst, filename, nspace, nfunc, checksum, errorIfMissing);
}

void ScrVm_AddString(int inst, uintptr_t hash) {
	auto func = reinterpret_cast<void(*)(int, uintptr_t)>(g_Addrs.ScrVm_AddString);
	func(inst, hash);
}

void ScrVm_AddInt(int inst, int value) {
	auto func = reinterpret_cast<void(*)(int, int)>(g_Addrs.ScrVm_AddInt);
	func(inst, value);
}

bool Com_IsInGame() {
	auto func = reinterpret_cast<bool(*)()>(g_Addrs.Com_IsInGame);
	return func();
}

bool Sys_IsServerThread() {
	auto func = reinterpret_cast<bool(*)()>(g_Addrs.Sys_IsServerThread);
	return func();
}

float get_version() {
	switch (*reinterpret_cast<int*>(0x1411017B0_g)) {
	case 0xB8ECE9C1:
		return 1.918634551f;
	case 0xE9CC1D0E:
		return 1.33215030131f;
	}
	return 0.00000000f;
}

void LobbyHostBots_AddBotsToLobby(unsigned int lobbyType, int numBots, bool param_3, unsigned long lobbyID, unsigned long param_5)
{
	auto func = reinterpret_cast<void(*)(unsigned int lobbyType, int numBots, bool param_3, unsigned long lobbyID, unsigned long param_5)>(g_Addrs.LobbyHostBots_AddBotsToLobby);
	func(lobbyType, numBots, param_3, lobbyID, param_5);
}

void Cbuf_AddText(const char* cmd)
{
	auto func = reinterpret_cast<void(*)()>(g_Addrs.CbufAddtextFunc);

	DWORD dwProcessId = GetCurrentProcessId();
	HANDLE hProcess = OpenProcess(0x1fffff, 0, dwProcessId);

	unsigned int cmdlen = strlen(cmd) + 1;

	void* lVar2 = (void*)*(unsigned int*)(g_Addrs.CbufAddtextFunc + 0x1d);
	unsigned int iVar1 = *(unsigned int*)(g_Addrs.CbufAddtextFunc + (long long)lVar2 + 0x21);

	if (iVar1 == 0x74736F68)
	{
		WriteProcessMemory(hProcess, (LPVOID)g_Addrs.cusCbufBuffer1, cmd, cmdlen, NULL);
		func();
		WriteProcessMemory(hProcess, (LPVOID)g_Addrs.cusCbufBuffer1, "hostmigration_start\n", strlen("hostmigration_start\n"), NULL);
	}

	if (iVar1 == 0x63656863)
	{
		WriteProcessMemory(hProcess, (LPVOID)g_Addrs.cusCbufBuffer2, cmd, cmdlen, NULL);
		func();
		WriteProcessMemory(hProcess, (LPVOID)g_Addrs.cusCbufBuffer1, "checkpoint_restart\n", strlen("checkpoint_restart\n"), NULL);
	}

	if (iVar1 == 0x7373696D)
	{
		WriteProcessMemory(hProcess, (LPVOID)g_Addrs.cusCbufBuffer3, cmd, cmdlen, NULL);
		func();
		WriteProcessMemory(hProcess, (LPVOID)g_Addrs.cusCbufBuffer1, "mission_restart\n", strlen("mission_restart\n"), NULL);
	}
}

const uint64_t fnvHash(const char* data)
{
	uint64_t result = 0xCBF29CE484222325;

	for (size_t i = 0; i < strlen(data); i++)
	{
		uint64_t value = tolower(data[i]);

		if (value == '\\')
			value = '/';

		result = 0x100000001B3 * (value ^ result);
	}

	return result & 0x7FFFFFFFFFFFFFFF;
}

// build 1.34.0.15931218

dvar_t* Dvar_FindVar(__int64 dvar) {
	auto func = reinterpret_cast<dvar_t * (*)(__int64)>(0x14C0B2090_g);
	return func(dvar);
}
const dvar_t* Dvar_RegisterInt(__int64 hash, const char* dvarName, int value, int min, int max, unsigned int flags) {
	printf("[%s] Registering int 0x%llX with value %d\n", __FUNCTION__, hash, value);
	auto func = reinterpret_cast<const dvar_t * (*)(__int64, const char*, int, int, int, unsigned int)>(0x14C0C7360_g);
	return func(hash, dvarName, value, min, max, flags);
}
void Dvar_SetInt(dvar_t* dvar, int value, int a3) {
	auto func = reinterpret_cast<void(*)(dvar_t*, int, int)>(0x14C0B7B90_g);
	func(dvar, value, a3);
}

void Dvar_SetIntByHash(__int64 hash, int value) {
	dvar_t* dvar = Dvar_FindVar(hash);
	// if not found we should try to register it
	if (!dvar) {
		Dvar_RegisterInt(hash, 0, 1, 1, 1, 0);
	}
	else {
		Dvar_SetInt(dvar, value, 0);
	}
}

void Dvar_SetVariantByHash(__int64 hash, DvarValue value)
{
	auto Dvar_SetVariant = reinterpret_cast<void(*)(dvar_t* dvar, DvarValue value, int setSource)>(0xC0B8DB0_b);

	dvar_t* dvar = Dvar_FindVar(hash);
	if (dvar)
	{
		Dvar_SetVariant(dvar, value, 0);
	}
	else
	{
		printf("Dvar 0x%p doesn't exist.\n", hash);
	}
}

gentity_t* SV_AddTestClient(const char* name, const char* unk) {
	auto func = reinterpret_cast<gentity_t * (*)(const char*, const char*)>(0x147189410_g);
	return func(name, unk);
}

void Scr_AddEntity(scriptInstance_t inst, gentity_t* ent, unsigned int classNum) {
	auto func = reinterpret_cast<void(*)(scriptInstance_t, gentity_t*, unsigned int)>(0x14737F490_g);
	func(inst, ent, classNum);
}

void AddTestClient() {
	gentity_t* client = SV_AddTestClient(0, 0);
	if (client)
		Scr_AddEntity(SCRIPTINSTANCE_SERVER, client, 0);
}

void Cmd_AddCommandInternal(const char* cmdName, xcommand_t function, cmd_function_t* allocedCmd) {
	__int64 hash = fnvHash(cmdName);
	auto func = reinterpret_cast<void(*)(__int64, xcommand_t, cmd_function_t*)>(0x149ACBAD0_g);
	func(hash, function, allocedCmd);
}

Addresses g_Addrs;
