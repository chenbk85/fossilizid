/*
 * channel.h
 *
 *  Created on: 2014-10-3
 *      Author: qianqians
 */
#ifndef _channel_h
#define _channel_h

#include "typedef.h"

#ifdef _WINDOWS
#include "windows/channelimpl.h"
#endif

namespace Fossilizid{
namespace remoteq {

namespace reliable{

/*
 * connect to remote endpoint
 */
CHANNEL connect(ENDPOINT ep, QUEUE que = 0);

} /* namespace reliable */

namespace fast{

/*
 * create a channel to remote endpoint
 */
CHANNEL channel(ENDPOINT ep, QUEUE que = 0);

} /* namespace fast */

} /* namespace remoteq */
} /* namespace Fossilizid */

#endif //_channel_h