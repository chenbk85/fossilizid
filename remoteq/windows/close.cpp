/*
 * close.cpp
 *
 *  Created on: 2014-10-9
 *      Author: qianqians
 */
#include "../../pool/objpool.h"

#include "../close.h"
#include "queueimpl.h"
#include "endpointimpl.h"
#include "channelimpl.h"
#include "acceptorimpl.h"

namespace Fossilizid{
namespace remoteq {

void close(HANDLE _handle){
	handle * h = (handle*)_handle;
	switch(h->_handle_type)
	{
	case handle_reliable_channel_type:
		{
			reliable::channelimpl * impl = (reliable::channelimpl*)h;
			pool::objpool<reliable::channelimpl>::deallocator(impl, 1);
		}
		break;

	case handle_fast_channel_type:
		{
			fast::channelimpl * impl = (fast::channelimpl*)h;
			pool::objpool<fast::channelimpl>::deallocator(impl, 1);
		}
		break;
	
	case handle_reliable_acceptor_type:
		{
			reliable::acceptorimlp * impl = (reliable::acceptorimlp*)h;
			pool::objpool<reliable::acceptorimlp>::deallocator(impl, 1);
		}
		break;
	
	case handle_fast_acceptor_type:
		{
			fast::acceptorimlp * impl = (fast::acceptorimlp*)h;
			pool::objpool<fast::acceptorimlp>::deallocator(impl, 1);
		}
		break;

	case handle_queue_type:
		{
			queueimpl * impl = (queueimpl*)h;
			pool::objpool<queueimpl>::deallocator(impl, 1);
		}
		break;

	case handle_endpoint_type:
		{
			endpointimpl * impl = (endpointimpl*)h;
			pool::objpool<endpointimpl>::deallocator(impl, 1);
		}
		break;

	default:
		break;
	}
}

} /* namespace remoteq */
} /* namespace Fossilizid */