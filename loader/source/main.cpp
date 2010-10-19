#include <psl1ght/lv2.h>
#include <kammy.h>
#include <common/kammy_lv2.h>

#include "ethdebug.bin.h"

#include <stdio.h>

int len(const char* str) {
	int l;
	for (l = 0; str[l]; l++)
		;
	return l - 1;
}

#define printf(fmt, ...) Lv2Syscall(403, 1, (u64)fmt, len(fmt), 0)
#define Debug_Print(...) printf(__VA_ARGS__)

static u64 LoadModule(const u8* bin, const char* name, u64 param1 = 0, u64 param2 = 0, u64 param3 = 0, u64 param4 = 0, u64 param5 = 0, u64 param6 = 0)
{
	u64 ret = KAMMY_ERROR;
	Debug_Print("\tLoading %s into lv2... ", name);
	const Lv2Module* module = Kammy_Load(bin);
	if (!module->Verify()) {
		Debug_Print("Failed! %s is corrupt.\n", name);
		return ret;
	}
	if (!module->Execute(&ret, param1, param2, param3, param4, param5, param6))
		Debug_Print("Failed! Kammy was unable to load %s.\n", name);
	else
		Debug_Print("Done! Result returned from %s in lv2: 0x%08x%08x\n", name, (u32)(ret >> 32), (u32)ret);
	return ret;
}

static bool Init()
{
	if (Kammy_IsInitialised())
		Debug_Print("\tAlready loaded!\n");
	else {
		Debug_Print("\tInitializing... ");
		if (Kammy_Initialise())
			Debug_Print("Done! Kammy injected into lv2 successfully.\n");
		else {
			Debug_Print("Failed! Syscalls could not be injected. Does this payload have peek/poke?\n");
			return false;
		}
	}

	return true;
}

extern "C" int main(int argc, char** argv)
{
	Debug_Print("Kammy v" LIBKAMMY_VERSION "\n");
	
	if (!Init()) {
		Debug_Print("Failed!\n");
		return 0;
	}
	
	LoadModule(ethdebug_bin, "ethdebug.bin");
	printf("This should be a TTY_WRITE test.\n");

	Debug_Print("Aaand we're done for tonight folks.\n");

	return 0;
}
