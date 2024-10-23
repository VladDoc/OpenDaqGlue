#include "OpenDaqObject.h"

#include <iostream>
#include <string_view>

#include "../ErrorCodes.h"

int            OpenDaqObject::print(const string_view item)
{
    std::cout << "Method is not supported\n";
    std::cout << object.toString();
    return EC_METHOD_NOT_IMPLEMENTED;
}
int            OpenDaqObject::list(const string_view item_type)
{
    std::cout << "Method is not supported\n";
    return EC_METHOD_NOT_IMPLEMENTED;
}
int            OpenDaqObject::set(const string_view item, const string_view value)
{
    std::cout << "Method is not supported\n";
    return EC_METHOD_NOT_IMPLEMENTED;
}

OpenDaqObjectPtr  OpenDaqObject::select(const string_view item, uint64_t index)
{
    std::cout << "Method is not supported\n";
    return nullptr;
}

String          OpenDaqObject::get(const string_view item)
{
    std::cout << "Method is not supported\n";
    return nullptr;
}
int            OpenDaqObject::getCount(const string_view itemArray)
{
    std::cout << "Method is not supported\n";
    return EC_METHOD_NOT_IMPLEMENTED;
}
