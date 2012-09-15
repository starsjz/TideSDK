/*
 * Appcelerator Kroll - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#ifndef _NET_H_
#define _NET_H_
#include <Poco/StringTokenizer.h>
#include <Poco/Net/IPAddress.h>
#include <Poco/URI.h>
namespace tide
{
	class Proxy;
	typedef SharedPtr<Proxy> SharedProxy;
	typedef SharedPtr<Poco::URI> SharedURI;
}
#include "proxy_config.h"
#endif
