#include <json_plugin.h>
#include <juggle.h>
#include <boost/make_shared.hpp>

namespace sync{

class juggle: public Fossilizid::juggle::caller{
public:
	juggle(Fossilizid::juggle::channel * ch) : caller(ch, "juggle"){
	}

	~juggle(){
	}

	std::string login(std::string argv3){
		boost::shared_ptr<Fossilizid::juggle::object> v = boost::make_shared<Fossilizid::jsonplugin::object>();
		(*v)["argv3"] = argv3;
		boost::shared_ptr<Fossilizid::juggle::object> r = call_module_method_sync("juggle_login", v);
		return (*r)["ret"].asstring();;
	}

	std::string test(std::string argv3){
		boost::shared_ptr<Fossilizid::juggle::object> v = boost::make_shared<Fossilizid::jsonplugin::object>();
		(*v)["argv3"] = argv3;
		boost::shared_ptr<Fossilizid::juggle::object> r = call_module_method_sync("juggle_test", v);
		return (*r)["ret"].asstring();;
	}

};

}

namespace async{

class juggle: public Fossilizid::juggle::caller{
public:
	juggle(Fossilizid::juggle::channel * ch) : caller(ch, "juggle"){
	}

	~juggle(){
	}

	std::string login(std::string argv3, boost::function<void(std::string)> callback){
		boost::shared_ptr<Fossilizid::juggle::object> v = boost::make_shared<Fossilizid::jsonplugin::object>();
		(*v)["argv3"] = argv3;
		auto cb = [this, callback](boost::shared_ptr<Fossilizid::juggle::object> r){
			std::string ret = (*r)["ret"].asstring();
			callback(ret);
		};
		call_module_method_async("juggle_login", v, cb);
	}

	std::string test(std::string argv3, boost::function<void(std::string)> callback){
		boost::shared_ptr<Fossilizid::juggle::object> v = boost::make_shared<Fossilizid::jsonplugin::object>();
		(*v)["argv3"] = argv3;
		auto cb = [this, callback](boost::shared_ptr<Fossilizid::juggle::object> r){
			std::string ret = (*r)["ret"].asstring();
			callback(ret);
		};
		call_module_method_async("juggle_test", v, cb);
	}

};

}

