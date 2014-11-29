#pragma once

#include "i_engine.hpp"
#include "connection.hpp"
#include "ini_reader.hpp"

namespace jnet {
	class client :
		public i_engine
	{
	public:
			client(connection_p server);
			~client();

			void update();
			void reset();

			std::string rv_command(const std::string &);

			int recv(const connection_p, const message_p);
	protected:
		connection_p	_server;

		std::string		_serverSettings;
		ini_reader		_serverSettingsReader;
	};
};