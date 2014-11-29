#include "_CONSTANTS.hpp"

#include "server.hpp"

#include "hooker.hpp"
#include "logging.hpp"
#include "john.hpp"
#include "text_message.hpp"
#include "ini_reader.hpp"

#include <sstream>

namespace jnet {
	server::server() : _config(nullptr)
	{
		load_configuration();
	}
	server::~server() {}

	void server::update() {
		timeout_clients();
	}

	void server::timeout_clients() {
		std::vector<std::string> to_delete;
		for (auto conn_ref : _clients) {
			std::chrono::milliseconds total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - conn_ref.second->last_packet_time);
			float seconds = total_ms.count() / 1000.0f;
			LOG(DEBUG) << "[" << conn_ref.first << "] ms=" << total_ms.count() << ", seconds=" << seconds;

			if (seconds > DISCONNECT_TIMEOUT) {
				to_delete.push_back(conn_ref.first);
			}
		}
		for (auto del : to_delete) {
			LOG(WARNING) << "Lost connection from client [" << del << "]";
			_clients.erase(del);
		}
	}

	void server::reset() {
		_clients.clear();
	}
	int server::recv(connection_p conn, message_p message) {
		//Check for headers, otherwise its a binary packet
		if (message->length > 4) {
			if (memcmp(msg_hello+5, message->buffer, 5) != 0) {

				text_message msg(const_cast<char *>(message->buffer), message->length);

				if (msg.getProcedureNameAsString().size() < 1)
					return 0;

				LOG(DEBUG) << "SRV NETWORK [" << msg.getData() << "]";

				if (msg.getProcedureNameAsString() == "client_ready") {
					std::string settings = compile_settings_message();
					
					if (msg.getParameterCount() > 0)
						conn->name = msg.getParameterAsString(0);

					LOG(INFO) << "Synchronizing with [" << conn->name << " @ " << conn->id << "]";

					text_message_p msg_config(text_message::formatNewMessage(
						"JNET server_settings:%1", 
						const_cast<char *>(settings.c_str())
					));

					sys_sendto(conn->socket,
						reinterpret_cast<char *>(msg_config->getData()), msg_config->getLength(),
						0, reinterpret_cast<struct sockaddr *>(&conn->addr), conn->addr_len);
				}

				return 0;
			} else {			// GENERIC HELLO packets get handled seperate.
				h_sendto(message->socket, msg_welcome, 13, 0, message->from, message->fromlen);

				if (_clients.find(conn->id) == _clients.end()) {
					LOG(DEBUG) << "New JNET Client Peer - {" << message->src->id << "}";
					_clients[conn->id] = conn;
					handle_new_client(conn);
				}
			}
		}

		return -1;
	}

	std::string server::rv_command(std::string &cmd) {
		std::string ret = "";
		
		LOG(DEBUG) << "SRV ["<< cmd << "]";
		text_message msg(const_cast<char *>(cmd.c_str()), cmd.size());

		if (msg.getProcedureNameAsString() == "getServerConfig") {
			std::string requestedConfig = msg.getParameterAsString(0);

			if(requestedConfig.find_first_of("jvon_") == 0 || requestedConfig.find_first_of("jnet_") == 0) {
				ret = config().Get("", requestedConfig, "");
			} else {
				ret = "You naughty little fucker";
			}
			LOG(DEBUG) << "return" << ret;
			return ret;
		}
		return ret;
	}

	void server::handle_new_client(connection_p conn) {
		// Hardcode handling and sending JVON data here
	}

	std::string server::compile_settings_message() {
		std::stringstream str;

		LOG(INFO) << "Configuration File Loaded";
		
		str << "[jvon_settings]";
		str << "jvon_launcher = " << config().GetInteger("", "jvon_launcher", 0) << ";\n";
		str << "jvon_host = " << config().Get("", "jvon_host", "") << ";\n";
		str << "jvon_host_port = " << config().GetInteger("", "jvon_host_port", 0) << ";\n";
		str << "jvon_server_config = " << config().Get("", "jvon_server_config", "@JVON\\jvon_server.json") << ";\n";
		str << "jvon_password = " << config().Get("", "jvon_password", "") << ";\n";
		str << "jvon_required = " << config().GetInteger("", "jvon_required", 0) << ";\n";
		str << "jvon_required_timeout = " << config().GetInteger("", "jvon_required_timeout", 0) << ";\n";
		str << "jvon_extended_config = " << config().Get("", "jvon_extended_config", "") << ";\n";

		return str.str();
	}

	void server::load_configuration() {
		// Since we are a server process, we need to determine the config file location. Either it will be command-line provided, or the default arma3 path.
		// Extract the config name
		std::string cfg_name = "";

		std::string cmd_line = get_cmdline();
		LOG(DEBUG) << "Server Parameters: " << cmd_line;

		if (cmd_line.find("-config") == std::string::npos) {
			LOG(DEBUG) << "* No server configuration detected; falling back on defaults";

		} else {
			if (cmd_line.find_first_of("-config") != std::string::npos) {
				cfg_name = cmd_line.substr(cmd_line.find("-config") + 8, cmd_line.size() - (cmd_line.find("-config") + 8));
				cfg_name.resize(cfg_name.find_first_of(" "));

			} else if (cmd_line.find_first_of("-cfg") != std::string::npos) {
				cfg_name = cmd_line.substr(cmd_line.find("-cfg") + 8, cmd_line.size() - (cmd_line.find("-cfg") + 8));
				cfg_name.resize(cfg_name.find_first_of(" "));
			} else {
				cfg_name = "";
			}
			LOG(DEBUG) << "Determined config name: '" << cfg_name << "'";

			std::ifstream file(cfg_name);
			if (!file) {
				LOG(DEBUG) << "Couldn't open server configuration file.";
			} else {
				// Close our test
				file.close();
				LOG(INFO) << "Loading server configuration {" << cfg_name << "}";
			}
		}
		_config = new ini_reader(cfg_name);

		if (config().ParseError() < 0) {
			LOG(ERROR) << "Can't load server configuration '" << cfg_name << "'";
			return;
		}
#ifdef _DEAD	// This was testing with our JVON configurations. We should implement fetching these from command line.
		LOG(INFO) << "Configuration File Loaded";
		LOG(INFO) << "\t jvon_launcher = " << config().GetInteger("", "jvon_launcher", 0);
		LOG(INFO) << "\t jvon_host = " << config().Get("", "jvon_host", "");
		LOG(INFO) << "\t jvon_host_port = " << config().GetInteger("", "jvon_host_port", 0);
		LOG(INFO) << "\t jvon_server_config = " << config().Get("", "jvon_server_config", "@JVON\jvon_server.json");
		LOG(INFO) << "\t jvon_password = " << config().Get("", "jvon_password", "");
		LOG(INFO) << "\t jvon_required = " << config().GetInteger("", "jvon_required", 0);
		LOG(INFO) << "\t jvon_required_timeout = " << config().GetInteger("", "jvon_required_timeout", 0);
		LOG(INFO) << "\t jvon_extended_config = " << config().Get("", "jvon_extended_config", "");
#endif
	}
};
