#pragma once

#include "i_engine.hpp"
#include "connection.hpp"

namespace jnet {
	class client :
		public i_engine
	{
	public:
			client(connection_p server);
			~client();

			void update();
			void reset();

			std::string rv_command(std::string &);

			int recv(connection_p, message_p);
	protected:
		connection_p	_server;
		std::string		_serverSettings;
	};
};