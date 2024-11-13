#pragma once
#include <opendaq/signal_ptr.h>
#include <opendaq/data_descriptor_ptr.h>
#include <opendaq/stream_reader_ptr.h>
#include <opendaq/time_reader.h>

#include <string_view>
#include <iostream>
#include <type_traits>

#include "OpenDaqObject.h"

#include "../ErrorCodes.h"
#include "stdout_redirect.h"

#include <chrono>

struct SampleData
{
    using time_point = std::chrono::system_clock::time_point;
    using time_int   = std::chrono::system_clock::time_point::duration::rep;
    using data_type  = double;

    static_assert(sizeof(time_int) == 8, "Time size has to be 64 bits");

    static constexpr inline size_t uninitialized = -1;

    std::vector<data_type>  readings;
    std::vector<time_point> timestamps;
    uint64_t                readCount = uninitialized;

    double*     GetData()       { return readings.size() > 0    ? readings.data()   : nullptr;}
    time_point* GetTimestamps() { return timestamps.size() > 0  ? timestamps.data() : nullptr;}

    void Resize(size_t N)   { readings.resize(N); timestamps.resize(N); readCount = +N;}

    void Erase()            { readings.clear();   timestamps.clear();   readCount = -1;}
};

BEGIN_NAMESPACE_OPENDAQ

class AppDescriptor;

class AppSignal : public OpenDaqObjectStaticImpl<OpenDaqObject, AppSignal, SignalPtr>
{
public:
    static bool             processCommand(OpenDaqObject& output, const std::vector<std::string>& command);
    /*
    virtual void             help    () override;
    virtual int              print   (const string_view item) override;
    virtual int              list    (const string_view itemArray) override;
    virtual int              set     (const string_view item, const string_view value) override;
    virtual OpenDaqObjectPtr select  (const string_view item, uint64_t index) override;
    */


    virtual int Read(size_t NumOfSamples, int timeout);

    virtual void SendDataPacket(double* data, size_t count);
    virtual void SendTestDataPacket(size_t count, double sine_range);

    virtual int LoadDataDescriptorFromJson(const string_view json);

    SampleData samples{};
private:
    static void help();
    static int print(const SignalPtr& signal, const string_view item);
    static int list(const SignalPtr& signal, const string_view item);
    static int set(const SignalPtr& signal, const string_view item, const string_view value);
    static OpenDaqObjectPtr select(const SignalPtr& signal, const string_view item, uint64_t index);

    static int getCount(const SignalPtr& signal, const string_view item);


    static void printDataDescriptor(const DataDescriptorPtr& descriptor, std::streamsize indent, int indentLevel);
    static void printDimensions(const ListPtr<IDimension>& dimensions, std::streamsize indent, int indentLevel);
    static void printDataRule(const DataRulePtr& rule, std::streamsize indent, int indentLevel);
    static void printDimensionRule(const DimensionRulePtr& rule, std::streamsize indent, int indentLevel);
    static void printUnit(const UnitPtr& unit, std::streamsize indent, int indentLevel);
    static void printScaling(const ScalingPtr& scaling, std::streamsize indent, int indentLevel);
    static void printTags(const TagsPtr& tags, std::streamsize indent, int indentLevel);
    static void printMetadata(const DictPtr<IString, IString>& metadata, std::streamsize indent, int indentLevel);

    friend AppDescriptor;
    friend OpenDaqObjectStaticImpl<OpenDaqObject, AppSignal, SignalPtr>;

    StreamReaderPtr reader = nullptr;
};

END_NAMESPACE_OPENDAQ
