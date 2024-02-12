#pragma once
#include "Main.hpp"

#define HOOK_TYPE(x) 1 + x

#define SETUP_HOOK(original, x, xx, xxx) if (*reinterpret_cast<uintptr_t*>(x)) { \
original = *reinterpret_cast<uintptr_t*>(x); \
*reinterpret_cast<uintptr_t*>(x) = HOOK_TYPE(xx); \
/*nlog("%s setup\n", xxx);*/}

struct ScrVarValue_t;

enum zombie_vars : uintptr_t {
	zombie_point_scalar = 0x6FCAB2A3C50A4E87,
	zombie_insta_kill = 0x1CEA104D1E92CDC4,
	zombie_perk_cost = 0xABED6E2265973794,
	zombie_drop_item = 0x597E1388D63A735,
	zombie_powerup_drop_max_per_round = 0x64295B2016E31C22,
};

extern int hashes[535];
struct sys_error {
	int id;
	char buffer[4096];
};

enum Hooks {
	RENDERER,
	LOAD_SCRIPT,
};

struct Addresses {
	uintptr_t ModuleBase;
	uintptr_t jmp_rbx;

	uintptr_t s_uiScreen;
	uintptr_t s_networkMode;
	uintptr_t sSessionModeState;
	uintptr_t config[2];

	uintptr_t CL_Disconnect;
	uintptr_t set_username;
	uintptr_t LiveUser_GetUserDataForController;
	uintptr_t SetScreen;
	uintptr_t LobbyBase_SetNetworkMode;
	uintptr_t Com_SessionMode_SetNetworkMode;
	uintptr_t Com_GametypeSettings_SetGametype;
	uintptr_t Dvar_SetBoolFromSource;
	uintptr_t Dvar_SetStringFromSource;
	uintptr_t KeyboardInput;
	uintptr_t LobbySession_GetControllingLobbySession;
	uintptr_t LobbyData_SetMap;
	uintptr_t LobbyData_SetGameType;
	uintptr_t ScrStr_ConvertToString;
	uintptr_t VM_OP_Table;
	uintptr_t VM_OP_Notify_Handler;
	uintptr_t ClientEndFrame;
	uintptr_t Dvar_GetBool_cmp;
	uintptr_t watermark_font;
	uintptr_t dvar_show_over_stack;

	uintptr_t CL_DrawTextPhysical;

	uintptr_t R_AddCmdDrawStretchPic;
	uintptr_t ScrVm_ExecThread;
	uintptr_t ScrVar_ReleaseVariable;
	uintptr_t Scr_GetFunctionHandle;
	uintptr_t ScrVm_AddString;
	uintptr_t ScrVm_AddInt;
	uintptr_t Com_IsInGame;
	uintptr_t Sys_IsServerThread;
	uintptr_t Material_RegisterHandle;
	uintptr_t ScrPlace_GetViewUIContext;
	uintptr_t UI_GetFontHandle;
	uintptr_t R_TextWidth;

	uintptr_t clientthink_ret;
	uintptr_t dvar_load_script;
	uintptr_t g_auth_manager;

	uintptr_t shader_white;

	uintptr_t dvar_noDW;

	uintptr_t Cbuf_AddText;
	uintptr_t CbufAddtextFunc;
	uintptr_t cusCbufBuffer1;
	uintptr_t cusCbufBuffer2;
	uintptr_t cusCbufBuffer3;
	uintptr_t LobbyHostBots_AddBotsToLobby;
	uintptr_t s_inited;

};

enum ScrString_t;

enum LobbyNetworkMode
{
	LOBBY_NETWORKMODE_INVALID = 0xFFFFFFFF,
	LOBBY_NETWORKMODE_UNKNOWN = 0x0,
	LOBBY_NETWORKMODE_LAN = 0x1,
	LOBBY_NETWORKMODE_LIVE = 0x2,
	LOBBY_NETWORKMODE_COUNT = 0x3,
	LOBBY_NETWORKMODE_LOCAL = 0x1,
};

typedef uint32_t ScrVarIndex_t;
struct ScrVmContext_t;
struct dvar_t;
typedef void (*xcommand_t)(void);
struct cmd_function_t;
struct gentity_t;
enum scriptInstance_t;
struct DvarValue;

void Dvar_SetBoolFromSource(uintptr_t dvar, bool value, int source);
void Dvar_SetStringFromSource(uintptr_t dvar, const char* value, int source);

//Dvar_GetInt - E8 ? ? ? ? 85 C0 0F 85 ? ? ? ? 48 8B 0D ? ? ? ? E8 ? ? ? ? 84 C0 74 09 80 BB ? ? ? ? ? 74 72
//Dvar_FindVarByName - E8 ? ? ? ? 48 8B F8 48 85 C0 74 6F E8 ? ? ? ?
//Com_IsInGame - 48 83 EC 28 E8 ? ? ? ? 84 C0 75 20 48 8B 0D ? ? ? ? E8 ? ? ? ? 84 C0 74 09 80 3D ? ? ? ? ? 75 07 B0 01 48 83 C4 28 C3

void CL_Disconnect(int localClientNum, bool deactivateClient, const char* message);
void set_username(uintptr_t a1, const char* username);
uintptr_t LiveUser_GetUserDataForController(int controllerIndex);
void SetScreen(int screen, char unk);
void LobbyBase_SetNetworkMode(LobbyNetworkMode networkMode);
void Com_SessionMode_SetMode(int mode);
void Com_GametypeSettings_SetGametype(const char* gametype, bool loadDefaultSettings);
LRESULT KeyboardInput(int code, WPARAM wParam, LPARAM lParam);
int LobbySession_GetControllingLobbySession(int lobbyModule);
void LobbyData_SetMap(int lobbyType, const char* mapname);
void LobbyData_SetGameType(int lobbyType, const char* gametype);
const char* ScrStr_ConvertToString(ScrString_t stringValue);
void VM_OP_Notify_Handler(int a1, __int64 a2, ScrVmContext_t* a3, __int64 a4);

void SetMode(int mode, int menu);

void* RtlAddVectoredExceptionHandler(LONG First, PVECTORED_EXCEPTION_HANDLER Handler);
NTSTATUS NtContinue(PCONTEXT threadContext, BOOLEAN raiseAlert);

bool is_alpha();

void CL_DrawTextPhysical(const char* text, int maxChars, uintptr_t font, float x, float y, float rotation, float xScale, float yScale, const float* color, int style, int padding);
void R_AddCmdDrawStretchPic(float x, float y, float w, float h, float s0, float t0, float s1, float t1, const float* color, uintptr_t material);

uintptr_t Material_RegisterHandle(uintptr_t hash, bool a2, int a3);

uintptr_t ScrPlace_GetViewUIContext(int a1);
uintptr_t UI_GetFontHandle(uintptr_t a1, int a2, float a3);
float R_TextWidth(int a1, const char* a2, int a3, uintptr_t a4);

int ScrVm_ExecThread(int inst, uintptr_t handle, unsigned int paramcount, ScrVarValue_t* value, unsigned int self);
void ScrVm_FreeThread(int inst, ScrVarIndex_t handle);
void ScrVar_ReleaseVariable(int inst, ScrVarIndex_t id);
uintptr_t Scr_GetFunctionHandle(int inst, uintptr_t filename, uint32_t nspace, uint32_t nfunc, uint32_t* checksum, bool errorIfMissing);
void ScrVm_AddString(int inst, uintptr_t hash);
void ScrVm_AddInt(int inst, int value);
bool Com_IsInGame();
bool Sys_IsServerThread();

float get_version();

void LobbyHostBots_AddBotsToLobby(unsigned int lobbyType, int numBots, bool param_3, unsigned long lobbyID, unsigned long param_5);

void Cbuf_AddText(const char* cmd);

const uint64_t fnvHash(const char* data);

// build 1.34.0.15931218

dvar_t* Dvar_FindVar(__int64 dvar);

const dvar_t* Dvar_RegisterInt(__int64 hash, const char* dvarName, int value, int min, int max, unsigned int flags);

void Dvar_SetInt(dvar_t* dvar, int value, int a3);

void Dvar_SetIntByHash(__int64 hash, int value);

void Dvar_SetVariantByHash(__int64 hash, DvarValue value);

gentity_t* SV_AddTestClient(const char* name, const char* unk);

void Scr_AddEntity(scriptInstance_t inst, gentity_t* ent, unsigned int classNum);

void AddTestClient();

void Cmd_AddCommandInternal(const char* cmdName, xcommand_t function, cmd_function_t* allocedCmd);

extern Addresses g_Addrs;
