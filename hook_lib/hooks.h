#pragma once
#include "Main.hpp"

extern uint32_t vm_notify_tables[55];

enum ScrString_t : __int32 { };
typedef uint32_t ScrVarIndex_t;
union ScrVarValueUnion_t
{
	int64_t intValue;
	uintptr_t uintptrValue;
	float floatValue;
	ScrString_t stringValue;
	const float* vectorValue;
	byte* codePosValue;
	ScrVarIndex_t pointerValue;
	void* stackValue;
};


struct ScrVarValue_t
{
	ScrVarValueUnion_t u;
	int type;
};

struct ScrVmContext_t
{
	char __padding0000[0x0008];
	ScrVarValue_t* lastGoodTop;
};



void VM_OP_Notify_Handler_Hook(int inst, __int64 a2, ScrVmContext_t* vmc, __int64 a4);

void RunFunctions();

void Hook();
void UnHook();
