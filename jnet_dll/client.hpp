#pragma once

#include "i_engine.hpp"
#include "base_engine.hpp"

#include "connection.hpp"
#include "ini_reader.hpp"

#include <mutex>
#include <vector>

namespace jnet {
	class client :
		public i_engine,
		public base_engine
	{
	public:
			client(connection_p server);
			~client();

			void update();
			void reset();

			std::string rv_command(const std::string &);
			int recv(const connection_p, const message_p);

			void queue_message(const std::string &, message_p);
			void _worker_send_messages();
	protected:
		std::mutex													_outboundMessageQueueMutex;
		std::vector<std::pair<std::string, message_p>>				_outboundMessageQueue;

		connection_p	_server;

		std::string		_serverSettings;
		ini_reader		_serverSettingsReader;
	};
};