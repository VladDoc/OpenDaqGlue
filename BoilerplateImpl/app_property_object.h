#pragma once
#include <coreobjects/property_object_ptr.h>

#include <iostream>

#include "OpenDaqObject.h"
#include "../ErrorCodes.h"


BEGIN_NAMESPACE_OPENDAQ

class AppDescriptor;
class AppFunctionBlock;
class AppDevice;
class AppChannel;
class AppSignal;
class AppInputPort;

class AppPropertyObject : public OpenDaqObjectStaticImpl<OpenDaqObject, AppPropertyObject, PropertyObjectPtr>
{
public:
    static bool processCommand(OpenDaqObject& propObj, const std::vector<std::string>& command);

    virtual ::String get(const string_view item) override
    {
        return AppPropertyObject::get(this->object, item);
    }

private:
    static int      print   (const PropertyObjectPtr& prop, const string_view item);
    static int      list    (const PropertyObjectPtr& prop, const string_view item);
    static int      set     (const PropertyObjectPtr& prop, const string_view item, const string_view value);
    static ::String get     (const PropertyObjectPtr& prop, const string_view item);

    static void     help();

    static OpenDaqObjectPtr select(const PropertyObjectPtr& prop, const string_view item, uint64_t index)
    {
        std::cout << "Unsupported Method AppPropertyObject::select\n";
        return nullptr;
    }
    static int getCount(const PropertyObjectPtr& prop, const string_view item)
    {
        if (item == "properties")
        {
            return prop.getVisibleProperties().getCount();
        }

        return EC_PROPERTY_DOESNT_EXIST;
    }

    static void printSingleInfo(const std::string& type, const std::string& info);
    static void printProperty(const PropertyPtr& info);
    static std::string coreTypeToString(CoreType type);

    friend AppSync;
    friend AppInputPort;
    friend AppChannel;
    friend AppSignal;
    friend AppFunctionBlock;
    friend AppDevice;
    friend AppDescriptor;
    friend OpenDaqObjectStaticImpl<OpenDaqObject, AppPropertyObject, PropertyObjectPtr>;
};

END_NAMESPACE_OPENDAQ
