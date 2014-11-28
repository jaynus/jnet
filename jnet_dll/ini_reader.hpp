// Read an INI file into easy-to-access name/value pairs.

// inih and ini_reader are released under the New BSD license (see LICENSE.txt).
// Go to the project home page for more info:
//
// http://code.google.com/p/inih/

#ifndef __ini_reader_H__
#define __ini_reader_H__

#include <map>
#include <string>
namespace jnet {


	// Read an INI file into easy-to-access name/value pairs. (Note that I've gone
	// for simplicity here rather than speed, but it should be pretty decent.)
	class ini_reader
	{
	public:
		// Construct ini_reader and parse given filename. See ini.h for more info
		// about the parsing.
		ini_reader(std::string filename);

		// Return the result of ini_parse(), i.e., 0 on success, line number of
		// first error on parse error, or -1 on file open error.
		int ParseError();

		// Get a string value from INI file, returning default_value if not found.
		std::string Get(std::string section, std::string name,
			std::string default_value);

		// Get an integer (long) value from INI file, returning default_value if
		// not found or not a valid integer (decimal "1234", "-1234", or hex "0x4d2").
		long GetInteger(std::string section, std::string name, long default_value);

		// Get a real (floating point double) value from INI file, returning
		// default_value if not found or not a valid floating point value
		// according to strtod().
		double GetReal(std::string section, std::string name, double default_value);

		// Get a boolean value from INI file, returning default_value if not found or if
		// not a valid true/false value. Valid true values are "true", "yes", "on", "1",
		// and valid false values are "false", "no", "off", "0" (not case sensitive).
		bool GetBoolean(std::string section, std::string name, bool default_value);

	private:
		int _error;
		std::map<std::string, std::string> _values;
		static std::string MakeKey(std::string section, std::string name);
		static int ValueHandler(void* user, const char* section, const char* name,
			const char* value);
	};
}
#endif  // __ini_reader_H__
