#pragma once

#if	defined(_MSC_VER)

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
#include <io.h>

inline std::string get_path() {
	char moduleName[MAX_PATH];
	GetModuleFileName(NULL, moduleName, MAX_PATH);
	return std::string(moduleName);
}

inline std::string get_cmdline() {
	return std::string(GetCommandLineA());
}

#define socklen_t int
#define XPLATFORM_STRNCAT(a,b,c) strcat_s(a,b,c)

#define sleep Sleep
#define access _access

#else

#include <stdio.h>
#include <cstdio>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>

#define vsprintf_s vsnprintf
#define _snprintf_s(a,b,c,...) snprintf(a,b,__VA_ARGS__)

#define XPLATFORM_STRNCAT(a,b,c) strncat(a,c,b)

typedef int SOCKET;
typedef unsigned long DWORD;

#define FAR
#define WSAAPI
#define PATH_MAX 1024

inline std::string get_path() {
	char arg1[PATH_MAX];
	char exepath[PATH_MAX + 1] = { 0 };

	snprintf(arg1, PATH_MAX, "/proc/%d/exe", getpid());
	size_t len = readlink(arg1, exepath, PATH_MAX);
	
	if(len < 1) {
		return std::string("");
	} else {
		return std::string(exepath);
	}
}

inline std::string get_cmdline() {
	std::stringstream ret_cmd_line;
	std::ifstream cmdline("/proc/self/cmdline", std::ifstream::binary);

	if (cmdline.is_open()) {
		for (std::string arg; std::getline(cmdline, arg); ) {
			ret_cmd_line << arg << " " << std::endl;
		}
	} else {
		return std::string("");	
	}

	return ret_cmd_line.str();
}

#endif
