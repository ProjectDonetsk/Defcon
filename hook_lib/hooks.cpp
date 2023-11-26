#include "hooks.h"

uint32_t vm_notify_tables[55] = { 0x488, 0x550, 0x6D8, 0xE80, 0x18F0, 0x1A48, 0x1A80, 0x20F8, 0x2F18, 0x3718, 0x3A90, 0x3F88, 0x3F90, 0x4068, 0x4CD8, 0x5138, 0x52F8, 0x5D70, 0x5F50, 0x60D8, 0x60E0, 0x6120, 0x62C0, 0x6A08, 0x6AE8, 0x7E68, 0x7F00, 0x108C0, 0x10990, 0x109A0, 0x10CC8, 0x11038, 0x111B8, 0x113F8, 0x11D18, 0x11ED0, 0x120B8, 0x123C8, 0x12438, 0x126F0, 0x13750, 0x13840, 0x13D38, 0x14080, 0x142B0, 0x14A60, 0x14D10, 0x14DC0, 0x160F8, 0x167C0, 0x16C38, 0x16E58, 0x17018, 0x178D8, 0x17D80 };


void VM_OP_Notify_Handler_Hook(int inst, __int64 a2, ScrVmContext_t* vmc, __int64 a4) {

	uintptr_t top = *reinterpret_cast<uintptr_t*>(vmc + 8);

	if (vmc->lastGoodTop[-1].type == 4) {
		uintptr_t notify_hash = vmc->lastGoodTop[-1].u.uintptrValue;
		if (notify_hash != 0x7E1333FF017B0007 && notify_hash != 0x7405A475B731576D && notify_hash != 0x3a79bcab1c1dd008 && notify_hash != 0x4936922A8652B530) {
			//nlog("notify: %p\n", notify_hash);
		}
		if (notify_hash == 0x3562F5AFE8064E85) {
			UnHook();
			vars.bAllPlayersSpawnedOnce = false;
		}

		if (notify_hash == 0x50DE8D6F08284A8B || notify_hash == 0x19FB0A7FE2E8EC41) {
			//spawn event

		}
	}

	if (vmc->lastGoodTop[-1].type == 2) {
		ScrString_t val = vmc->lastGoodTop[-1].u.stringValue;
		//nlog("notify: %s\n", ScrStr_ConvertToString(val));
		if (strcmp(ScrStr_ConvertToString(val), Xors("loadout_given")) == 0) {

			if (!vars.bAllPlayersSpawnedOnce) {
				Hook();
				vars.bAllPlayersSpawnedOnce = true;
			}

			//for (int i = 0; i < 2048; i++) {
			//	uintptr_t ent = GetEntity(i);
			//	if (*reinterpret_cast<uint32_t*>(ent + 0x340) != 0) {
			//		const char* classname = ScrStr_ConvertToString(*reinterpret_cast<ScrString_t*>(ent + 0x340));
			//		if (strcmp(classname, Xors("trigger_hurt")) == 0 ||
			//			strcmp(classname, Xors("trigger_out_of_bounds")) == 0) {
			//			float origin[3] = { 0, 0, -99999999 };
			//			G_SetOrigin(ent, origin);
			//		}
			//	}
			//}
		}

	}



	VM_OP_Notify_Handler(inst, a2, vmc, a4);
}


void RunFunctions() {
	if (vars.bAllPlayersSpawnedOnce) {

	}
}

void Hook() {

}

void UnHook() {

}

