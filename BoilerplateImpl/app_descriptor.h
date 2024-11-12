#pragma once
#include <opendaq/data_descriptor_ptr.h>

#include <iostream>

#include "OpenDaqObject.h"
#include "../ErrorCodes.h"

BEGIN_NAMESPACE_OPENDAQ

class AppDescriptor : public OpenDaqObjectStaticImpl<OpenDaqObject, AppDescriptor, DataDescriptorPtr>
{
public:
    static bool processCommand(OpenDaqObject& channel, const std::vector<std::string>& command);

    virtual ::String SaveConfiguration();

private:
    static int              print   (const DataDescriptorPtr& desc, const string_view item);
    static void             help    ();

    static int              list    (const DataDescriptorPtr& desc, const string_view item);
    static OpenDaqObjectPtr select  (const DataDescriptorPtr& desc, const string_view item, uint64_t index);
    static int              set     (const DataDescriptorPtr& desc, const string_view item, const string_view value);
    static int              getCount(const DataDescriptorPtr& desc, const string_view item);

    friend OpenDaqObjectStaticImpl<OpenDaqObject, AppDescriptor, DataDescriptorPtr>;
};

END_NAMESPACE_OPENDAQ
