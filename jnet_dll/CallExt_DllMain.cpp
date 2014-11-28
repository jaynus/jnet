#include "xplatform.hpp"

#if	defined(_MSC_VER)

	#include <Windows.h>
	#include <detours.h>
	#pragma comment(lib, "Shlwapi.lib")
	#pragma comment(lib, "ws2_32.lib")
	#pragma comment(lib, "detours.lib" )

#endif

#include <string>

#include "john.hpp"
#include "logging.hpp"
_INITIALIZE_EASYLOGGINGPP

void initialize() {
		printf("------------enter--------\n");

		LOG(DEBUG) << "initialize()";
		jnet::g_Engine->initialize();
	}

void release() {
	jnet::g_Engine->release();
}


#if	defined(_MSC_VER)
	static void load(void) {
		LOG(INFO) << "JNET DLL Loaded";
		jnet::g_Engine = nullptr;
	}

	static void destroy(void) {
		LOG(DEBUG) << "JNET DLL destroyed";
	}
	BOOL APIENTRY DllMain( HMODULE hModule,
						   DWORD  ul_reason_for_call,
						   LPVOID lpReserved
						 )
	{
		switch (ul_reason_for_call) {
			case DLL_PROCESS_ATTACH:;
				load();
				break;
			case DLL_THREAD_ATTACH:
			case DLL_THREAD_DETACH:
			case DLL_PROCESS_DETACH:
				destroy();
				break;
			default:
				break;
		}
		
		return TRUE;
	}
#else
	static void __attribute__((constructor)) load(void) {
//		LOG(DEBUG) << "JNET DLL Loaded";
		printf("jnet.so Loaded\n");
		jnet::g_Engine = nullptr;
	}

	static void __attribute__((destructor)) destroy(void) {
//		LOG(DEBUG) << "JNET DLL destroyed";
		printf("jnet.so Destroyed\n");
	}
#endif

#if	defined(_MSC_VER)
	extern "C" {
		__declspec(dllexport) void __stdcall RVExtension(char *output, int outputSize, const char *function);
	};

	void __stdcall RVExtension(char *output, int outputSize, const char *function) {
		if (!jnet::g_Engine) {
			jnet::g_Engine = new jnet::john();
			initialize();
		}
		jnet::g_Engine->rv_command(output, outputSize, function);
	}
#else						// Linux
	extern "C" {
		void RVExtension(char *output, int outputSize, const char *function);
	};


	void RVExtension(char *output, int outputSize, const char *function) {
		if (!jnet::g_Engine) {
			jnet::g_Engine = new jnet::john();
			initialize();
		}
		jnet::g_Engine->rv_command(output, outputSize, function);
	}
#endif
