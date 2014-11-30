#include "_CONSTANTS.hpp"

#include "client.hpp"

#include "logging.hpp"
#include "hooker.hpp"

#include "text_message.hpp"

#include <fstream>

namespace jnet {
	client::client(connection_p server) : 
		_server(server),
		_serverSettingsReader("")
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
	void client::update() {
	
		_worker_send_messages();
	}

	std::string client::rv_command(const std::string &message) {
		std::string ret = "";
		text_message msg(const_cast<char *>(message.c_str()), message.size());

		if (msg.getProcedureNameAsString().size() < 1)
			return "";

		if (msg.getProcedureNameAsString() == "getRawServerSettings") {
			ret = _serverSettings;
		} else if (msg.getProcedureNameAsString() == "getCurrentServer") {
			if(_server) {
				ret = _server->id;
			} else {
				ret = std::string("");
			}
		} else if (msg.getProcedureNameAsString() == "getServerConfig") {
			
			if (msg.getParameterCount() < 1)
				return std::string("");

			if (msg.getParameterAsString(0).size() < 1)
				return std::string("");
			
			std::string requestedConfig = msg.getParameterAsString(0);
			
			ret = _serverSettingsReader.Get("", requestedConfig, "");
			
			LOG(DEBUG) << "getServerConfig, returning '" << ret << "'";

			return ret;
		}


		return ret;
	}
	int client::recv(const connection_p conn, const message_p message) {
		text_message msg(const_cast<char *>(message->buffer), message->length);

		if (msg.getProcedureNameAsString().size() < 1)
			return 0;

		if (msg.getProcedureNameAsString() == "server_settings") {
			LOG(INFO) << "Received server settings";
			_serverSettings = msg.getParameterAsString(0);
			//_serverSettingsReader = ini_reader(_serverSettings);
			// We need to write to a temp file, and then pass it to ini_reader
			std::ofstream tempFile(".jnet.lastserver.cfg");
			
			tempFile << _serverSettings;
			tempFile.flush();
			tempFile.close();

			_serverSettingsReader = ini_reader(".jnet.lastserver.cfg");
			if (_serverSettingsReader.ParseError() < 0) {
				LOG(ERROR) << "Error parsing provided server configuration {'" << _server->id << "', '.jnet.lastserver.cfg' }";
			}
		}

		return 0;
	}

	void client::queue_message(const std::string & dst, message_p msg) {
		std::lock_guard<std::mutex> lock(_outboundMessageQueueMutex);

		_outboundMessageQueue.push_back(std::make_pair(dst, msg));
	}
	void client::_worker_send_messages() {
		if (!_server || _outboundMessageQueue.size() < 1)
			return;

		{
			std::lock_guard<std::mutex> lock_queue(_outboundMessageQueueMutex);

			for (auto msg_pair : _outboundMessageQueue) {
				if (msg_pair.first == "") {
					LOG(DEBUG) << "Sending message to server";
					sendto(_server->socket,
						msg_pair.second->buffer, msg_pair.second->length, 0,
						reinterpret_cast<struct sockaddr *>(&_server->addr), _server->addr_len);
				}
				else {
					LOG(DEBUG) << "Sending targeted message to [" << msg_pair.first << "]";
					sendto(_server->socket,
						msg_pair.second->buffer, msg_pair.second->length, 0,
						reinterpret_cast<struct sockaddr *>(&msg_pair.second->addr), msg_pair.second->addr_len);
				}
			}

			_outboundMessageQueue.clear();
		}
	}

};