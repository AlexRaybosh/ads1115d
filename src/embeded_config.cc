

#include <jsonutils.h>

extern "C" {
	extern char _binary_conf_json_start;
	extern char _binary_conf_json_end;
}


json::jsonptr readEmbededConfig() {
	return json::parse(&_binary_conf_json_start, &_binary_conf_json_end-&_binary_conf_json_start);
}


