#pragma once

#include <cstdint>
#include <string>

#define INITIAL_RECV_THRESHOLD		50
#define INITIAL_SEND_THRESHOLD		10

#define DISCONNECT_TIMEOUT			3.0f
#define CONNECTION_TIMEOUT			10.0f


namespace jnet {
	extern const char * header;
	extern const char * msg_hello;
	extern const char * msg_welcome;

	extern std::string g_ProfileName;
}