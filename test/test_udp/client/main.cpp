#include <algorithm>

#include "../../remoteq/remote_queue.h"
#include "../../third_party/json/json_protocol.h"

int main(){
	Fossilizid::remoteq::ENDPOINT ep = Fossilizid::remoteq::endpoint("127.0.0.1", 4567);
	Fossilizid::remoteq::CHANNEL ch = Fossilizid::remoteq::fast::channel(ep);

	Json::Value begin;
	begin["ret"] = "begin";
	Fossilizid::remoteq::fast::push(ch, begin, Fossilizid::json_parser::json_to_buf);
		
	while (1){
		Json::Value ret;
		if (Fossilizid::remoteq::fast::pop(ch, ret, Fossilizid::json_parser::buf_to_json)){
			printf("test=%s\n", ret["test"].asString().c_str());

			Json::Value value;
			value["ret"] = "ok";
			Fossilizid::remoteq::fast::push(ch, value, Fossilizid::json_parser::json_to_buf);

			Sleep(1);

			break;
		}
	}

	return 0;
}
