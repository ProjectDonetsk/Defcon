#pragma once
#include <Windows.h>
#include <winternl.h>
#include <fstream>
#include <string>
#include <iostream>
#include <intrin.h>

#include <dxgi1_5.h>
#include <d3d12.h>

//#define PROTECT

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Winmm.lib")

#pragma warning(disable:4996)

#ifdef PROTECT 
#pragma comment(lib, "SecureEngineSDK64.lib")
#include "Themida/ThemidaSDK.h"
#endif
#include "xorstr.hpp"



#define Xors(x) XorStr(x)

#include "common/utils/hook.hpp"
#include "common/utils/hardware_breakpoint.hpp"
#include "spoof_call.h"
#include "functions.hpp"

#include "hooks.h"
#include "console.h"

struct WeaponDef {
	uintptr_t szInternalName;
	uintptr_t unk0000;
	uintptr_t weapName1;
	uintptr_t unk0001;
	uintptr_t weapName2;
};

//#define TEST_HOOKS

#pragma comment(lib, "ntdll.lib")

#define base g_Addrs.ModuleBase

#pragma warning(disable:4996)

void entry_point();

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern uintptr_t* threadFunc;
#define INRANGE(x, a, b)   (x >= a && x <= b)
#define GET_BITS( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define GET_BYTE( x )    (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))

inline bool IsBadPointer(uintptr_t* ptr)
{
	__try
	{
		volatile auto result = *ptr;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return true;
	}
	return false;
}

extern "C" NTSTATUS _ZwQueryInformationThread(HANDLE ThreadHandle, THREADINFOCLASS ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength, PULONG ReturnLength);
extern "C" NTSTATUS _NtQueryInformationProcess(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);
extern "C" NTSTATUS _NtClose(HANDLE Handle);
extern "C" NTSTATUS _NtUserBuildHwndList(HDESK hDesktop, HWND hwndParent, BOOLEAN bChildren, ULONG dwThreadId, ULONG cHwnd, HWND * phwndList, ULONG * pcHwndNeeded);
extern "C" NTSTATUS _ZwTerminateProcess(HANDLE ProcessHandle, NTSTATUS ExitStatus);

struct menu_variables {
	bool bInitiateMenu;
	bool bMenuOpen;
	bool bAllPlayersSpawnedOnce;
	char mapname[32];
	char name[32];
};
extern menu_variables vars;
enum dvarType_t : __int32
{
	DVAR_TYPE_INVALID = 0,
	DVAR_TYPE_BOOL = 1,
	DVAR_TYPE_FLOAT = 2,
	DVAR_TYPE_FLOAT_2 = 3,
	DVAR_TYPE_FLOAT_3 = 4,
	DVAR_TYPE_FLOAT_4 = 5,
	DVAR_TYPE_INT = 6,
	DVAR_TYPE_ENUM = 7,
	DVAR_TYPE_STRING = 8,
	DVAR_TYPE_COLOR = 9,
	DVAR_TYPE_INT64 = 10,
	DVAR_TYPE_UINT64 = 11,
	DVAR_TYPE_LINEAR_COLOR_RGB = 12,
	DVAR_TYPE_COLOR_XYZ = 13,
	DVAR_TYPE_COLOR_LAB = 14,
	DVAR_TYPE_SESSIONMODE_BASE_DVAR = 15,
	DVAR_TYPE_COUNT = 16,
};

//don't remember what update I dumped this from, but it most likely changed since whenever update it was from
enum XAssetType : unsigned __int8
{
	ASSET_TYPE_ZONE = 0x0,
	ASSET_TYPE_ASSETLIST = 0x1,
	ASSET_TYPE_PHYSPRESET = 0x2,
	ASSET_TYPE_PHYSCONSTRAINTS = 0x3,
	ASSET_TYPE_DESTRUCTIBLEDEF = 0x4,
	ASSET_TYPE_XANIM = 0x5,
	ASSET_TYPE_XMODEL = 0x6,
	ASSET_TYPE_XCOLLISION = 0x7,
	ASSET_TYPE_XSKELETON = 0x8,
	ASSET_TYPE_XMODELMESH = 0x9,
	ASSET_TYPE_MATERIAL = 0xA,
	ASSET_TYPE_CSDEF = 0xB,
	ASSET_TYPE_COMPUTESHADERSET = 0xC,
	ASSET_TYPE_RTSDEF = 0xD,
	ASSET_TYPE_RAYTRACESHADERSET = 0xE,
	ASSET_TYPE_TECHSET = 0xF,
	ASSET_TYPE_IMAGE = 0x10,
	ASSET_TYPE_SOUND = 0x11,
	ASSET_TYPE_SOUND_BANK = 0x12,
	ASSET_TYPE_SOUND_ASSET = 0x13,
	ASSET_TYPE_SOUND_DUCK = 0x14,
	ASSET_TYPE_SOUND_ALIAS_MODIFIER = 0x15,
	ASSET_TYPE_SOUND_ACOUSTICS = 0x16,
	ASSET_TYPE_COL_MAP = 0x17,
	ASSET_TYPE_CLIP_MAP = 0x18,
	ASSET_TYPE_COM_MAP = 0x19,
	ASSET_TYPE_GAME_MAP = 0x1A,
	ASSET_TYPE_GFX_MAP = 0x1B,
	ASSET_TYPE_FONTICON = 0x1C,
	ASSET_TYPE_LOCALIZEENTRY = 0x1D,
	ASSET_TYPE_GESTURE = 0x1E,
	ASSET_TYPE_GESTURETABLE = 0x1F,
	ASSET_TYPE_CINEMATICMOTION = 0x20,
	ASSET_TYPE_WEAPON = 0x21,
	ASSET_TYPE_WEAPONFULL = 0x22,
	ASSET_TYPE_WEAPONFRONTEND = 0x23,
	ASSET_TYPE_WEAPONBLUEPRINT = 0x24,
	ASSET_TYPE_WEAPONSTYLESETTINGS = 0x25,
	ASSET_TYPE_WEAPONSECONDARYMOVEMENT = 0x26,
	ASSET_TYPE_WEAPONTUNABLES = 0x27,
	ASSET_TYPE_CGMEDIATABLE = 0x28,
	ASSET_TYPE_PLAYERSOUNDSTABLE = 0x29,
	ASSET_TYPE_PLAYERFXTABLE = 0x2A,
	ASSET_TYPE_SHAREDWEAPONSOUNDS = 0x2B,
	ASSET_TYPE_ATTACHMENT = 0x2C,
	ASSET_TYPE_ATTACHMENTUNIQUE = 0x2D,
	ASSET_TYPE_WEAPONCAMO = 0x2E,
	ASSET_TYPE_WEAPONCAMOBINDING = 0x2F,
	ASSET_TYPE_CUSTOMIZATIONTABLE = 0x30,
	ASSET_TYPE_CUSTOMIZATIONTABLEFRONTEND = 0x31,
	ASSET_TYPE_SNDDRIVERGLOBALS = 0x32,
	ASSET_TYPE_FX = 0x33,
	ASSET_TYPE_TAGFX = 0x34,
	ASSET_TYPE_KLF = 0x35,
	ASSET_TYPE_IMPACTSFXTABLE = 0x36,
	ASSET_TYPE_IMPACTSOUNDSTABLE = 0x37,
	ASSET_TYPE_AITYPE = 0x38,
	ASSET_TYPE_CHARACTER = 0x39,
	ASSET_TYPE_XMODELALIAS = 0x3A,
	ASSET_TYPE_RAWFILE = 0x3B,
	ASSET_TYPE_RAWFILEPREPROC = 0x3C,
	ASSET_TYPE_RAWTEXTFILE = 0x3D,
	ASSET_TYPE_ANIMTREE = 0x3E,
	ASSET_TYPE_STRINGTABLE = 0x3F,
	ASSET_TYPE_STRUCTUREDTABLE = 0x40,
	ASSET_TYPE_LEADERBOARDDEF = 0x41,
	ASSET_TYPE_DDL = 0x42,
	ASSET_TYPE_GLASSES = 0x43,
	ASSET_TYPE_SCRIPTPARSETREE = 0x44,
	ASSET_TYPE_SCRIPTPARSETREEDBG = 0x45,
	ASSET_TYPE_SCRIPT_USING = 0x46,
	ASSET_TYPE_SCRIPT_USING_CP = 0x47,
	ASSET_TYPE_SCRIPT_USING_MP = 0x48,
	ASSET_TYPE_SCRIPT_USING_WZ = 0x49,
	ASSET_TYPE_SCRIPT_USING_ZM = 0x4A,
	ASSET_TYPE_KEYVALUEPAIRS = 0x4B,
	ASSET_TYPE_VEHICLE = 0x4C,
	ASSET_TYPE_TRACER = 0x4D,
	ASSET_TYPE_SURFACEFXTABLE = 0x4E,
	ASSET_TYPE_SURFACESOUNDDEF = 0x4F,
	ASSET_TYPE_FOOTSTEPTABLE = 0x50,
	ASSET_TYPE_ENTITYFXIMPACTS = 0x51,
	ASSET_TYPE_ENTITYSOUNDIMPACTS = 0x52,
	ASSET_TYPE_ZBARRIER = 0x53,
	ASSET_TYPE_VEHICLEFXDEF = 0x54,
	ASSET_TYPE_VEHICLESOUNDDEF = 0x55,
	ASSET_TYPE_TYPEINFO = 0x56,
	ASSET_TYPE_SCRIPTBUNDLE = 0x57,
	ASSET_TYPE_SCRIPTBUNDLELIST = 0x58,
	ASSET_TYPE_RUMBLE = 0x59,
	ASSET_TYPE_BULLETPENETRATION = 0x5A,
	ASSET_TYPE_LOCDMGTABLE = 0x5B,
	ASSET_TYPE_AIMTABLE = 0x5C,
	ASSET_TYPE_SHOOTTABLE = 0x5D,
	ASSET_TYPE_PLAYERGLOBALTUNABLES = 0x5E,
	ASSET_TYPE_OVERHEADCAMERATUNABLES = 0x5F,
	ASSET_TYPE_ANIMSELECTORTABLE = 0x60,
	ASSET_TYPE_ANIMMAPPINGTABLE = 0x61,
	ASSET_TYPE_ANIMSTATEMACHINE = 0x62,
	ASSET_TYPE_BEHAVIORTREE = 0x63,
	ASSET_TYPE_BEHAVIORSTATEMACHINE = 0x64,
	ASSET_TYPE_TTF = 0x65,
	ASSET_TYPE_SANIM = 0x66,
	ASSET_TYPE_SHELLSHOCK = 0x67,
	ASSET_TYPE_STATUSEFFECT = 0x68,
	ASSET_TYPE_CINEMATIC_CAMERA = 0x69,
	ASSET_TYPE_CINEMATIC_SEQUENCE = 0x6A,
	ASSET_TYPE_SPECTATE_CAMERA = 0x6B,
	ASSET_TYPE_XCAM = 0x6C,
	ASSET_TYPE_BGCACHE = 0x6D,
	ASSET_TYPE_FLAMETABLE = 0x6E,
	ASSET_TYPE_BITFIELD = 0x6F,
	ASSET_TYPE_MAPTABLE = 0x70,
	ASSET_TYPE_MAPTABLEENTRY = 0x71,
	ASSET_TYPE_MAPTABLELIST = 0x72,
	ASSET_TYPE_OBJECTIVE = 0x73,
	ASSET_TYPE_OBJECTIVELIST = 0x74,
	ASSET_TYPE_NAVMESH = 0x75,
	ASSET_TYPE_NAVVOLUME = 0x76,
	ASSET_TYPE_LASER = 0x77,
	ASSET_TYPE_BEAM = 0x78,
	ASSET_TYPE_STREAMERHINT = 0x79,
	ASSET_TYPE_FLOWGRAPH = 0x7A,
	ASSET_TYPE_POSTFXBUNDLE = 0x7B,
	ASSET_TYPE_LUAFILE = 0x7C,
	ASSET_TYPE_LUAFILEDEBUG = 0x7D,
	ASSET_TYPE_RENDEROVERRIDEBUNDLE = 0x7E,
	ASSET_TYPE_STATICLEVELFXLIST = 0x7F,
	ASSET_TYPE_TRIGGERLIST = 0x80,
	ASSET_TYPE_PLAYERROLETEMPLATE = 0x81,
	ASSET_TYPE_PLAYERROLETEMPLATEFRONTEND = 0x82,
	ASSET_TYPE_PLAYERROLECATEGORYTABLE = 0x83,
	ASSET_TYPE_PLAYERROLECATEGORY = 0x84,
	ASSET_TYPE_CHARACTERBODYTYPE = 0x85,
	ASSET_TYPE_CHARACTERBODYTYPEFRONTEND = 0x86,
	ASSET_TYPE_PLAYEROUTFIT = 0x87,
	ASSET_TYPE_GAMETYPETABLE = 0x88,
	ASSET_TYPE_GAMETYPETABLEENTRY = 0x89,
	ASSET_TYPE_FEATURE = 0x8A,
	ASSET_TYPE_FEATURETABLE = 0x8B,
	ASSET_TYPE_UNLOCKABLEITEM = 0x8C,
	ASSET_TYPE_UNLOCKABLEITEMTABLE = 0x8D,
	ASSET_TYPE_ENTITYLIST = 0x8E,
	ASSET_TYPE_PLAYLISTS = 0x8F,
	ASSET_TYPE_PLAYLISTGLOBALSETTINGS = 0x90,
	ASSET_TYPE_PLAYLISTSCHEDULE = 0x91,
	ASSET_TYPE_MOTIONMATCHINGINPUT = 0x92,
	ASSET_TYPE_BLACKBOARD = 0x93,
	ASSET_TYPE_TACTICALQUERY = 0x94,
	ASSET_TYPE_PLAYERMOVEMENTTUNABLES = 0x95,
	ASSET_TYPE_HIERARCHICALTASKNETWORK = 0x96,
	ASSET_TYPE_RAGDOLL = 0x97,
	ASSET_TYPE_STORAGEFILE = 0x98,
	ASSET_TYPE_STORAGEFILELIST = 0x99,
	ASSET_TYPE_CHARMIXER = 0x9A,
	ASSET_TYPE_STOREPRODUCT = 0x9B,
	ASSET_TYPE_STORECATEGORY = 0x9C,
	ASSET_TYPE_STORECATEGORYLIST = 0x9D,
	ASSET_TYPE_RANK = 0x9E,
	ASSET_TYPE_RANKTABLE = 0x9F,
	ASSET_TYPE_PRESTIGE = 0xA0,
	ASSET_TYPE_PRESTIGETABLE = 0xA1,
	ASSET_TYPE_FIRSTPARTYENTITLEMENT = 0xA2,
	ASSET_TYPE_FIRSTPARTYENTITLEMENTLIST = 0xA3,
	ASSET_TYPE_ENTITLEMENT = 0xA4,
	ASSET_TYPE_ENTITLEMENTLIST = 0xA5,
	ASSET_TYPE_SKU = 0xA6,
	ASSET_TYPE_LABELSTORE = 0xA7,
	ASSET_TYPE_LABELSTORELIST = 0xA8,
	ASSET_TYPE_CPU_OCCLUSION_DATA = 0xA9,
	ASSET_TYPE_LIGHTING = 0xAA,
	ASSET_TYPE_DISTRICTS = 0xAB,
	ASSET_TYPE_STREAMERWORLD = 0xAC,
	ASSET_TYPE_TALENT = 0xAD,
	ASSET_TYPE_PLAYERTALENTTEMPLATE = 0xAE,
	ASSET_TYPE_PLAYERANIMATION = 0xAF,
	ASSET_TYPE_UNUSED = 0xB0,
	ASSET_TYPE_TERRAINGFX = 0xB1,
	ASSET_TYPE_HIGHLIGHTREELINFODEFINES = 0xB2,
	ASSET_TYPE_HIGHLIGHTREELPROFILEWEIGHTING = 0xB3,
	ASSET_TYPE_HIGHLIGHTREELSTARLEVELS = 0xB4,
	ASSET_TYPE_DLOGEVENT = 0xB5,
	ASSET_TYPE_RAWSTRING = 0xB6,
	ASSET_TYPE_BALLISTICDESC = 0xB7,
	ASSET_TYPE_STREAMKEY = 0xB8,
	ASSET_TYPE_RENDERTARGETS = 0xB9,
	ASSET_TYPE_DRAWNODES = 0xBA,
	ASSET_TYPE_GROUPLODMODEL = 0xBB,
	ASSET_TYPE_FXLIBRARYVOLUME = 0xBC,
	ASSET_TYPE_ARENASEASONS = 0xBD,
	ASSET_TYPE_SPRAYORGESTUREITEM = 0xBE,
	ASSET_TYPE_SPRAYORGESTURELIST = 0xBF,
	ASSET_TYPE_HWPLATFORM = 0xC0,
	ASSET_TYPE_ATTACHMENTTABLE = 0xC1,
	ASSET_TYPE_NAVINPUT = 0xC2,
	ASSET_TYPE_UIMODELDATASTRUCT = 0xC3,
	ASSET_TYPE_CRAFTICON = 0xC4,
	ASSET_TYPE_CRAFTICONLIST = 0xC5,
	ASSET_TYPE_CRAFTWEAPONSTICKER = 0xC6,
	ASSET_TYPE_CRAFTWEAPONSTICKERLIST = 0xC7,
	ASSET_TYPE_CRAFTBACKGROUND = 0xC8,
	ASSET_TYPE_CRAFTBACKGROUNDLIST = 0xC9,
	ASSET_TYPE_CRAFTMATERIAL = 0xCA,
	ASSET_TYPE_CRAFTMATERIALLIST = 0xCB,
	ASSET_TYPE_CRAFTCATEGORY = 0xCC,
	ASSET_TYPE_CRAFTCATEGORYLIST = 0xCD,
	ASSET_TYPE_CRAFTWEAPONICONTRANSFORM = 0xCE,
	ASSET_TYPE_CRAFTWEAPONICONTRANSFORMLIST = 0xCF,
	ASSET_TYPE_XANIMCURVE = 0xD0,
	ASSET_TYPE_DYNMODEL = 0xD1,
	ASSET_TYPE_VECTORFIELD = 0xD2,
	ASSET_TYPE_WINDDEF = 0xD3,
	ASSET_TYPE_VEHICLEASSEMBLY = 0xD4,
	ASSET_TYPE_MILESTONE = 0xD5,
	ASSET_TYPE_MILESTONETABLE = 0xD6,
	ASSET_TYPE_TRIGGEREFFECTDESC = 0xD7,
	ASSET_TYPE_TRIGGERACTIONS = 0xD8,
	ASSET_TYPE_PLAYERSETTINGS = 0xD9,
	ASSET_TYPE_COMPASSTUNABLES = 0xDA,
	ASSET_TYPE_EXECUTION = 0xDB,
	ASSET_TYPE_SCENARIO = 0xDC,
};

enum scriptInstance_t : __int32
{
	SCRIPTINSTANCE_SERVER = 0x0,
	SCRIPTINSTANCE_CLIENT = 0x1,
	SCRIPTINSTANCE_MAX = 0x2,
};

typedef float vec4_t[4];

struct LuaFile {
	uintptr_t name;
	uintptr_t unk;
	uintptr_t size;
	const char * data;
};


struct UnlockableItemEntry {
	const char* name;
	char __padding[0x170 - 0x8];
};

const char* DB_GetXAssetTypeName(int asset);

struct AssetLink
{
	AssetLink* next;
};

struct ScriptParseTreeOld {
	uintptr_t name;
	uintptr_t __padding0000;
	const char* data;
	int size;
	int __padding0001;
};

struct ScriptParseTreeNew {
	uintptr_t name;
	const char* data;
	int size;
	int __padding0001;
};

union XAsset {
	void* unk;
	ScriptParseTreeOld* scriptParseTreeOld;
	ScriptParseTreeNew* scriptParseTreeNew;
};

struct XAssetPool
{
	XAsset pool;
	unsigned int itemSize;
	int itemCount;
	bool isSingleton;
	int itemAllocCount;
	AssetLink* freeHead;
};

//this struct changes from alpha to original
struct GSC_OBJ
{
	char magic[8];
	int checksum;
	int field_C;
	uintptr_t name;
	char field_18;
	char field_19;
	__int16 exports_count;
	__int16 imports_count;
	char field_1E;
	char field_1F;
	char field_20;
	char field_21;
	char field_22;
	char field_23;
	__int16 include_count;
	char field_26;
	char field_27;
	char field_28;
	char field_29;
	char field_2A;
	char field_2B;
	int cseg_offset;
	char field_30;
	char field_31;
	char field_32;
	char field_33;
	int include_offset;
	int exports_offset;
	int imports_offset;
	char field_40;
	char field_41;
	char field_42;
	char field_43;
	char field_44;
	char field_45;
	char field_46;
	char field_47;
	char field_48;
	char field_49;
	char field_4A;
	char field_4B;
	char field_4C;
	char field_4D;
	char field_4E;
	char field_4F;
	int cseg_size;
	char field_54;
	char field_55;
	char field_56;
	char field_57;
};

/*
struct GSC_OBJ
{
	unsigned __int8 magic[8];
	unsigned __int64 checksum;
	unsigned __int64 file_name;
	int includes_offset;
	__int16 stringtablefixup_count;
	__int16 exports_count;
	int cseg_offset;
	int stringtablefixup_offset;
	__int16 imports_count;
	__int16 fixup_count;
	unsigned __int8 field_2C;
	unsigned __int8 field_2D;
	unsigned __int8 field_2E;
	unsigned __int8 field_2F;
	int exports_offset;
	int imports_offset;
	__int16 globalvar_count;
	unsigned __int8 field_3A;
	unsigned __int8 field_3B;
	int fixup_offset;
	int globalvar_offset;
	unsigned __int8 field_44;
	unsigned __int8 field_45;
	unsigned __int8 field_46;
	unsigned __int8 field_47;
	unsigned __int8 field_48;
	unsigned __int8 field_49;
	unsigned __int8 field_4A;
	unsigned __int8 field_4B;
	int cseg_size;
	__int16 include_count;
	unsigned __int8 field_52;
	unsigned __int8 field_53;
	unsigned __int8 field_54;
	unsigned __int8 field_55;
	unsigned __int8 field_56;
	unsigned __int8 field_57;
};
*/
struct __declspec(align(4)) GSC_STRINGTABLE_ITEM
{
	int string;
	uint8_t num_address;
	uint8_t type;
	int references[255];
};

struct GSC_IMPORT_ITEM
{
	uint32_t name;
	uint32_t name_space;
	uint16_t num_address;
	uint8_t param_count;
	uint8_t flags;
};

struct __declspec(align(4)) GSC_EXPORT_ITEM
{
	uint32_t checksum;
	uint32_t address;
	uint32_t name;
	uint32_t name_space;
	uint32_t callback_event;
	uint8_t param_count;
	uint8_t flags;
};

union DvarLimits
{
	struct
	{
		int stringCount;
		const char** strings;
	} enumeration;

	struct
	{
		int min;
		int max;
	} integer;

	struct
	{
		int64_t min;
		int64_t max;
	} integer64;

	struct
	{
		uint64_t min;
		uint64_t max;
	} unsignedInt64;

	struct
	{
		float min;
		float max;
	} value;

	struct
	{
		float min;
		float max;
	} vector;
};

struct dvar_t;

struct DvarValue
{
	union
	{
		bool enabled;
		int integer;
		uint32_t unsignedInt;
		int64_t integer64;
		uint64_t unsignedInt64;
		float value;
		vec4_t vector;
		const char* string;
		byte color[4];
		const dvar_t* indirect[3];
	} naked;

	uint64_t encrypted;
};

struct DvarData
{
	DvarValue current;
	DvarValue latched;
	DvarValue reset;
};

struct DvarName
{
	__int64 hash;
	__int64 null;
};

struct dvar_t
{
	DvarName name;
	DvarData* value;
	dvarType_t type;
	unsigned int flags;
	DvarLimits domain;
	char padding_unk1[8];
};

struct gentity_t
{

};

typedef void (*xcommand_t)(void);

struct cmd_function_t
{
	cmd_function_t* next;
	__int64 name;
	const char* autoCompleteDir;
	const char* autoCompleteExt;
	xcommand_t function;
};

uintptr_t find_pattern(const char* module_name, const char* pattern);
uintptr_t find_pattern(uintptr_t start, const char* module_name, const char* pattern);

uintptr_t find_pattern(const char* pattern);
uintptr_t find_pattern(uintptr_t start, const char* pattern);

std::pair<void**, void*> patch_import(const std::string& lib, const std::string& func, void* function);

extern uintptr_t cg_entitiesArray;
extern uintptr_t clients;

extern XAssetPool* g_assetPool;

size_t operator"" _b(size_t val);
size_t reverse_b(size_t val);
size_t reverse_b(const void* val);

size_t operator"" _g(size_t val);
size_t reverse_g(size_t val);
size_t reverse_g(const void* val);
