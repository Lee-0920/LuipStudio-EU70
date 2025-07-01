/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _THRIFT_WINDOWS_CONFIG_H_
#define _THRIFT_WINDOWS_CONFIG_H_ 1

// use std::thread
#define USE_STD_THREAD 1

// has stdint.h
#define HAVE_STDINT_H 1

//#if defined(_M_IX86) || defined(_M_X64)
#define ARITHMETIC_RIGHT_SHIFT 1
#define SIGNED_RIGHT_SHIFT_IS 1
//#endif

//#pragma warning(disable : 4996) // Deprecated posix name.

#define VERSION "0.10.0"
#define HAVE_GETTIMEOFDAY 1
#define HAVE_SYS_STAT_H 1

// Must be using VS2010 or later, or boost, so that C99 types are defined in the global namespace
#ifdef HAVE_STDINT_H
#include <stdint.h>
#else
#include <boost/cstdint.hpp>

typedef boost::int64_t int64_t;
typedef boost::uint64_t uint64_t;
typedef boost::int32_t int32_t;
typedef boost::uint32_t uint32_t;
typedef boost::int16_t int16_t;
typedef boost::uint16_t uint16_t;
typedef boost::int8_t int8_t;
typedef boost::uint8_t uint8_t;
#endif

#define  HAVE_NETINET_IN_H 1
// Linux
//#include <Winsock2.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>

#endif // _THRIFT_WINDOWS_CONFIG_H_
