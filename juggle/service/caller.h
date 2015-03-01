/*
 * caller.h
 *
 *  Created on: 2015-1-11
 *      Author: qianqians
 */
#ifndef _caller_h
#define _caller_h

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include "../interface/object.h"
#include "../interface/channel.h"

#include "../../uuid/uuid.h"

namespace Fossilizid{
namespace juggle{

class caller{
public:
	caller(channel * ch, std::string modulename);
	~caller();

public:
	/*
	 * get class name
	 */
	std::string module_name();

	/*
	 * get object id
	 */
	uuid::uuid module_id();
	
	/*
	 * call rpc mothed reliable
	 */
	boost::shared_ptr<object> call_module_method_sync_reliable(std::string methodname, boost::shared_ptr<object> value);

	/*
	* call rpc mothed fast
	*/
	boost::shared_ptr<object> call_module_method_sync_fast(std::string methodname, boost::shared_ptr<object> value);

	/*
	 * call rpc mothed reliable
	 */
	void call_module_method_async_reliable(std::string methodname, boost::shared_ptr<object> value, boost::function<void(boost::shared_ptr<object>)> callback);
	
	/*
	* call rpc mothed fast
	*/
	void call_module_method_async_fast(std::string methodname, boost::shared_ptr<object> value, boost::function<void(boost::shared_ptr<object>)> callback);

protected:
	std::string _module_name;
	uuid::uuid _module_id;
	channel * _ch;
	
};

} /* namespace juggle */
} /* namespace Fossilizid */

#endif //_caller_h