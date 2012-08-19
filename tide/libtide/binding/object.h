/*
 * Appcelerator Kroll - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2008 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>
#include <string>
#include <map>
#include "callback.h"

namespace tide
{
	/**
	 * An abstract object.
	 *
	 * In general, Object implementations follow two patterns:
	 * - Dynamic implementation (subclassing Object directly)
	 *   This allows the implementation to do custom logic for specific
	 *   properties, but is also more verbose
	 * - Static implementation (subclassing StaticBoundObject)
	 *   This implementation uses an internal map to bind property names
	 *   to \link Value Values\endlink (objects, methods, etc).
	 */
	class TIDE_API Object : public ReferenceCounted
	{
	public:
		Object(std::string type = "Object") : type(type) {}
		virtual ~Object() {}

	public:

		/**
		 * Set a property on this object to the given value. 
		 * When an error occurs will throw an exception of type ValueException.
		 * @param name The property name
		 * @param value The new property value
		 */
		virtual void Set(const char *name, ValueRef value) = 0;

		/**
		 * @param name The property name
		 * @return the value of the property with the given name or Value::Undefined
		 *         if the property is not found.
		 * Errors will result in a thrown ValueException
		 */
		virtual ValueRef Get(const char *name) = 0;

		/**
		 * Determine if two objects have *reference* equality
		 * @param other The object to compare to
		 * @return true if these two objects are equal, false otherwise.
		 */
		virtual bool Equals(KObjectRef other);

		/**
		 * Determine if this objects has the property with the given name.
		 * @param name the name of the property to look for
		 * @return true if the property exists and false otherwise
		 */
		virtual bool HasProperty(const char* name);

		/**
		 * @return a list of this object's property names.
		 */
		virtual SharedStringList GetPropertyNames() = 0;

		/**
		 * @param levels The number of levels of children to display in this string (default: 3)
		 * @return a string representation of this object
		 */
		virtual SharedString DisplayString(int levels=1);

		/**
	     * If this object uses a large amount of extra memory, this method
	     * allows reporting this extra cost to the GC. This will help avoid
	     * leaking memory due to a lazy GC not running to collect the loose reference.
	     */
		virtual size_t ExtraMemoryCost() { return 0; }

		/**
		 * @param name The property name
		 * @param value The new property value
		 * Helpful overload to Set which takes a SharedString
		 */
		void Set(SharedString name, ValueRef value);

		/**
		 * @see Object::Get(const char*)
		 * @param name The property name
		 * Helpful overload to Get which takes a SharedString
		 */
		ValueRef Get(SharedString name);

		/**
		 * Get an int property from this object.
		 *
		 * @param name The name of the property to get
		 * @param defaultValue A value to return on failure
		 *
		 * @return Value of given property name, or the default value if
		 * if it does not exist or is not an int.
		 */
		int GetInt(const char *name, int defaultValue=0);

		/**
		 * Get a double property from this object.
		 *
		 * @param name The name of the property to get
		 * @param defaultValue A value to return on failure
		 *
		 * @return Value of given property name, or the default value if
		 * if it does not exist or is not a double.
		 */
		double GetDouble(const char *name, double defaultValue=0.0);

		/**
		 * Get a number property from this object.
		 *
		 * @param name The name of the property to get
		 * @param defaultValue A value to return on failure
		 *
		 * @return Value of given property name, or the default value if
		 * if it does not exist or is not a number.
		 */
		double GetNumber(const char *name, double defaultValue=0.0);

		/**
		 * Get a boolean property from this object.
		 *
		 * @param name The name of the property to get
		 * @param defaultValue A value to return on failure
		 *
		 * @return Value of given property name, or the default value if
		 * if it does not exist or is not a Bool.
		 */
		bool GetBool(const char *name, bool defaultValue=false);

		/**
		 * Get a string property from this object.
		 *
		 * @param name The name of the property to get
		 * @param defaultValue A value to return on failure
		 *
		 * @return Value of given property name, or the default value if
		 * if it does not exist or is not a string.
		 */
		std::string GetString(const char *name, std::string defaultValue="");

		/**
		 * Get an object property from this object.
		 *
		 * @param name The name of the property to get
		 * @param defaultValue A value to return on failure
		 *
		 * @return Value of given property name, or the default value if
		 * if it does not exist or is not an object.
		 */
		KObjectRef GetObject(const char *name, KObjectRef defaultValue=NULL);

		/**
		 * Get a method property from this object.
		 *
		 * @param name The name of the property to get
		 * @param defaultValue A value to return on failure
		 *
		 * @return Value of given property name, or the default value if
		 * if it does not exist or is not a method.
		 */
		KMethodRef GetMethod(const char *name, KMethodRef defaultValue=NULL);

		/**
		 * Get a list property from this object.
		 *
		 * @param name The name of the property to get
		 * @param defaultValue A value to return on failure
		 *
		 * @return Value of given property name, or the default value if
		 * if it does not exist or is not a list.
		 */
		KListRef GetList(const char *name, KListRef defaultValue=NULL);

		/**
		 * Set an undefined property on this object
		 * When an error occurs will throw an exception of type ValueException.
		 */
		void SetUndefined(const char *name);

		/**
		 * Set a null property on this object
		 * When an error occurs will throw an exception of type ValueException.
		 */
		void SetNull(const char *name);

		/**
		 * Set an int property on this object
		 * When an error occurs will throw an exception of type ValueException.
		 */
		void SetInt(const char *name, int);

		/**
		 * Set a double property on this object
		 * When an error occurs will throw an exception of type ValueException.
		 */
		void SetDouble(const char *name, double);

		/**
		 * Set a number property on this object
		 * When an error occurs will throw an exception of type ValueException.
		 */
		void SetNumber(const char *name, double);

		/**
		 * Set a bool property on this object
		 * When an error occurs will throw an exception of type ValueException.
		 */
		void SetBool(const char *name, bool);

		/**
		 * Set a string property on this object
		 * When an error occurs will throw an exception of type ValueException.
		 */
		void SetString(const char *name, std::string);

		/**
		 * Set an object property on this object
		 * When an error occurs will throw an exception of type ValueException.
		 */
		void SetObject(const char *name, KObjectRef);

		/**
		 * Set a method property on this object
		 * When an error occurs will throw an exception of type ValueException.
		 */
		void SetMethod(const char *name, KMethodRef);

		/**
		 * Set a list property on this object
		 * When an error occurs will throw an exception of type ValueException.
		 */
		void SetList(const char *name, KListRef);

		/**
		 * Get a list of strings for the given property of this object. The list
		 * is appended to the end of the passed-in list.
		 *
		 * @param name the name of the property
		 * @param list the vector where the list of strings is appended to
		 */
		void GetStringList(const char *name, std::vector<std::string> &list);

		/**
		 * Set the value of a child of this object using a simple object notation
		 * For example:
		 * \code
		 * this->SetNS("object.property.subproperty", value);
		 * // instead of
		 * this->Get("object")->ToObject()->Get("property")->ToObject()->Get("subproperty")->ToObject()->SetValue(value);
		 * \endcode
		 *
		 * This function does nothing if the object or it's parents are undefined
		 */
		void SetNS(const char *name, ValueRef value);

		/**
		 * Get the value of a child of this object using a simple object notation
		 * For example:
		 * \code
		 * ValueRef value = this->GetNS("object.property.subproperty");
		 * // instead of
		 * ValueRef value = this->Get("object")->ToObject()->Get("property")->ToObject()->Get("subproperty");
		 * \endcode
		 *
		 * @return The value of the child object, or Value::Undefined if the object, or it's parents are undefined
		 */
		ValueRef GetNS(const char *name);

		/**
		 * Call a child method on this object using simple object notation
		 * For example:
		 * \code
		 * this->CallNS("object.property.method", value1, value2);
		 * // instead of
		 * ValueList args;
		 * args.push_back(value1);
		 * args.push_back(value2);
		 * this->Get("object")->ToObject()->Get("property")->ToObject()->Get("method")->ToMethod()->Call(args);
		 * \endcode
		 * CallNS is overridden, and can accept up to 3 arguments inline, or a ValueList
		 * @return The return value of the function, or Value::Undefined if the object or method in the string is undefined
		 */
		ValueRef CallNS(const char *name);
		
		/**
		 * @see Object::CallNS(const char *name);
		 */
		ValueRef CallNS(const char *name, ValueRef val1);

		/**
		 * @see Object::CallNS(const char *name);
		 */
		ValueRef CallNS(const char *name, ValueRef val1, ValueRef val2);

		/**
		 * @see Object::CallNS(const char *name);
		 */
		ValueRef CallNS(const char *name, ValueRef val1, ValueRef val2, ValueRef val3);

		/**
		 * @see Object::CallNS(const char *name);
		 */
		ValueRef CallNS(const char *name, const ValueList& args);

		/**
		 * Return the type of this object as a string.
		 */
		virtual std::string& GetType();

		/**
		 * Return the unwrapped version of this object
		 */
		static KObjectRef Unwrap(KObjectRef);

		/**
		 * If this object is already exposed as an AutoPtr, this method
		 * returns a shared version of this object
		 */
		AutoPtr<Object> GetAutoPtr()
		{
			return AutoPtr<Object>(this, true);
		}

	protected:
		std::string type;

		Logger* logger()
		{
			return Logger::Get(this->type);
		}

	private:
		DISALLOW_EVIL_CONSTRUCTORS(Object);
	};

}

#endif
