#pragma once
#ifdef __CYGWIN__
#include <sys/cygwin.h>
#endif

#include "common_win32.h"
#include "defines_win32.h"
#include "ipc_win32.h"
#include "log_generic.h"
#include "tls_win32.h"

// Per-process, will cause race condition, only used at early stages of DLL loading and hook initializing
extern SYSTEMTIME log_time_early;
extern wchar_t log_szLogLine_early[PXCH_LOG_IPC_BUFSIZE];
extern PXCH_IPC_MSGBUF log_msg_early;
extern PXCH_IPC_MSGBUF log_respMsg_early;
extern PXCH_UINT32 log_cbMsgSize_early;
extern PXCH_UINT32 log_cbRespMsgSize_early;
extern PXCH_UINT32 log_pid_early;
extern PXCH_UINT32 log_tid_early;
extern wchar_t log_ods_buf_early[PXCH_LOG_ODS_BUFSIZE];

#ifdef __CYGWIN__
extern PXCH_UINT32 log_cygpid_early;
#endif

#ifdef __CYGWIN__
static void __attribute__((unused)) suppress_unused_variables(void)
{
	(void)log_time_early;
	(void)log_szLogLine_early;
	(void)log_time_early;
	(void)log_msg_early;
	(void)log_respMsg_early;
	(void)log_cbMsgSize_early;
	(void)log_cbRespMsgSize_early;
	(void)log_pid_early;
	(void)log_tid_early;
	(void)log_cygpid_early;
}
#endif

// Per-thread(in TLS), thread safe
#define log_time (*(g_dwTlsIndex ? PXCH_TLS_PTR_LOG_TIME(g_dwTlsIndex) : &log_time_early))
#define log_szLogLine (g_dwTlsIndex ? PXCH_TLS_PTR_LOG_SZLOGLINE(g_dwTlsIndex) : log_szLogLine_early)
#define log_msg (g_dwTlsIndex ? PXCH_TLS_PTR_LOG_MSG(g_dwTlsIndex) : log_msg_early)
#define log_respMsg (g_dwTlsIndex ? PXCH_TLS_PTR_LOG_RESPMSG(g_dwTlsIndex) : log_respMsg_early)
#define log_cbMsgSize (*(g_dwTlsIndex ? PXCH_TLS_PTR_LOG_CBMSGSIZE(g_dwTlsIndex) : &log_cbMsgSize_early))
#define log_cbRespMsgSize (*(g_dwTlsIndex ? PXCH_TLS_PTR_LOG_CBRESPMSGSIZE(g_dwTlsIndex) : &log_cbRespMsgSize_early))
#define log_pid (*(g_dwTlsIndex ? PXCH_TLS_PTR_LOG_PID(g_dwTlsIndex) : &log_pid_early))
#define log_cygpid (*(g_dwTlsIndex ? PXCH_TLS_PTR_LOG_CYGPID(g_dwTlsIndex) : &log_cygpid_early))
#define log_tid (*(g_dwTlsIndex ? PXCH_TLS_PTR_LOG_TID(g_dwTlsIndex) : &log_tid_early))
#define log_ods_buf (g_dwTlsIndex ? PXCH_TLS_PTR_LOG_ODS_BUF(g_dwTlsIndex) : log_ods_buf_early)

#define PXCH_LOG_IPC_PID_QUERY_CYG() log_pid = GetCurrentProcessId(); log_cygpid = g_bCurrentlyInWinapiCall ? -1 : cygwin_winpid_to_pid(log_pid)
#define PXCH_LOG_IPC_PID_VALUE_CYG log_cygpid, log_pid
#define PXCH_LOG_IPC_PID_QUERY_WIN() log_pid = GetCurrentProcessId();
#define PXCH_LOG_IPC_PID_VALUE_WIN log_pid

#ifdef __CYGWIN__
#define PXCH_LOG_IPC_PID_QUERY() PXCH_LOG_IPC_PID_QUERY_CYG()
#define PXCH_LOG_IPC_PID_VALUE   PXCH_LOG_IPC_PID_VALUE_CYG
#else
#define PXCH_LOG_IPC_PID_QUERY() PXCH_LOG_IPC_PID_QUERY_WIN()
#define PXCH_LOG_IPC_PID_VALUE   PXCH_LOG_IPC_PID_VALUE_WIN
#endif

#define PXCH_LOG_REAL(real_fmt, ...) \
	do { \
		GetLocalTime(&log_time); \
		StdWprintf(STD_OUTPUT_HANDLE, real_fmt, log_time.wYear, log_time.wMonth, log_time.wDay, log_time.wHour, log_time.wMinute, log_time.wSecond, ##__VA_ARGS__); \
		StdFlush(STD_OUTPUT_HANDLE); \
	} while(0)

#define PXCH_LOG_REAL_E(real_fmt, ...) \
	do { \
		GetLocalTime(&log_time); \
		StdWprintf(STD_ERROR_HANDLE, real_fmt, log_time.wYear, log_time.wMonth, log_time.wDay, log_time.wHour, log_time.wMinute, log_time.wSecond, ##__VA_ARGS__); \
		StdFlush(STD_ERROR_HANDLE); \
	} while(0)

#define PXCH_LOG_IPC_REAL(real_fmt, ...) do { \
	GetLocalTime(&log_time); \
	log_szLogLine[0] = L'\0'; \
	StringCchPrintfW(log_szLogLine, MAX_FWPRINTF_BUFSIZE, real_fmt, PXCH_LOG_IPC_PID_VALUE, log_time.wYear, log_time.wMonth, log_time.wDay, log_time.wHour, log_time.wMinute, log_time.wSecond, ##__VA_ARGS__); \
	if (log_szLogLine[MAX_FWPRINTF_BUFSIZE - 2]) log_szLogLine[MAX_FWPRINTF_BUFSIZE - 2] = L'\n'; \
	log_szLogLine[MAX_FWPRINTF_BUFSIZE - 1] = L'\0'; \
	WstrToMessage(log_msg, &log_cbMsgSize, log_szLogLine); \
	IpcCommunicateWithServer(log_msg, log_cbMsgSize, log_respMsg, &log_cbRespMsgSize); \
} while(0)

#define PXCH_LOG_IPC(level, fmt, ...) \
	do { \
		PXCH_LOG_IPC_PID_QUERY(); \
		if (g_pPxchConfig && log_pid == g_pPxchConfig->dwMasterProcessId) {\
			PXCH_LOG_REAL(PXCH_LOG_CONCAT_FMT(level, fmt), ##__VA_ARGS__); \
		} else { \
			PXCH_LOG_IPC_REAL(PXCH_LOG_IPC_CONCAT_FMT(level, fmt), ##__VA_ARGS__); \
		} \
	} while(0)

#define PXCH_LOG_IPC_E(level, fmt, ...) \
	do { \
		PXCH_LOG_IPC_PID_QUERY(); \
		if (g_pPxchConfig && log_pid == g_pPxchConfig->dwMasterProcessId) {\
			PXCH_LOG_REAL_E(PXCH_LOG_CONCAT_FMT(level, fmt), ##__VA_ARGS__); \
		} else { \
			PXCH_LOG_IPC_REAL(PXCH_LOG_IPC_CONCAT_FMT(level, fmt), ##__VA_ARGS__); \
		} \
	} while(0)

#define PXCH_LOG(level, fmt, ...) PXCH_LOG_REAL(PXCH_LOG_CONCAT_FMT(level, fmt), ##__VA_ARGS__)

#define PXCH_LOG_E(level, fmt, ...) PXCH_LOG_REAL_E(PXCH_LOG_CONCAT_FMT(level, fmt), ##__VA_ARGS__)

#if PXCH_LOG_LEVEL >= PXCH_LOG_LEVEL_DEBUG
#define ODBGSTRLOG(fmt, ...) do { StringCchPrintfW(log_ods_buf, PXCH_LOG_ODS_BUFSIZE, fmt, ##__VA_ARGS__); OutputDebugStringW(log_ods_buf); } while(0)
#else
#define ODBGSTRLOG(...)
#endif

#if PXCH_LOG_LEVEL >= PXCH_LOG_LEVEL_CRITICAL
#define LOGC(fmt, ...) PXCH_LOG_E(C, fmt, ##__VA_ARGS__)
#define IPCLOGC(fmt, ...) PXCH_LOG_IPC_E(C, fmt, ##__VA_ARGS__)
#else
#define LOGC(...)
#define IPCLOGC(...)
#endif

#if PXCH_LOG_LEVEL >= PXCH_LOG_LEVEL_ERROR
#define LOGE(fmt, ...) PXCH_LOG_E(E, fmt, ##__VA_ARGS__)
#define IPCLOGE(fmt, ...) PXCH_LOG_IPC_E(E, fmt, ##__VA_ARGS__)
#else
#define LOGE(...)
#define IPCLOGE(...)
#endif

#if PXCH_LOG_LEVEL >= PXCH_LOG_LEVEL_WARNING
#define LOGW(fmt, ...) PXCH_LOG(W, fmt, ##__VA_ARGS__)
#define IPCLOGW(fmt, ...) PXCH_LOG_IPC(W, fmt, ##__VA_ARGS__)
#else
#define LOGW(...)
#define IPCLOGW(...)
#endif

#if PXCH_LOG_LEVEL >= PXCH_LOG_LEVEL_INFO
#define LOGI(fmt, ...) PXCH_LOG(I, fmt, ##__VA_ARGS__)
#define IPCLOGI(fmt, ...) PXCH_LOG_IPC(I, fmt, ##__VA_ARGS__)
#else
#define LOGI(...)
#define IPCLOGI(...)
#endif

#if PXCH_LOG_LEVEL >= PXCH_LOG_LEVEL_DEBUG
#define LOGD(fmt, ...) PXCH_LOG(D, fmt, ##__VA_ARGS__)
#define IPCLOGD(fmt, ...) PXCH_LOG_IPC(D, fmt, ##__VA_ARGS__)
#else
#define LOGD(...)
#define IPCLOGD(...)
#endif

#if PXCH_LOG_LEVEL >= PXCH_LOG_LEVEL_VERBOSE
#define LOGV(fmt, ...) PXCH_LOG(V, fmt, ##__VA_ARGS__)
#define IPCLOGV(fmt, ...) PXCH_LOG_IPC(V, fmt, ##__VA_ARGS__)
#else
#define LOGV(...)
#define IPCLOGV(...)
#endif
