#include "xplatform.hpp"

#if	defined(_MSC_VER)
#include <Windows.h>
#include <detours.h>
#else
#include "plthook/plthook.h"

#endif

#include "hooker.hpp"

#include "logging.hpp"

#include "john.hpp"

#if	defined(_MSC_VER)
int (WSAAPI * sys_recvfrom)(SOCKET s, char * buf, int len, int flags, struct sockaddr * from, int *fromlen) = recvfrom;
int (WSAAPI * sys_recv)(SOCKET s, char * buf, int len, int flags) = recv;

int (WSAAPI * sys_sendto)(SOCKET s, const char FAR * buf, int len, int flags, const struct sockaddr FAR * to, int tolen) = sendto;
int (WSAAPI * sys_send)(SOCKET s, const char FAR * buf, int len, int flags) = send;

int (WSAAPI * sys_WSARecv)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) = WSARecv;
int (WSAAPI * sys_WSARecvFrom)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD dwFlags, struct sockaddr * from, int *fromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) = WSARecvFrom;
int (WSAAPI * sys_WSASend)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) = WSASend;
HMODULE(WINAPI *sys_LoadLibraryA)(LPCSTR) = LoadLibraryA;

#endif

namespace jnet {
	int hooker::recv_track(SOCKET s, char *buffer, int len, struct sockaddr * from, socklen_t fromlen) {
		if(g_Engine) {
			return g_Engine->recv(s, buffer, len, from, fromlen);
		}
		return -1;
	}

	int hooker::send_track(SOCKET s, const char *buffer, int len, const struct sockaddr * dst, socklen_t dstlen) {
		if(g_Engine) {
			return g_Engine->send(s, const_cast<char *>(buffer), len, const_cast<sockaddr *>(dst), dstlen);
		}
		return -1;
	}

	void hooker::initialize() {
#if	defined(_MSC_VER)
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourAttach(&(PVOID &)sys_recv, h_recv);
		DetourAttach(&(PVOID &)sys_recvfrom, h_recvfrom);
		DetourAttach(&(PVOID &)sys_WSARecv, h_WSARecv);
		DetourAttach(&(PVOID &)sys_WSARecvFrom, h_WSARecvFrom);

		DetourAttach(&(PVOID &)sys_send, h_send);
		DetourAttach(&(PVOID &)sys_sendto, h_sendto);
		DetourAttach(&(PVOID &)sys_WSASend, h_WSASend);

		DetourAttach(&(PVOID &)sys_LoadLibraryA, h_LoadLibraryA);
		DetourTransactionCommit();
#else
//#ifdef _DEAD
		// Do linux attach
		plthook_t *plthook;

		//if (plthook_open(&plthook, "libstdc++.so.6") != 0) {
		//	printf("plthook_open error: %s\n", plthook_error());
		//	return;
		//}
		if (plthook_open(&plthook, NULL) != 0) {
			printf("plthook_open error: %s\n", plthook_error());
			return;
		}
		
		if (plthook_replace(plthook, "recv", (void*)h_recv, NULL) != 0) {
			printf("plthook_replace error: %s\n", plthook_error());
			plthook_close(plthook);
			return;
		}
		if (plthook_replace(plthook, "recvfrom", (void*)h_recvfrom, NULL) != 0) {
			printf("plthook_replace error: %s\n", plthook_error());
			plthook_close(plthook);
			return;
		}

		if (plthook_replace(plthook, "send", (void*)h_recv, NULL) != 0) {
			printf("plthook_replace error: %s\n", plthook_error());
			plthook_close(plthook);
			return;
		}
		if (plthook_replace(plthook, "sendto", (void*)h_sendto, NULL) != 0) {
			printf("plthook_replace error: %s\n", plthook_error());
			plthook_close(plthook);
			return;
		}

		plthook_close(plthook);
//#endif
#endif
	}

	void hooker::release() {
#if	defined(_MSC_VER)
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourDetach(&(PVOID &)sys_recv, h_recv);
		DetourDetach(&(PVOID &)sys_recvfrom, h_recvfrom);
		DetourDetach(&(PVOID &)sys_WSARecv, h_WSARecv);
		DetourDetach(&(PVOID &)sys_WSARecvFrom, h_WSARecvFrom);

		DetourDetach(&(PVOID &)sys_send, h_send);
		DetourDetach(&(PVOID &)sys_sendto, h_sendto);
		DetourDetach(&(PVOID &)sys_WSASend, h_WSASend);

		DetourDetach(&(PVOID &)sys_LoadLibraryA, h_LoadLibraryA);
		if ((DetourTransactionCommit()) == NO_ERROR) {
			LOG(DEBUG) << "Detours successful";
		}
#else
		// Do linux detach
		printf("Placeholder");
#endif
	}

	hooker::hooker() {
		//initialize();
	}
	hooker::~hooker() {
		//release();
	}

	int WSAAPI h_recvfrom(SOCKET s, char * buf, int len, int flags, struct sockaddr * from, int * fromlen) {
		//LOG(DEBUG) << "h_recvfrom() entered";

		int retVal = sys_recvfrom(s, buf, len, flags, from, reinterpret_cast<socklen_t*>(fromlen));
		int ret = jnet::hooker::recv_track(s, buf, len, from, *fromlen);

		if (ret) {
			return retVal;
		} else {
			return 0;
		}

		return retVal;
	}
	int WSAAPI h_recv(SOCKET s, char * buf, int len, int flags) {
		//LOG(DEBUG) << "h_recv() entered";

		int retVal = sys_recv(s, buf, len, flags);
		int ret = jnet::hooker::recv_track(s, buf, len, 0, 0);

		if (ret) {
			return retVal;
		} else {
			return 0;
		}

	}

	int WSAAPI h_sendto(SOCKET s, const char * buf, int len, int flags, const struct sockaddr * to, int tolen) {
		//LOG(DEBUG) << "h_sendto() entered";

		int ret = jnet::hooker::send_track(s, buf, len, to, tolen);

		return sys_sendto(s, buf, len, flags, to, ((socklen_t)tolen));
	}
	int WSAAPI h_send(SOCKET s, const char FAR * buf, int len, int flags) {
		//LOG(DEBUG) << "h_send() entered";

		int ret = jnet::hooker::send_track(s, buf, len, 0, 0);

		return sys_send(s, buf, len, flags);
	}

#if	defined(_MSC_VER)
	int WSAAPI h_WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
		//LOG(DEBUG) << "h_WSARecv() entered";

		int retVal = sys_WSARecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, dwFlags, lpOverlapped, lpCompletionRoutine);

		int ret = jnet::hooker::recv_track(s, lpBuffers->buf, lpBuffers->len, 0, 0);

		if (ret) {
			return retVal;
		}
		else {
			return 0;
		}
	}
	int WSAAPI h_WSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD dwFlags, struct sockaddr * from, int *fromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
		//LOG(DEBUG) << "h_WSARecv() entered";

		int retVal = sys_WSARecvFrom(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, dwFlags, from, fromlen, lpOverlapped, lpCompletionRoutine);
		int ret = jnet::hooker::recv_track(s, lpBuffers->buf, lpBuffers->len, from, *fromlen);

		if (ret) {
			return retVal;
		}
		else {
			return 0;
		}
	}

	int WSAAPI h_WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
		//LOG(DEBUG) << "h_WSASend() entered";

		int ret = jnet::hooker::send_track(s, (char *)lpBuffers, dwBufferCount, 0, 0);

		return sys_WSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
	}

	HMODULE WINAPI h_LoadLibraryA(LPCSTR module) {
		//LOG(DEBUG) << "h_LoadLibraryA() entered";

		return sys_LoadLibraryA(module);
	}
#endif
};
