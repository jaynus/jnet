#include "_CONSTANTS.hpp"

#include "client.hpp"

#include "logging.hpp"
#include "hooker.hpp"

#include "text_message.hpp"

namespace jnet {
	client::client(connection_p server) : 
		_server(server)
	{
		// On construction, send an initial client ready message to the server
		text_message_p msg_ready(text_message::formatNewMessage("JNET client_ready", const_cast<char *>(g_ProfileName.c_str())));

		sys_sendto(_server->socket,
			reinterpret_cast<char *>(msg_ready->getData()), msg_ready->getLength(),
			0, reinterpret_cast<struct sockaddr *>(&_server->addr), _server->addr_len);
		LOG(DEBUG) << "Sent client ready to [" << _server->id << "]";
	}

	client::~client() {}

	void client::reset() {}
	void client::update() {}

	std::string client::rv_command(std::string &message) {
		std::string ret = "";
		text_message msg(const_cast<char *>(message.c_str()), message.size());

		if (msg.getProcedureNameAsString().size() < 1)
			return 0;

		if (msg.getProcedureNameAsString() == "getServerSettings") {
			ret = _serverSettings;
		}

		return ret;
	}
	int client::recv(connection_p conn, message_p message) {
		text_message msg(const_cast<char *>(message->buffer), message->length);

		if (msg.getProcedureNameAsString().size() < 1)
			return 0;

		if (msg.getProcedureNameAsString() == "server_settings") {
			LOG(INFO) << "Received server settings";
			_serverSettings = msg.getParameterAsString(0);
		}

		return 0;
	}
};