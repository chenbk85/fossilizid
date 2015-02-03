#include <json_plugin.h>
#include <juggle.h>
#include <boost/make_shared.hpp>

class juggle: public Fossilizid::juggle::module{
public:
	juggle() : module("juggle", Fossilizid::uuid::UUID()){
		Fossilizid::juggle::_service_handle->register_module_method("juggle_login", boost::bind(&juggle::call_login, this, _1, _2));
		Fossilizid::juggle::_service_handle->register_module_method("juggle_test", boost::bind(&juggle::call_test, this, _1, _2));
	}
	~juggle(){
	}

	virtual std::string login(std::string argv3) = 0;
	void call_login(Fossilizid::juggle::channel * ch, boost::shared_ptr<Fossilizid::juggle::object> v){
		auto argv3 = (*v)["argv3"].asstring();
		auto ret = login(argv3);
		boost::shared_ptr<Fossilizid::juggle::object> r = boost::make_shared<Fossilizid::jsonplugin::object>();
		(*r)["suuid"] = (*v)["suuid"].asstring();
		(*r)["method"] = (*v)["method"].asstring();
		(*r)["rpcevent"] = "reply_rpc_method";

		(*r)["ret"] = ret;
		ch->push(r);
	}

	virtual std::string test(std::string argv3) = 0;
	void call_test(Fossilizid::juggle::channel * ch, boost::shared_ptr<Fossilizid::juggle::object> v){
		auto argv3 = (*v)["argv3"].asstring();
		auto ret = test(argv3);
		boost::shared_ptr<Fossilizid::juggle::object> r = boost::make_shared<Fossilizid::jsonplugin::object>();
		(*r)["suuid"] = (*v)["suuid"].asstring();
		(*r)["method"] = (*v)["method"].asstring();
		(*r)["rpcevent"] = "reply_rpc_method";

		(*r)["ret"] = ret;
		ch->push(r);
	}
};
juggle* create_juggle();
