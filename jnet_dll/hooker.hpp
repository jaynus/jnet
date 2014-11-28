#pragma once

#include "xplatform.hpp"
#if	defined(_MSC_VER)

extern int (WSAAPI * sys_recvfrom)(SOCKET s, char * buf, int len, int flags, struct sockaddr * from, int *fromlen);
extern int (WSAAPI * sys_recv)(SOCKET s, char * buf, int len, int flags);

extern int (WSAAPI * sys_sendto)(SOCKET s, const char FAR * buf, int len, int flags, const struct sockaddr FAR * to, int tolen);
extern int (WSAAPI * sys_send)(SOCKET s, const char FAR * buf, int len, int flags) ;

extern int (WSAAPI * sys_WSARecv)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
extern int (WSAAPI * sys_WSARecvFrom)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD dwFlags, struct sockaddr * from, int *fromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
extern int (WSAAPI * sys_WSASend)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
#else

#define sys_recvfrom recvfrom
#define sys_recv recv
#define sys_sendto sendto
#define sys_send send

#endif

namespace jnet {
	int WSAAPI h_recvfrom(SOCKET s, char * buf, int len, int flags, struct sockaddr * from, int * fromlen);
	int WSAAPI h_recv(SOCKET s, char * buf, int len, int flags);

	int WSAAPI h_sendto(SOCKET s, const char * buf, int len, int flags, const struct sockaddr * to, int tolen);
	int WSAAPI h_send(SOCKET s, const char FAR * buf, int len, int flags);
	
#if	defined(_MSC_VER)
	int WSAAPI h_WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
	int WSAAPI h_WSARecvFrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD dwFlags, struct sockaddr * from, int *fromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
	int WSAAPI h_WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
	HMODULE WINAPI h_LoadLibraryA(LPCSTR module);
#endif

	class hooker {
	public:
		hooker();
		~hooker();
	
		static int recv_track(SOCKET, char *buffer, int len, struct sockaddr * from, socklen_t fromlen);
		static int send_track(SOCKET, const char *buffer, int len, const struct sockaddr * dst, socklen_t dstlen);

		void initialize();
		void release();
	};
};