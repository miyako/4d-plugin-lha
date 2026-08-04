/* --------------------------------------------------------------------------------
 #
 #	jsoncpp_bridge.h
 #	Replaces libjson_methods.h for the lha plugin.
 #	Bridges 4D's native text type (C_TEXT, UTF-16) to jsoncpp's Json::Value,
 #	which stores strings as UTF-8 std::string internally - so unlike the old
 #	libjson_methods bridge, there's no UTF-32 intermediate step needed.
 #
 #	Scope note: this only covers what 4DPlugin.cpp's LHA() command and its
 #	_cb2 callback actually call (parse a JSON filter list, walk a JSON array,
 #	build a JSON array of result paths, stringify it back out). The original
 #	libjson_methods.h exposed a much larger surface (NSString/NSDate helpers,
 #	object-key setters, base64, etc.) for other consumers that weren't part
 #	of what was reviewed here - if anything else in the project still calls
 #	those, it will need its own migration pass, not just this file.
 #
 # --------------------------------------------------------------------------------*/

#ifndef JSONCPP_BRIDGE_H
#define JSONCPP_BRIDGE_H

#include "4DPluginAPI.h"
#include "json.h"
#include <string>

/* C_TEXT (4D's native UTF-16 text) -> UTF-8 std::string, the encoding
   jsoncpp's Json::Value stores string values in. */
std::string json_bridge_to_utf8(C_TEXT &t);

/* UTF-8 std::string -> C_TEXT. */
void json_bridge_from_utf8(const std::string &s, C_TEXT &t);

/* Extracted-file path (CUTF16String, as produced by copyPath) -> UTF-8,
   for appending to the result array in _cb2. */
std::string json_bridge_to_utf8(CUTF16String *path);

/* Parse a C_TEXT holding JSON text into a Json::Value.
   Returns a null Value (isNull() == true, so isArray()/isString() etc. are
   all false) for an empty string, for invalid JSON, or for text that fails
   to parse for any other reason - callers must check the returned Value's
   actual type before using it rather than assuming success. This mirrors
   the original code's behavior of silently skipping filtering whenever
   json_parse returned NULL or the parsed root wasn't a JSON_ARRAY. */
Json::Value json_bridge_parse(C_TEXT &t);

/* Serialize a Json::Value back into a C_TEXT. Compact (single-line) by
   default; pass pretty=true for tab-indented output. */
void json_bridge_stringify(const Json::Value &v, C_TEXT &t, bool pretty = false);

#endif
