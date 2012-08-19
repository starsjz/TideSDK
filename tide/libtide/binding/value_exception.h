/*
 * Appcelerator Kroll - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _VALUE_EXCEPTION_H_
#define _VALUE_EXCEPTION_H_

#include <vector>
#include <string>
#include <map>

namespace tide
{

#ifndef OS_WIN32
	#pragma GCC visibility push(default)
#endif

	class TIDE_API ValueException : public std::exception
	{
		public:
		ValueException(ValueRef v);
		~ValueException() throw ();

		public:
		static ValueException FromString(const char* s);
		static ValueException FromString(std::string s);
		static ValueException FromFormat(const char* format, ...);
		static ValueException FromObject(KObjectRef o);
		ValueRef GetValue();
		SharedString DisplayString();
		std::string& ToString();

		private:
		ValueRef value;
		std::string displayString;
	};

#ifndef OS_WIN32
	#pragma GCC visibility pop
#endif
}

#endif
