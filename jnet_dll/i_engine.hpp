#pragma once

#include <cstdint>
#include <memory>

#include "xplatform.hpp"

#include "connection.hpp"

namespace jnet {

	struct message_t {
		message_t() :
			socket(0), buffer(0), length(0), addr(0), addr_len(0)
		{
		
		}
		message_t(connection_p src_, int socket_, char *buffer_, int length_, struct sockaddr * from_, int fromlen_) :
			src(src_), socket(socket_), buffer(buffer_), length(length_), addr(from_), addr_len(fromlen_)
		{}

		connection_p		src;
		
		int					socket;

		char				*buffer;
		int					length;

		struct sockaddr	  * addr;
		int					addr_len;
	};
	typedef std::shared_ptr<message_t> message_p;

	class i_engine {
	public:
		virtual ~i_engine() {}

		virtual void update() = 0;
		virtual void reset() = 0;

		virtual std::string rv_command(const std::string &) = 0;
		virtual int recv(const connection_p, const message_p) = 0;

		virtual void queue_message(const std::string &, message_p) = 0;

	};
}