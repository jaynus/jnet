#pragma once

#include <string>
#include <memory>
#include <cstdint>

#include "xplatform.hpp"

#include <chrono>

namespace jnet {
	enum e_connection_states {
		NEW = 0,
		DISCOVERY_SENT = 1,
		VALID = 2,
		FAILED = 3,
		SERVER = 4
	};

	struct connection {
		connection() :
			id(""), server(""), port(""),
			packets_sent(0), packets_recv(0), score(0),
			failed(false), has_jnet(false),
			state(e_connection_states::NEW)
		{
		}
		connection(const connection &o) :
			id(o.id), server(o.server), port(o.port),
			packets_sent(o.packets_sent), packets_recv(o.packets_recv), score(o.score),
			failed(o.failed), has_jnet(o.has_jnet),
			state(o.state)
		{
		}
		connection& operator=(const connection & rhs) {
			id = rhs.id; server = rhs.server; port = rhs.port;
			packets_sent = rhs.packets_sent; packets_recv = rhs.packets_recv; score = rhs.score;
			failed = rhs.failed; has_jnet = rhs.has_jnet;
			state = rhs.state;
			return *this;
		}

		std::string		id;
		std::string		name;

		uint32_t			socket;
		struct sockaddr_in	addr;
		int					addr_len;

		float			score;

		std::string		server;
		std::string		port;

		uint64_t		packets_sent;
		uint64_t		packets_recv;
		uint64_t		packets_total() {
			return packets_sent + packets_recv;
		}

		std::chrono::high_resolution_clock::time_point first_packet_time;
		std::chrono::high_resolution_clock::time_point last_packet_time;

		bool			failed;
		bool			has_jnet;
		e_connection_states	state;
	};
	typedef std::shared_ptr<connection> connection_p;
}