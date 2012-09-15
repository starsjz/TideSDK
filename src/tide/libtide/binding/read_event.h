/*
 * Appcelerator Kroll - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _READ_EVENT_H_
#define _READ_EVENT_H_

namespace tide
{
	class TIDE_API ReadEvent : public Event
	{
	public:
		ReadEvent(AutoPtr<KEventObject> target, BytesRef);
		void _GetData(const ValueList&, ValueRef result);

	protected:
		BytesRef data;
	};
}
#endif