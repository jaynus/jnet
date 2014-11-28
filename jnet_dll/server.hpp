#pragma once

#include "i_engine.hpp"
#include "connection.hpp"

#include <map>

#include "ini_reader.hpp"

namespace jnet {
	class server :
		public i_engine 
	{
	public:
		server();
		~server();

		void update();
		
		void timeout_clients();

		void reset();

		std::string rv_command(std::string &);

		void load_configuration();
		void handle_new_client(connection_p);

		std::string compile_settings_message();

		int recv(connection_p, message_p);

		ini_reader & config() { return *_config;  }

	protected:
		std::map<std::string, connection_p> _clients;
		ini_reader * _config;
	};
};