#pragma once
#include <opendaq/sync_component.h>
#include <opendaq/sync_component_factory.h>
#include <opendaq/sync_component_ptr.h>

#include <iostream>

#include "OpenDaqObject.h"
#include "../ErrorCodes.h"

BEGIN_NAMESPACE_OPENDAQ

class AppSync : public OpenDaqObjectStaticImpl<OpenDaqObject, AppSync, SyncComponentPtr>
{
public:
    static bool processCommand(OpenDaqObject& channel, const std::vector<std::string>& command);

    virtual ::String get(const string_view item) override
    {
        return AppSync::get(this->object, item);
    }

private:
    static int              print   (const SyncComponentPtr& sync, const string_view item);
    static void             help    ();

    static int              list    (const SyncComponentPtr& sync, const string_view item);
    static OpenDaqObjectPtr select  (const SyncComponentPtr& sync, const string_view item, uint64_t index);
    static int              set     (const SyncComponentPtr& sync, const string_view item, const string_view value);
    static int              getCount(const SyncComponentPtr& sync, const string_view item);
    static ::String         get     (const SyncComponentPtr& sync, const string_view item);

    friend OpenDaqObjectStaticImpl<OpenDaqObject, AppSync, SyncComponentPtr>;
};

END_NAMESPACE_OPENDAQ
