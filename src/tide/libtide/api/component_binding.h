/**
 * Appcelerator Kroll - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _COMPONENT_BINDING_H_
#define _COMPONENT_BINDING_H_

#include <tide/tide.h>
#include <map>
#include <vector>
#include <string>

namespace tide
{
	class ComponentBinding : public KAccessorObject
	{
		public:
		ComponentBinding(SharedComponent component);

		private:
		SharedComponent component;

		void _GetType(const ValueList& args, ValueRef value);
		void _GetName(const ValueList& args, ValueRef value);
		void _GetVersion(const ValueList& args, ValueRef value);
		void _GetPath(const ValueList& args, ValueRef value);
		void _IsBundled(const ValueList& args, ValueRef value);
		void _IsLoaded(const ValueList& args, ValueRef value);
	};
}

#endif