/*
 * channelimpl.h
 *
 *  Created on: 2014-10-3
 *      Author: qianqians
 */
#ifdef _WINDOWS
#include <WinSock2.h>

#ifndef _reliable_channelimpl_h
#define _reliable_channelimpl_h

#include <exception>
#include <boost/function.hpp>

#include "../../container/msque.h"
#include "../../pool/objpool.h"

#include "../../typedef.h"

#include "../../windows/handle.h"
#include "../../windows/overlapped.h"
#include "../../windows/queueimpl.h"

namespace Fossilizid{
namespace remoteq{

namespace reliable{

struct channelimpl : public handle{
	channelimpl(QUEUE _que, SOCKET s_){
		_handle_type = handle_reliable_channel_type;
		s = s_;
		que = _que;

		rindex = 0;
		windex = 0;
		buflen = 65536;
		buf = (char*)pool::mempool::allocator(buflen);
	}

	QUEUE que;
	container::msque<EVENT> evque;

	char * buf;
	int buflen, rindex, windex;
	SOCKET s;
};

template<class CMD, class CMDTOBUF>
bool push(CHANNEL ch, CMD & cmd, CMDTOBUF fn){
	boost::shared_ptr<std::string> buf = fn(cmd);

	WSABUF * wsabuf = pool::objpool<WSABUF>::allocator(1);
	wsabuf->buf = const_cast<char*>(buf->c_str());
	wsabuf->len = buf->size();
	DWORD bytes = 0;
	overlappedex * ovp = pool::objpool<overlappedex>::allocator(1);
	new (ovp)overlappedex();
	ovp->h = (handle*)(channelimpl*)((handle*)ch);
	ovp->type = iocp_type_tcp_send;
	ovp->sendbuf.buf = buf;
	OVERLAPPED * ovp_ = static_cast<OVERLAPPED *>(ovp);
	memset(ovp_, 0, sizeof(OVERLAPPED));
	if (WSASend(((channelimpl*)((handle*)ch))->s, wsabuf, 1, &bytes, 0, ovp_, 0) == SOCKET_ERROR){
		if (WSAGetLastError() != WSA_IO_PENDING){
			return false;
		}
	}

	return true;
}

template<class CMD, class BUFTOCMD>
bool pop(CHANNEL ch, CMD & cmd, BUFTOCMD fn){
	channelimpl * implch = (channelimpl*)((handle*)ch);
	if (((channelimpl*)((handle*)ch))->que == 0){
		while(1){
			char * buf = implch->buf + implch->windex;
			int buflen = implch->buflen - implch->windex;

			int len = recv(implch->s, buf, buflen, 0);
		
			if (len > 0){
				implch->windex += len;
			}

			if (len > 0 && len < buflen){
				break;
			}

			if (len < 0){
				break;
			}

			if (len == 0){
				break;
			}

			buflen = implch->buflen;
			implch->buflen *= 2;
			char * tmp = implch->buf;
			implch->buf = (char*)pool::mempool::allocator(implch->buflen);
			memcpy(implch->buf, tmp, buflen);
			pool::mempool::deallocator(tmp, buflen);
		}
	}

	if (implch->windex == implch->rindex){
		return false;
	}

	bool ret = true;
	do{
		if (implch->windex > 0){
			int cmdbuflen = 0;
			if ((cmdbuflen = fn(cmd, implch->buf + implch->rindex, implch->windex)) < 0){
				memmove(implch->buf, implch->buf + implch->rindex, implch->windex - implch->rindex);
				implch->windex -= implch->rindex;
				implch->rindex = 0;

				if (implch->windex == implch->buflen){
					auto buflen = implch->buflen;
					implch->buflen *= 2;
					char * tmp = implch->buf;
					implch->buf = (char*)pool::mempool::allocator(implch->buflen);
					memcpy(implch->buf, tmp, buflen);
					pool::mempool::deallocator(tmp, buflen);
				}
				ret = false;
			}
			implch->rindex += cmdbuflen;
		}
		
		if (implch->rindex != implch->windex){
			break;
		}
		
		implch->rindex = 0;
		implch->windex = 0;

		if (((channelimpl*)((handle*)ch))->que != 0){
			WSABUF * wsabuf = pool::objpool<WSABUF>::allocator(1);
			wsabuf->buf = implch->buf + implch->windex;
			wsabuf->len = implch->buflen - implch->windex;
			DWORD bytes = 0;
			DWORD flags = 0;
			overlappedex * ovp = pool::objpool<overlappedex>::allocator(1);
			new (ovp) overlappedex();
			ovp->h = (handle*)(channelimpl*)((handle*)ch);
			ovp->type = iocp_type_tcp_recv;
			OVERLAPPED * ovp_ = static_cast<OVERLAPPED *>(ovp);
			memset(ovp_, 0, sizeof(OVERLAPPED));
			if (WSARecv(implch->s, wsabuf, 1, &bytes, &flags, ovp_, 0) == SOCKET_ERROR){
				if (WSAGetLastError() != WSA_IO_PENDING){
					return false;
				}
			}
		}
		
	}while (0);
	
	return ret;
}

} /* namespace reliable */

} /* namespace remoteq */
} /* namespace Fossilizid */

#endif //_reliable_channelimpl_h

#endif //_WINDOWS
