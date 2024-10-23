#pragma once
#include <opendaq/function_block_ptr.h>

#include <vector>
#include <iostream>

#include "OpenDaqObject.h"
#include "../ErrorCodes.h"

BEGIN_NAMESPACE_OPENDAQ

class AppChannel;

class AppFunctionBlock : public OpenDaqObjectStaticImpl<OpenDaqObject, AppFunctionBlock, FunctionBlockPtr>
{
public:
    static bool processCommand(OpenDaqObject& obj, const std::vector<std::string>& command);

private:
    static int              list(const FunctionBlockPtr& fb, const string_view item);
    static OpenDaqObjectPtr select(const FunctionBlockPtr& fb, const string_view item, uint64_t index);
    static int              print(const FunctionBlockPtr& fb, const string_view item);
    static void             help();

    static int set(const BaseObjectPtr&, const string_view, const string_view)
    {
        std::cout << "Unsupported Method Asdf::set\n";
        return EC_METHOD_NOT_IMPLEMENTED;
    }

    static int getCount(const FunctionBlockPtr& fb, const string_view item);

    friend OpenDaqObjectStaticImpl<OpenDaqObject, AppFunctionBlock, FunctionBlockPtr>;
    friend AppChannel;
};

END_NAMESPACE_OPENDAQ
