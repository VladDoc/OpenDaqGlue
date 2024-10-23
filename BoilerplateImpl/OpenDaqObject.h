#pragma once

#include <coretypes/objectptr.h>
#include <coretypes/baseobject_factory.h>

#include <cstdint>
#include <string_view>
#include <memory>
#include <string>

#include "stdout_redirect.h"

using std::string_view;
using daq::BaseObjectPtr;
using daq::ObjectPtr;

class OpenDaqObject;

using OpenDaqObjectPtr = std::unique_ptr<OpenDaqObject>;
using String = std::string;

class OpenDaqObject
{
public:
    OpenDaqObject() {}
    OpenDaqObject(std::nullptr_t) {}
    OpenDaqObject(BaseObjectPtr ptr) : object(ptr) {}
    OpenDaqObject(const OpenDaqObject& other) : object(other.object) {}
    template <typename T>
    OpenDaqObject(const ObjectPtr< T > &objPtr) : object(objPtr.getObject()) {}
    OpenDaqObject& operator=(const OpenDaqObject& other)
    {
        object = other.object;
        return *this;
    }

    virtual ~OpenDaqObject() {}

    virtual int              print   (const string_view item);
    virtual int              list    (const string_view itemArray);
    virtual int              set     (const string_view item, const string_view value);
    virtual OpenDaqObjectPtr select  (const string_view item, uint64_t index);
    virtual String           get     (const string_view item);
    virtual int              getCount(const string_view itemArray);
    virtual void             help    (int i = 0) = 0;

    BaseObjectPtr object = nullptr;
    BaseObjectPtr& self = object;
};

template <typename T>
auto Make_OpenDaqObjectPtr(BaseObjectPtr&& from)
{
    OpenDaqObjectPtr ptr {new T};
    ptr->object = from;
    return ptr;
}

template <typename Base, typename Derived, typename OpenDaqInterface>
class OpenDaqObjectStaticImpl : public Base
{
public:
    using Base::Base;

    virtual int              print   (const string_view item)
    {
        return Derived::print(this->object, item);
    }
    virtual int              list    (const string_view itemArray)
    {
        return Derived::list(this->object, itemArray);
    }
    virtual int              set     (const string_view item, const string_view value)
    {
        return Derived::set(this->object, item, value);
    }
    virtual OpenDaqObjectPtr select  (const string_view item, uint64_t index)
    {
        return Derived::select(this->object, item, index);
    }
    // TODO: rewrite so that user's StdOut Pipe won't be messed with
    virtual String           get     (const string_view item)
    {
        bool piped = IsPipedToString();
        String oldbuf = piped ? GetBufferString() : "";
        EraseBuffer();
        if(!piped) PipeToString();

        String result = "";

        int errorcode = Derived::print(this->object, item);
        if(errorcode >= 0) {
            result = GetBufferString();

            auto it = result.find(": ");
            if(it != String::npos)
                result = result.substr(it + 2, String::npos);
        }

        if(!piped) DefaultStdOut();
        SetStringBuffer(oldbuf);
        //return Derived::get(this->object, item);
        return result;
    }
    virtual int              getCount(const string_view itemArray)
    {
        return Derived::getCount(this->object, itemArray);
    }
    virtual void             help    (int)
    {
        return Derived::help();
    }
};

/*  Placeholders
 *
 *
 *  static int   list(const BaseObjectPtr&, const string_view)
    {
        std::cout << "Unsupported Method Asdf::list\n";
        return EC_METHOD_NOT_IMPLEMENTED;
    }
    static int set(const BaseObjectPtr&, const string_view, const string_view)
    {
        std::cout << "Unsupported Method Asdf::set\n";
        return EC_METHOD_NOT_IMPLEMENTED;
    }
    static OpenDaqObjectPtr select(const BaseObjectPtr&, const string_view, uint64_t)
    {
        std::cout << "Unsupported Method Asdf::select\n";
        return nullptr;
    }
    static String   get(const BaseObjectPtr&, const string_view)
    {
        std::cout << "Unsupported Method Asdf::get\n";
        return "";
    }
    static int   getCount(const BaseObjectPtr&, const string_view)
    {
        std::cout << "Unsupported Method Asdf::getCount\n";
        return EC_METHOD_NOT_IMPLEMENTED;
    }
*/
