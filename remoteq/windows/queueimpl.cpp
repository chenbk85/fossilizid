/*
 * queueimpl.cpp
 *
 *  Created on: 2014-10-3
 *      Author: qianqians
 */
#ifdef _WINDOWS
#include <WinSock2.h>

#include "../../pool/objpool.h"

#include "../acceptor.h"
#include "../queue.h"
#include "../endpoint.h"

#include "queueimpl.h"
#include "overlapped.h"
#include "acceptorimpl.h"
#include "channelimpl.h"
#include "endpointimpl.h"

namespace Fossilizid{
namespace remoteq {

QUEUE queue(){
	queueimpl * impl = new queueimpl;

	int corenum = 8;
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	corenum = info.dwNumberOfProcessors;

	impl->iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, corenum);

	return (QUEUE)((handle*)impl);
}

void dispense(EVENT & ev, DWORD bytes, handle * h){
	if (h->_handle_type == handle_fast_acceptor_type){
		fast::acceptorimlp * acp = (fast::acceptorimlp*)h;
		ev.type = event_type_fast_accept;
		ev.handle.acp = (ACCEPTOR)(acp);
		
		fast::channelimpl * ch = 0;
		int64_t key = ((endpointimpl*)acp->from)->addr.sin_addr.S_un.S_addr || (((uint64_t)((endpointimpl*)acp->from)->addr.sin_port) << 32);
		auto find = acp->channels.find(key);
		if (find == acp->channels.end()){
			ch = pool::objpool<fast::channelimpl>::allocator(1);
			new (ch)fast::channelimpl(acp->que, acp->s, endpoint(inet_ntoa(((endpointimpl*)acp->from)->addr.sin_addr), ((endpointimpl*)acp->from)->addr.sin_port));
			acp->chque.push(ch);
		}else{
			ch = (fast::channelimpl *)find->second;
		}
		if (bytes >= DWORD(ch->buflen - ch->windex)){
			auto buflen = ch->buflen;
			ch->buflen *= 2;
			char * tmp = ch->buf;
			ch->buf = (char*)pool::mempool::allocator(ch->buflen);
			memcpy(ch->buf, tmp, buflen);
			pool::mempool::deallocator(tmp, buflen);
		}
		memmove(ch->buf + ch->windex, acp->outbuf, bytes);

		EVENT recvev;
		recvev.type = event_type_fast_recv;
		recvev.handle.acp = (CHANNEL)(ch);
		((queueimpl *)acp->que)->evque.push(recvev);
	}else if (h->_handle_type == handle_fast_channel_type){
		ev.type = event_type_fast_recv;
		ev.handle.ch = (CHANNEL)((fast::channelimpl*)h);
	}
}

EVENT queue(QUEUE que){
	queueimpl * impl = (queueimpl *)((handle*)que);

	EVENT ev; 
	ev.type = event_type_none;
	ev.handle.acp = 0;
	
	do{
		if (impl->evque.pop(ev)){
			break;
		}

		DWORD bytes = 0;
		ULONG_PTR ptr = 0;
		LPOVERLAPPED ovp = 0;
		if (GetQueuedCompletionStatus(impl->iocp, &bytes, &ptr, &ovp, 0)){
			overlappedex * ovlp = static_cast<overlappedex *>(ovp);
		
			if (ovlp->type == iocp_type_tcp_accept){
				ev.type = event_type_reliable_accept;
				ev.handle.acp = (ACCEPTOR)((reliable::acceptorimlp*)ovlp->h);
			} else if (ovlp->type == iocp_type_tcp_recv){
				reliable::channelimpl* ch = ((reliable::channelimpl*)ovlp->h);
				if (bytes == 0){
					EVENT ev;
					ev.handle.ch = (CHANNEL)((reliable::channelimpl*)ovlp->h);
					ev.type = event_type_reliable_disconnect;
					impl->evque.push(ev);
				}else{
					ev.type = event_type_reliable_recv;
					ev.handle.ch = (CHANNEL)((reliable::channelimpl*)ovlp->h);
					((reliable::channelimpl*)ovlp->h)->windex += bytes;
				}
			} else if (ovlp->type == iocp_type_udp_recv){
				dispense(ev, bytes, ovlp->h);
			} 

			pool::objpool<overlappedex>::deallocator(ovlp, 1);
		}
	} while (0);

	return ev;
}

} /* namespace remoteq */
} /* namespace Fossilizid */

#endif //_WINDOWS