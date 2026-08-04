#include "jsoncpp_bridge.h"
#include <memory>

std::string json_bridge_to_utf8(C_TEXT &t)
{
	/* copyUTF8String / CUTF8String::c_str() is the same pair the original
	   plugin already relied on (see options.extract_path in LHA()), so this
	   introduces no new assumption about C_TEXT's interface. */
	CUTF8String u8;
	t.copyUTF8String(&u8);
	return std::string((const char *)u8.c_str());
}

void json_bridge_from_utf8(const std::string &s, C_TEXT &t)
{
	/* setUTF8String(const uint8_t*, uint32_t) is the pointer+length overload
	   the original copyPath() already used - deliberately not routed through
	   a CUTF8String constructor here, since only the null-terminated
	   single-argument CUTF8String constructor was ever demonstrated in the
	   source we were given; going straight from the std::string's own buffer
	   avoids assuming a length-taking CUTF8String overload that was never
	   actually observed. */
	t.setUTF8String((const uint8_t *)s.c_str(), (uint32_t)s.length());
}

std::string json_bridge_to_utf8(CUTF16String *path)
{
	C_TEXT t;
	t.setUTF16String(path);
	return json_bridge_to_utf8(t);
}

Json::Value json_bridge_parse(C_TEXT &t)
{
	std::string utf8 = json_bridge_to_utf8(t);

	if(utf8.empty())
		return Json::Value(); // null - "no filter", same as the old json_parse("") == NULL path

	Json::CharReaderBuilder builder;
	Json::Value root;
	std::string errs;

	const char *begin = utf8.c_str();
	const char *end = begin + utf8.length();

	std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if(!reader->parse(begin, end, &root, &errs))
		return Json::Value(); // invalid JSON - treated the same as "no filter"

	return root;
}

void json_bridge_stringify(const Json::Value &v, C_TEXT &t, bool pretty)
{
	Json::StreamWriterBuilder builder;
	builder["indentation"] = pretty ? "\t" : "";

	std::string utf8 = Json::writeString(builder, v);

	json_bridge_from_utf8(utf8, t);
}
