#pragma once

#include <list>
#include <string>
#include <memory>
#include <cstring>

#define TEXTMESSAGE_BUFSIZE 4096
#define TEXTMESSAGE_MAX_PARAMETER_COUNT 1024
namespace jnet {
	class text_message
	{
	public:
		// create new message to send constructor
		text_message(char *data, size_t length);
		~text_message(void);
		int parse(char *data, size_t len);

		char* getProcedureName(void);
		std::string getProcedureNameAsString(void);
		bool isValid();
		unsigned char *getParameter(unsigned int index);
		std::string getParameterAsString(unsigned int index);
		int getParameterAsInt(unsigned int index);
		float getParameterAsFloat(unsigned int index);
		unsigned int getParameterCount(void);
	

		static text_message *createNewMessage(char *procedureName, ... );
		static text_message *formatNewMessage(char *procedureName, char *format, ... );

		unsigned char *getData() { 
			return ((unsigned char *)this->m_DataPtr); 
		}
		int setData(unsigned char *data) {
			this->parse((char *)data, std::strlen((char*)data));
			return 0;
		}

		unsigned int getLength() {
			return this->m_Data->length()+1;
		}

	private:
		std::string *m_Data;
		std::string *m_RpcProcedureName;
		std::string *m_Parameters[TEXTMESSAGE_MAX_PARAMETER_COUNT];
		unsigned int m_ParameterCount;
		bool m_IsValid;
		char *m_DataPtr;
	};
	typedef std::shared_ptr<text_message> text_message_p;
}