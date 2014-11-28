#pragma once

#include <cstdint>
#include <string>
#include <thread>
#include <mutex>
#include <memory>
#include <map>

#include "connection.hpp"
#include "hooker.hpp"

#include "i_engine.hpp"

namespace jnet {
	enum e_status_t {
		NOT_CONNECTED			= 0,
		CONNECTED				= 1,
		DISCOVERY				= 2,
		SYNCING					= 3,
		SERVER_ENABLED			= 4,
		DISABLED				= 0xFFFFFFFF
	};
	struct state_t {
		bool			is_server;
		e_status_t		status;
	};

	class john {
	public:
		john();
		~john();

		void reset();
		void initialize();
		void release();

		void run();
		void monitor_connections();

		void send_discovery(connection_p server);
		void recv_discovery(uint32_t, char *, int, struct sockaddr *, int);

		bool handle_jnet_packet(uint32_t, char *, int, struct sockaddr *, int);

		void rv_command(char *output, int outputSize, const char *function);

		void track(uint32_t, char *, int, struct sockaddr *, int, uint32_t);
		int send(uint32_t, char *, int, struct sockaddr *, int);
		int recv(uint32_t, char *, int, struct sockaddr *, int);

		static std::string host_string(const struct sockaddr *);
		static std::string host_string(const struct sockaddr *, connection &);

		const state_t & state(void) { return _state;  }

	protected:
		std::unique_ptr<i_engine>								_currentEngine;
		connection_p											_currentServerConnection;

		volatile bool											_stopping;
		std::thread												_worker;

		state_t													_state;
		state_t													_lastState;

		jnet::hooker											_hooker;

		volatile uint64_t										_totalPackets;
		std::mutex												_connListMutex;
		
		std::map<std::string, connection_p>						_connList;
	};

	extern jnet::john * g_Engine;
};