// SPDX-License-Identifier: GPL-2.0-or-later
/* tls_generic.h
 * Copyright (C) 2020 Feng Shun.
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as 
 *   published by the Free Software Foundation, either version 3 of the
 *   License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License version 2 for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   version 2 along with this program. If not, see
 *   <http://www.gnu.org/licenses/>.
 */
#include "defines_generic.h"
#include "ipc_generic.h"

#define PXCH_TLS_OFFSET_W32HOSTENT 0
#define PXCH_TLS_W32HOSTENT_IP_NUM 16
#define PXCH_TLS_W32HOSTENT_ALIAS_NUM 16
#define PXCH_TLS_W32HOSTENT_ALIAS_BUFSIZE 64

extern PXCH_DLL_API const PXCH_UINT32 g_dwW32HostentSize;
extern PXCH_DLL_API const PXCH_UINT32 g_dwW32SystemTimeSize;

#define PXCH_TLS_OFFSET_W32HOSTENT_IP_PTR_LIST (PXCH_TLS_OFFSET_W32HOSTENT + g_dwW32HostentSize)
#define PXCH_TLS_OFFSET_W32HOSTENT_IP_BUF (PXCH_TLS_OFFSET_W32HOSTENT_IP_PTR_LIST + sizeof(PXCH_UINT32*[PXCH_TLS_W32HOSTENT_IP_NUM]))
#define PXCH_TLS_OFFSET_W32HOSTENT_ALIAS_PTR_LIST (PXCH_TLS_OFFSET_W32HOSTENT_IP_BUF + sizeof(PXCH_UINT32[PXCH_TLS_W32HOSTENT_IP_NUM]))
#define PXCH_TLS_OFFSET_W32HOSTENT_ALIAS_BUF (PXCH_TLS_OFFSET_W32HOSTENT_ALIAS_PTR_LIST + sizeof(char*[PXCH_TLS_W32HOSTENT_ALIAS_NUM]))
#define PXCH_TLS_OFFSET_W32HOSTENT_HOSTNAME_BUF (PXCH_TLS_OFFSET_W32HOSTENT_ALIAS_PTR_LIST + sizeof(char[PXCH_TLS_W32HOSTENT_ALIAS_NUM][PXCH_TLS_W32HOSTENT_ALIAS_BUFSIZE]))

#define PXCH_TLS_OFFSET_LOG_TIME (PXCH_TLS_OFFSET_W32HOSTENT_HOSTNAME_BUF + sizeof(char[PXCH_MAX_HOSTNAME_BUFSIZE]))
#define PXCH_TLS_OFFSET_LOG_SZLOGLINE (PXCH_TLS_OFFSET_LOG_TIME + g_dwW32SystemTimeSize)
#define PXCH_TLS_OFFSET_LOG_MSG (PXCH_TLS_OFFSET_LOG_SZLOGLINE + sizeof(wchar_t[PXCH_MAX_FWPRINTF_BUFSIZE]))
#define PXCH_TLS_OFFSET_LOG_RESPMSG (PXCH_TLS_OFFSET_LOG_MSG + sizeof(PXCH_IPC_MSGBUF))
#define PXCH_TLS_OFFSET_LOG_CBMSGSIZE (PXCH_TLS_OFFSET_LOG_RESPMSG + sizeof(PXCH_IPC_MSGBUF))
#define PXCH_TLS_OFFSET_LOG_CBRESPMSGSIZE (PXCH_TLS_OFFSET_LOG_CBMSGSIZE + sizeof(PXCH_UINT32))
#define PXCH_TLS_OFFSET_LOG_PID (PXCH_TLS_OFFSET_LOG_CBRESPMSGSIZE + sizeof(PXCH_UINT32))
#define PXCH_TLS_OFFSET_LOG_CYGPID (PXCH_TLS_OFFSET_LOG_PID + sizeof(PXCH_UINT32))
#define PXCH_TLS_OFFSET_LOG_TID (PXCH_TLS_OFFSET_LOG_CYGPID + sizeof(PXCH_UINT32))
#define PXCH_TLS_OFFSET_LOG_ODS_BUF (PXCH_TLS_OFFSET_LOG_TID + sizeof(PXCH_UINT32))
#define PXCH_TLS_OFFSET_DUMP_MEMORY_BUF (PXCH_TLS_OFFSET_LOG_ODS_BUF + sizeof(wchar_t[PXCH_LOG_ODS_BUFSIZE]))

#define PXCH_TLS_TOTAL_SIZE (PXCH_TLS_OFFSET_DUMP_MEMORY_BUF + sizeof(wchar_t[PXCH_MAX_DUMP_MEMORY_BUFSIZE]))


#define PXCH_TLS_PTR_LOG_SZLOGLINE_BY_BASE(base) ((wchar_t*)((char*)base + PXCH_TLS_OFFSET_LOG_SZLOGLINE))
#define PXCH_TLS_PTR_LOG_MSG_BY_BASE(base) ((char*)base + PXCH_TLS_OFFSET_LOG_MSG)
#define PXCH_TLS_PTR_LOG_RESPMSG_BY_BASE(base) ((char*)base + PXCH_TLS_OFFSET_LOG_RESPMSG)
#define PXCH_TLS_PTR_LOG_CBMSGSIZE_BY_BASE(base) ((PXCH_UINT32*)((char*)base + PXCH_TLS_OFFSET_LOG_CBMSGSIZE))
#define PXCH_TLS_PTR_LOG_CBRESPMSGSIZE_BY_BASE(base) ((PXCH_UINT32*)((char*)base + PXCH_TLS_OFFSET_LOG_CBRESPMSGSIZE))
#define PXCH_TLS_PTR_LOG_PID_BY_BASE(base) ((PXCH_UINT32*)((char*)base + PXCH_TLS_OFFSET_LOG_PID))
#define PXCH_TLS_PTR_LOG_CYGPID_BY_BASE(base) ((PXCH_UINT32*)((char*)base + PXCH_TLS_OFFSET_LOG_CYGPID))
#define PXCH_TLS_PTR_LOG_TID_BY_BASE(base) ((PXCH_UINT32*)((char*)base + PXCH_TLS_OFFSET_LOG_TID))
#define PXCH_TLS_PTR_LOG_ODS_BUF_BY_BASE(base) ((wchar_t*)((char*)base + PXCH_TLS_OFFSET_LOG_ODS_BUF))
#define PXCH_TLS_PTR_DUMP_MEMORY_BUF_BY_BASE(base) ((wchar_t*)((char*)base + PXCH_TLS_OFFSET_DUMP_MEMORY_BUF))

#define PXCH_TLS_PTR_LOG_SZLOGLINE(dwTlsIndex) PXCH_TLS_PTR_LOG_SZLOGLINE_BY_BASE(TlsGetValue(dwTlsIndex))
#define PXCH_TLS_PTR_LOG_MSG(dwTlsIndex) PXCH_TLS_PTR_LOG_MSG_BY_BASE(TlsGetValue(dwTlsIndex))
#define PXCH_TLS_PTR_LOG_RESPMSG(dwTlsIndex) PXCH_TLS_PTR_LOG_RESPMSG_BY_BASE(TlsGetValue(dwTlsIndex))
#define PXCH_TLS_PTR_LOG_CBMSGSIZE(dwTlsIndex) PXCH_TLS_PTR_LOG_CBMSGSIZE_BY_BASE(TlsGetValue(dwTlsIndex))
#define PXCH_TLS_PTR_LOG_CBRESPMSGSIZE(dwTlsIndex) PXCH_TLS_PTR_LOG_CBRESPMSGSIZE_BY_BASE(TlsGetValue(dwTlsIndex))
#define PXCH_TLS_PTR_LOG_PID(dwTlsIndex) PXCH_TLS_PTR_LOG_PID_BY_BASE(TlsGetValue(dwTlsIndex))
#define PXCH_TLS_PTR_LOG_CYGPID(dwTlsIndex) PXCH_TLS_PTR_LOG_CYGPID_BY_BASE(TlsGetValue(dwTlsIndex))
#define PXCH_TLS_PTR_LOG_TID(dwTlsIndex) PXCH_TLS_PTR_LOG_TID_BY_BASE(TlsGetValue(dwTlsIndex))
#define PXCH_TLS_PTR_LOG_ODS_BUF(dwTlsIndex) PXCH_TLS_PTR_LOG_ODS_BUF_BY_BASE(TlsGetValue(dwTlsIndex))
#define PXCH_TLS_PTR_DUMP_MEMORY_BUF(dwTlsIndex) PXCH_TLS_PTR_DUMP_MEMORY_BUF_BY_BASE(TlsGetValue(dwTlsIndex))