/**
 * Appcelerator Kroll - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */ 
#ifndef _ENVIRONMENT_BINDING_H_
#define _ENVIRONMENT_BINDING_H_

#include <tide/tide.h>

namespace tide
{
	class EnvironmentBinding : public Object
	{
	public:
		EnvironmentBinding() : Object("API.Environment") {}
		
		virtual ValueRef Get(const char *name);
		virtual SharedStringList GetPropertyNames();
		virtual void Set(const char *name, ValueRef value);
		virtual SharedString DisplayString(int levels=3);
	};
}

#endif
