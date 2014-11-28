// Read an INI file into easy-to-access name/value pairs.

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include "ini.h"
#include "ini_reader.hpp"

using std::string;
namespace jnet {
	ini_reader::ini_reader(string filename)
	{
		_error = ini_parse(filename.c_str(), ValueHandler, this);
	}

	int ini_reader::ParseError()
	{
		return _error;
	}

	string ini_reader::Get(string section, string name, string default_value)
	{
		if (ParseError() < 0)
			return default_value;

		string key = MakeKey(section, name);
		return _values.count(key) ? _values[key] : default_value;
	}

	long ini_reader::GetInteger(string section, string name, long default_value)
	{
		if (ParseError() < 0)
			return default_value;

		string valstr = Get(section, name, "");
		const char* value = valstr.c_str();
		char* end;
		// This parses "1234" (decimal) and also "0x4D2" (hex)
		long n = strtol(value, &end, 0);
		return end > value ? n : default_value;
	}

	double ini_reader::GetReal(string section, string name, double default_value)
	{
		if (ParseError() < 0)
			return default_value;

		string valstr = Get(section, name, "");
		const char* value = valstr.c_str();
		char* end;
		double n = strtod(value, &end);
		return end > value ? n : default_value;
	}

	bool ini_reader::GetBoolean(string section, string name, bool default_value)
	{
		if (ParseError() < 0)
			return default_value;

		string valstr = Get(section, name, "");
		// Convert to lower case to make string comparisons case-insensitive
		std::transform(valstr.begin(), valstr.end(), valstr.begin(), ::tolower);
		if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1")
			return true;
		else if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0")
			return false;
		else
			return default_value;
	}

	string ini_reader::MakeKey(string section, string name)
	{
		string key = section + "." + name;
		// Convert to lower case to make section/name lookups case-insensitive
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		return key;
	}

	int ini_reader::ValueHandler(void* user, const char* section, const char* name,
		const char* value)
	{
		ini_reader* reader = (ini_reader*)user;
		string key = MakeKey(section, name);
		if (reader->_values[key].size() > 0)
			reader->_values[key] += "\n";
		reader->_values[key] += value;
		return 1;
	}
}