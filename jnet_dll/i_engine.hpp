#pragma once

#include <cstdint>
#include <memory>

#include "xplatform.hpp"

#include "connection.hpp"

namespace jnet {

	struct message_t {
		message_t() :
			socket(0), buffer(0), length(0), from(0), fromlen(0)
		{
		
		}
		message_t(connection_p src_, int socket_, char *buffer_, int length_, struct sockaddr * from_, int fromlen_) :
			src(src_), socket(socket_), buffer(buffer_), length(length_), from(from_), fromlen(fromlen_)
		{}

		connection_p		src;
		
		int					socket;

		char				*buffer;
		int					length;

		struct sockaddr	  * from;
		int					fromlen;
	};
	typedef std::shared_ptr<message_t> message_p;

	class i_engine {
	public:
		virtual ~i_engine() {}

		virtual void update() = 0;
		virtual void reset() = 0;

		virtual std::string rv_command(std::string &) = 0;

		virtual int recv(connection_p, message_p) = 0;

	};
}