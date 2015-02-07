#include "_CONSTANTS.hpp"

#include "john.hpp"
#include "logging.hpp"
#include "connection.hpp"

#include "xplatform.hpp"
#include <string>
#include <vector>

#include "server.hpp"
#include "client.hpp"
#include "hooker.hpp"

#include "text_message.hpp"

namespace jnet {
	const char * header = "JNET";
	const char * msg_hello = "JNET HELLO";
	const char * msg_welcome = "JNET WELCOME";
	
	std::string g_ProfileName;

	john * g_Engine;

	void john::run() {
		while (!_stopping) {

			//LOG(DEBUG) << "tick {" << state().status << "-" << _lastState.status << "}";
			if (state().status != _lastState.status) {
				LOG(DEBUG) << "State change detected: [" << state().status << "]";
			}

			if (state().status == e_status_t::DISABLED) {
				sleep(1000);
				continue;
			}

			if (_currentEngine)
				_currentEngine->update();

			monitor_connections();
		
			_lastState = state();

			sleep(100);
		}
	}

	void john::monitor_connections() {
		if (_connList.size() > 0) {
			std::lock_guard<std::mutex> lock(_connListMutex);

			if (_state.status == e_status_t::DISCOVERY) {
				for (auto conn_ref : _connList) {
					if (conn_ref.second->packets_recv > INITIAL_RECV_THRESHOLD) {
						LOG(DEBUG) << "[" << conn_ref.first << "]: s=" << conn_ref.second->packets_sent << ", r=" << conn_ref.second->packets_recv;
						if (conn_ref.second->packets_sent > INITIAL_SEND_THRESHOLD && conn_ref.second->packets_recv > INITIAL_RECV_THRESHOLD) {
							if (conn_ref.second->has_jnet) {
								
								LOG(INFO) << "Primary Server determined [" << conn_ref.first << "]";
								LOG(INFO) << "Initializing";

								_state.status = e_status_t::CONNECTED;
								if (!_currentEngine) {
									LOG(DEBUG) << "Client created";
									_currentServerConnection = _connList[conn_ref.first];
									_currentEngine = std::unique_ptr<i_engine>(reinterpret_cast<i_engine *>(new client(_connList[conn_ref.first])));
								}
							}
						}
					}
				}
			}
			else {
				// If we receive nothing from the server for 10 seconds, we flag as disconnected.
				if (_state.status == e_status_t::CONNECTED) {
					if (_currentServerConnection) {
						std::chrono::milliseconds total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - _currentServerConnection->last_packet_time);
						float seconds = total_ms.count() / 1000.0f;
#ifdef DO_TRACE_CONNECTIONS
						LOG(DEBUG) << "Timeout check: ms=" << total_ms.count() << ", seconds=" << seconds;
#endif
						if (seconds > DISCONNECT_TIMEOUT) {
							LOG(WARNING) << "Server connection lost. Initiating discovery...";
							_state.status = e_status_t::DISCOVERY;
							_currentServerConnection = nullptr;
							_currentEngine = nullptr;

							for (auto conn : _connList) {
								_connList[conn.first] = nullptr;
							}
							_connList.clear();
						}
					}
				}

				// Clean up any connections tracked over 30 seconds old
				std::vector<std::string> to_delete;
				for (auto conn_ref : _connList) {
					std::chrono::milliseconds total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - conn_ref.second->last_packet_time);
					float seconds = total_ms.count() / 1000.0f;
#ifdef DO_TRACE_CONNECTIONS
					LOG(DEBUG) << "[" << conn_ref.first << "] ms=" << total_ms.count() << ", seconds=" << seconds;
#endif
					if (seconds > CONNECTION_TIMEOUT) {
						to_delete.push_back(conn_ref.first);
					}
				}
				if (to_delete.size() > 0) {
					for (auto del : to_delete) {
#ifdef DO_TRACE_CONNECTIONS
						LOG(DEBUG) << "GC: de-tracking [" << del << "]";
#endif
						_connList.erase(del);
					}
					if(_state.status == e_status_t::CONNECTED) {
						if (_connList.size() < 1) {
							LOG(WARNING) << "All connections lost, Initiating discovery...";
							_state.status = e_status_t::DISCOVERY;
							_currentServerConnection = nullptr;
							_currentEngine = nullptr;
						}
					}
				}
			}
		}
	}

	void john::reset() {
		if (_state.status != e_status_t::SERVER_ENABLED) {
			{
				std::lock_guard<std::mutex> lock(_connListMutex);
				LOG(WARNING) << "Warning, reset performed, Initiating discovery...";
				_connList.clear();
				_currentServerConnection = nullptr;
				_currentEngine = nullptr;
			}
			_state.status = e_status_t::DISCOVERY;
		}
	}

	void john::send_discovery(connection_p server) {
		int res = sys_sendto(server->socket, msg_hello, 13, 0, (sockaddr *)&server->addr, server->addr_len);
		server->state = e_connection_states::DISCOVERY_SENT;
	}
	void john::recv_discovery(uint32_t s, char *data, int length, struct sockaddr *from, int fromlen) {
		std::string cur_host;
		
		if (length > 10) {
			// Compare bytes
			if (memcmp(msg_welcome, data, 13) != 0) {
				return;
			}
		
			cur_host = host_string(from);

			// We only get here if the welcome header matched
			// Find the connection in our tracking, if it exists, then flag it as our active server.
			{
				std::lock_guard<std::mutex> lock(_connListMutex);

				if (_connList.find(cur_host) == _connList.end())
					return;		// We bail on the case of receiving a welcome from someone not in our conntrack list. This will prevent spoofing (mostly?)

				// Track the connection as having an active JNET instance. This will narrow down our filtering and searching for the current server.
				_connList[cur_host]->has_jnet = true;
				LOG(DEBUG) << "New JNET Peer: [" << cur_host << "]";
			}
		}

	}

	bool john::handle_jnet_packet(uint32_t socket, char *buffer, int length, struct sockaddr *from, int fromlen) {
		connection_p conn;
		LOG(DEBUG) << "enter";
		if (state().status == e_status_t::SERVER_ENABLED || state().status == e_status_t::CONNECTED) {
			{
				std::lock_guard<std::mutex> lock(_connListMutex);
				std::string cur_host = host_string(from);
				if (_connList.find(cur_host) != _connList.end()) {
					conn = _connList.find(cur_host)->second;
				} else {
					LOG(ERROR) << "!!!!!! received JNET packet without connection information.";
				}
			}
			if (conn) {
				return _currentEngine->recv(conn, message_p(new message_t(conn, socket, buffer+5, length-5, from, fromlen)));
				return true;
			}
		}

		if (state().status == e_status_t::DISCOVERY) {
			// Handle negotiation packets
			recv_discovery(socket, buffer, length, from, fromlen);
		}

		return false;
	}

	void john::rv_command(char *output, int outputSize, const char *function) {
		std::string cmd = function;

		std::string default_response = "";
		memcpy(output, default_response.c_str(), default_response.size() + 1);
		output[default_response.size() + 1] = 0x00;

		std::string initialized_server = "server_init";
		std::string initialized_client = "client_init";
		std::string initialized_name = "profileName";

		LOG(DEBUG) << "CMD:" << cmd;

		if (cmd.size() < 1)
			return;
		
		LOG(DEBUG) << "Got a command [" << cmd << "]";

		if (cmd == "disable") {
			_state.status = e_status_t::DISABLED;

			_currentEngine = nullptr;
			_currentServerConnection = nullptr;
		} else if (cmd == "reset") {
			reset();
		} else if (cmd == "init:base") {
			reset();
			
			// Determine starting conditions to tell if server or client
			std::string process_name = get_path();
			std::string cmd_line = get_cmdline();
			LOG(DEBUG) << "Current process: " << process_name;			

			if (process_name.find("server") != std::string::npos || cmd_line.find("-config") != std::string::npos) {
				LOG(INFO) << "Detected ArmA3 Server. Running in server mode...";

				_state.is_server = true;
				_state.status = e_status_t::SERVER_ENABLED;
				if (!_currentEngine) {
					_currentEngine = std::unique_ptr<i_engine>(
						reinterpret_cast<i_engine *>(new server())
						);
				}
			
				memcpy(output, initialized_server.c_str(), initialized_server.size() + 1);
				output[initialized_server.size() + 1] = 0x00;
			} else {
				LOG(INFO) << "Detected ArmA3 Client. Running in client mode...";
				_state.status = e_status_t::DISCOVERY;
				LOG(INFO) << "Automatic server discovery initiated";
				
				memcpy(output, initialized_client.c_str(), initialized_client.size()+1);
				output[initialized_client.size() + 1] = 0x00;
			}
		} else if (cmd.find("profileName:") != std::string::npos) {
			text_message msg(const_cast<char *>(cmd.c_str()), cmd.size() + 1);
			if (msg.getProcedureNameAsString().size() > 1) {
				if (msg.getParameterCount() > 0) {
					if (g_ProfileName != msg.getParameterAsString(0)) {
						LOG(INFO) << "Setting local name: " << msg.getParameterAsString(0);

						g_ProfileName = msg.getParameterAsString(0);

						memcpy(output, initialized_name.c_str(), initialized_name.size() + 1);
						output[initialized_name.size() + 1] = 0x00;
					}
				}
			}

		} else {
			if (_currentEngine) {
				std::string ret = _currentEngine->rv_command(cmd);
				if (ret.size() > outputSize)
					ret.resize(outputSize - 1);
				
				if (ret.size() > 0) {
					memcpy(output, ret.c_str(), ret.size());
					output[ret.size()] = 0x00;
				}

			} else {
				std::string ret = "NOCONN";
				memcpy(output, ret.c_str(), ret.size());
				output[ret.size()] = 0x00;
			}
		}
	}

	int john::send(uint32_t s, char *data, int length, struct sockaddr *to, int tolen) {

		track(s, data, length, to, tolen, 1);

		return -1;
	}
	int john::recv(uint32_t s, char *data, int length, struct sockaddr *from, int fromlen) {
		int retVal;
		std::string cur_host;

		if (state().status == e_status_t::DISABLED)
			return -1;

		track(s, data, length, from, fromlen, 0);

		// Check the packet for JNET command header
		if (length > 4) {
			if (memcmp(data, header, 4) == 0) {
				if (!handle_jnet_packet(s, data, length, from, fromlen)) {
					return 0;
				}
			}
		}

		return -1;
	}
	void john::track(uint32_t s, char *data, int length, struct sockaddr *from, int fromlen, uint32_t direction) {
		std::string cur_host;

		cur_host = host_string(from);
		if (cur_host.size() > 1) {
			_totalPackets++;

			{
				std::lock_guard<std::mutex> lock(_connListMutex);

				if (_connList.find(cur_host) == _connList.end()) {
#ifdef DO_TRACE_CONNECTIONS
					LOG(DEBUG) << "Tracking new connection [" << cur_host << "]";
#endif
					_connList[cur_host] = std::shared_ptr<connection>(new connection());
					_connList[cur_host]->id = cur_host;
					_connList[cur_host]->socket = s;
					_connList[cur_host]->state = e_connection_states::NEW;

					_connList[cur_host]->first_packet_time = std::chrono::high_resolution_clock::now();

					memset(&_connList[cur_host]->addr, 0x00, sizeof(_connList[cur_host]->addr));
					memcpy(&_connList[cur_host]->addr, from, fromlen);
					_connList[cur_host]->addr_len = fromlen;
				}

				_connList[cur_host]->last_packet_time = std::chrono::high_resolution_clock::now();

				if (direction == 0)
					_connList[cur_host]->packets_recv++;
				else
					_connList[cur_host]->packets_sent++;
				
				_connList[cur_host]->socket = s;
				memset(&_connList[cur_host]->addr, 0x00, sizeof(_connList[cur_host]->addr));
				memcpy(&_connList[cur_host]->addr, from, fromlen);
				_connList[cur_host]->addr_len = fromlen;

				// If its a new connection and we just tracked it, lets send it a discovery request.
				if (_state.status == e_status_t::DISCOVERY && _connList[cur_host]->state == e_connection_states::NEW) {
					send_discovery(_connList[cur_host]);
				}
			}
		}
	}

	void john::initialize() {
		LOG(DEBUG) << "enter";

		_lastState = { 0 };
		_state = { 0 };
#ifdef _DEBUG
		el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "[%datetime] - %level - {%loc}t:%thread- %msg");
#else
		el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%datetime-{%level}- %msg");
#endif
		el::Loggers::reconfigureAllLoggers(el::ConfigurationType::MaxLogFileSize, "100000");

		_hooker.initialize();

		_worker = std::thread(&john::run, this);
	}
	void john::release() {
		LOG(DEBUG) << "enter";
		_stopping = true;
		sleep(2000);
		_hooker.release();

		reset();

		//if (_worker.joinable())
		//	_worker.join();
	}

	john::john() :
		_stopping(false),
		_totalPackets(0)
	{
		_lastState = { 0 };
		_state = { 0 };
	}
	john::~john()
	{}

	std::string john::host_string(const struct sockaddr *from) {
		struct sockaddr_in *fromAddr;
		fromAddr = (struct sockaddr_in *)from;
		
		if (!from)
			return std::string("");

		char *hostname = inet_ntoa(fromAddr->sin_addr);
		
		//if (!ntohs(fromAddr->sin_port))
		//	return std::string("");

		std::stringstream host;
		host << hostname << ":" << (int)ntohs(fromAddr->sin_port);

		return host.str();
	}
	std::string john::host_string(const struct sockaddr * from, connection & track) {
		return std::string("");
	}
};
