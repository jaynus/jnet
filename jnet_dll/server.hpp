#pragma once

#include "i_engine.hpp"
#include "connection.hpp"

#include <map>
#include <mutex>
#include <vector>

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

		std::string rv_command(const std::string &);
		int recv(connection_p, message_p);

		void queue_message(const std::string &, message_p);
		void _worker_send_messages();

		void load_configuration();
		void handle_new_client(connection_p);

		ini_reader & config() { return *_config;  }
		std::string compile_settings_message();
	protected:
		std::mutex													_outboundMessageQueueMutex;
		std::vector<std::pair<std::string, message_p>>				_outboundMessageQueue;

		std::mutex													_clientsMutex;
		std::map<std::string, connection_p>							_clients;

		ini_reader *												_config;
	};
};