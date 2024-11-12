#include "lib.h"

#include <iostream>
#include <fstream>
#include <unordered_set>

#include "ErrorCodes.h"
#include "BoilerplateImpl/stdout_redirect.h"
#include "BoilerplateImpl/OpenDaqObject.h"
#include "BoilerplateImpl/util.h"

#include "BoilerplateImpl/app_device.h"
#include "BoilerplateImpl/app_input_port.h"
#include "BoilerplateImpl/app_signal.h"
#include "BoilerplateImpl/app_descriptor.h"

#include "opendaq/opendaq.h"

#include <chrono>
#include <cstdio>

static char help[] = "Create an Instance via Instance_New to start";
static char library_info[] = "OpenDAQ Glue version 0.0.69";

void LibraryHelp()
{
	puts(help);
}

const char* LibraryInfo(void) {
	return library_info;
}

void          StdOut_PipeToString     (void)
{
	PipeToString();
}

const char*   StdOut_GetBufferString  (void)
{
	return GetBufferString().c_str();
}

void          StdOut_Default          (void)
{
	DefaultStdOut();
}

void          StdOut_EraseBuffer      (void)
{
	EraseBuffer();
}

// keeping all allocated objects so we can check raw pointers validity
std::unordered_set<OpenDaqObjectPtr> createdPtrs;

OpenDaqObject* g_instance = nullptr;

DaqObjectPtr    Instance_New                	(void)
{
	try {
		auto instance_temp = daq::Instance(MODULE_PATH);
		const auto& [iterator, _] =
			createdPtrs.emplace(Make_OpenDaqObjectPtr<daq::AppDevice>(instance_temp));

		return g_instance = iterator->get();
	} catch(...) {
		return nullptr;
	}
}

void      OpenDaqObject_Free               	(DaqObjectPtr ptr)
{
	erase_uptr(createdPtrs, (OpenDaqObject*)ptr);
}

int          OpenDaqObject_List    (DaqObjectPtr self, const char* type)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return EC_INVALID_POINTER;

	try {
		return obj->list(type);
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}

static std::unordered_set<std::string> string_pool;


void         StringPool_Free       (const char* str)
{
	auto it = string_pool.find(str);

	if(it != string_pool.end())
		string_pool.erase(it);
}

const char*  OpenDaqObject_Get     (DaqObjectPtr self, const char* item)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return nullptr;

	try {
		const auto& [it, inserted] =
			string_pool.emplace(obj->get(item));
		return it->c_str();
	} catch(...) {
		return nullptr;
	}
}

int          OpenDaqObject_GetCount(DaqObjectPtr self, const char* itemArray)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return EC_INVALID_POINTER;

	try {
		return obj->getCount(itemArray);
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}

int          OpenDaqObject_Set     (DaqObjectPtr self, const char* item, const char* value)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return EC_INVALID_POINTER;

	try {
		return obj->set(item, value);
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}

DaqObjectPtr OpenDaqObject_Select  (DaqObjectPtr self, const char* type, uint64 index)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return nullptr;

	try {
		const auto& [it, inserted] =
			createdPtrs.emplace(obj->select(type, index));
		return it->get();
	} catch(...) {
		return nullptr;
	}
}

void         OpenDaqObject_Help    (DaqObjectPtr self)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj) || obj == nullptr)
		obj = g_instance;

	if(obj) obj->help();
	else {
		std::cout << help << std::endl;
	}
}

const char* Device_GetAvailableDeviceConnectionString(DaqObjectPtr self, uint64 index)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return nullptr;

	try {
		auto device = dynamic_cast<daq::AppDevice*>(obj);

		if(device) {
			const auto& [it, inserted] =
				string_pool.emplace(device->GetAvailableDeviceConnectionString(index));
			return it->c_str();
		}
		return nullptr;
	} catch(...) {
		return nullptr;
	}
}

const char* Device_GetAvailableFunctionBlockID(DaqObjectPtr self, uint64 index)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return nullptr;

	try {
		auto device = dynamic_cast<daq::AppDevice*>(obj);

		if(device) {
			const auto& [it, inserted] =
				string_pool.emplace(device->GetAvailableFunctionBlockID(index));
			return it->c_str();
		}
		return nullptr;
	} catch(...) {
		return nullptr;
	}
}

// Device specific methods. Work also for instance
DaqObjectPtr Device_Add              (DaqObjectPtr self, const char* type, const char* value)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return nullptr;

	try {
		auto device = dynamic_cast<daq::AppDevice*>(obj);
		if(device) {
			const auto& [it, inserted] =
				createdPtrs.emplace(device->Add(type, value));
			return it->get();
		}
		return nullptr;
	} catch(...) {
		return nullptr;
	}
}

DaqObjectPtr Device_AddDevice        (DaqObjectPtr device, const char* connectionString)
{
	return Device_Add(device, "device", connectionString);
}

DaqObjectPtr Device_AddFunctionBlock (DaqObjectPtr device, const char* fbId)
{
	return Device_Add(device, "function-block", fbId);
}

int          Device_Remove           (DaqObjectPtr self, const char* type, uint64 index)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return EC_INVALID_POINTER;

	try {
		auto device = dynamic_cast<daq::AppDevice*>(obj);
		if(device) {
			OpenDaqObject_Free(obj->select(type, index).get());

			return device->Remove(type, index);
		}
		return EC_OBJECT_TYPE_MISMATCH;
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}

int          Device_RemoveDevice     (DaqObjectPtr device, uint64 index)
{
	return Device_Remove(device, "device", index);
}

int          Device_RemoveFunctionBlock(DaqObjectPtr device, uint64 index)
{
	return Device_Remove(device, "function-block", index);
}

int          Device_LoadConfiguration(DaqObjectPtr self, const char* json)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return EC_INVALID_POINTER;

	try {
		auto device = dynamic_cast<daq::AppDevice*>(obj);
		if(device) {
			return device->LoadConfiguration(json);
		}
		return EC_OBJECT_TYPE_MISMATCH;
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}
int          Device_LoadConfigurationFromFile(DaqObjectPtr device, const char* json_path)
{
	try {
		auto str = read_file(json_path);

		return Device_LoadConfiguration(device, str.c_str());
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}

const char*  Device_SaveConfiguration(DaqObjectPtr self)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return nullptr;

	try {
		auto device = dynamic_cast<daq::AppDevice*>(obj);
		if(device) {
			const auto& [it, inserted] =
				string_pool.emplace(device->SaveConfiguration());
			return it->c_str();
		}
		return nullptr;
	} catch(...) {
		return nullptr;
	}
}
int          Device_SaveConfigurationToFile(DaqObjectPtr device, const char* json_path)
{
	auto obj = (OpenDaqObject*)device;

	if(!contains_uptr(createdPtrs, obj))
		return EC_INVALID_POINTER;

	if(!dynamic_cast<daq::AppDevice*>(obj))
		return EC_OBJECT_TYPE_MISMATCH;

	auto str = Device_SaveConfiguration(device);

	if(str) {
		std::ofstream out(json_path);
		out << str;

		return out.good() ? EC_OK : EC_IO_ERROR;
	}

	return EC_GENERIC_ERROR;
}

int          InputPort_Connect(DaqObjectPtr self, const char* signalid, DaqObjectPtr instance_device)
{
	auto obj = (OpenDaqObject*)self;
	auto obj2 = (OpenDaqObject*)instance_device;
	if(!contains_uptr(createdPtrs, obj) 	||
	   !contains_uptr(createdPtrs, obj2))
		return EC_INVALID_POINTER;

	try {
		auto port = dynamic_cast<daq::AppInputPort*>(obj);
		auto instance = dynamic_cast<daq::AppDevice*>(obj2);
		if(port) {
			return port->connect(signalid, instance->object);
		}
		return EC_OBJECT_TYPE_MISMATCH;
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}

int          InputPort_Disconnect(DaqObjectPtr self)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return EC_INVALID_POINTER;

	try {
		auto port = dynamic_cast<daq::AppInputPort*>(obj);
		if(port) {
			return port->disconnect();
		}
		return EC_OBJECT_TYPE_MISMATCH;
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}


int          Signal_Read(DaqObjectPtr self, uint64 NumOfSamples, int timeout)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return EC_INVALID_POINTER;

	try {
		auto signal = dynamic_cast<daq::AppSignal*>(obj);
		if(signal) {
			return signal->Read(NumOfSamples, timeout);
		}
		return EC_OBJECT_TYPE_MISMATCH;
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}


double*      Signal_GetSampleReadings(DaqObjectPtr self)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return nullptr;

	try {
		auto signal = dynamic_cast<daq::AppSignal*>(obj);
		if(signal) {
			if(signal->samples.readCount == SampleData::uninitialized)
				return nullptr;

			return signal->samples.GetData();
		}
		return nullptr;
	} catch(...) {
		return nullptr;
	}
}

int64*       Signal_GetSampleTimeStamps(DaqObjectPtr self)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return nullptr;

	try {
		auto signal = dynamic_cast<daq::AppSignal*>(obj);
		if(signal) {
			if(signal->samples.readCount == SampleData::uninitialized)
				return nullptr;

			return (int64*)signal->samples.GetTimestamps();
		}
		return nullptr;
	} catch(...) {
		return nullptr;
	}
}

int          Signal_GetSampleCountOfRead(DaqObjectPtr self)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return EC_INVALID_POINTER;

	try {
		auto signal = dynamic_cast<daq::AppSignal*>(obj);
		if(signal) {
			auto readCount = signal->samples.readCount;

			return
				readCount == SampleData::uninitialized ?
					EC_UNINITIALIZED 				   :
					readCount;
		}
		return EC_OBJECT_TYPE_MISMATCH;
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}

int          Signal_EraseSamples(DaqObjectPtr self)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return EC_INVALID_POINTER;

	try {
		auto signal = dynamic_cast<daq::AppSignal*>(obj);
		if(signal) {
			return (signal->samples.Erase(), EC_OK);
		}
		return EC_OBJECT_TYPE_MISMATCH;
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}

int          Signal_GetSampleReadingsToArray(DaqObjectPtr self, double* array, uint64 len)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return EC_INVALID_POINTER;

	try {
		auto signal = dynamic_cast<daq::AppSignal*>(obj);
		if(signal) {
			if(signal->samples.readCount == SampleData::uninitialized)
				return EC_UNINITIALIZED;

			if(len > signal->samples.readCount)
				len = signal->samples.readCount;

			memcpy(array, signal->samples.GetData(), len * sizeof(*array));

			return len;
		}
		return EC_OBJECT_TYPE_MISMATCH;
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}

int          Signal_GetSampleTimeStampsToArray(DaqObjectPtr self, int64* array, uint64 len)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return EC_INVALID_POINTER;

	try {
		auto signal = dynamic_cast<daq::AppSignal*>(obj);
		if(signal) {
			if(signal->samples.readCount == SampleData::uninitialized)
				return EC_UNINITIALIZED;

			if(len > signal->samples.readCount)
				len = signal->samples.readCount;

			memcpy(array, signal->samples.GetTimestamps(), len * sizeof(*array));

			return len;
		}
		return EC_OBJECT_TYPE_MISMATCH;
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}

const char*  TimeStampToString(int64 timestamp)
{
	std::chrono::system_clock::time_point tp;
	static_assert(sizeof(timestamp) == sizeof(tp));

#pragma warning(suppress:0000)
#pragma GCC diagnostic ignored "-Wclass-memaccess"
	memcpy(&tp, &timestamp, sizeof(tp));

	auto t = std::chrono::system_clock::to_time_t(tp);
	auto datetime_se = tp.time_since_epoch();
	auto datetime_se_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(datetime_se);
	auto hundreds_ns = (datetime_se_ns % 1000000000)/100;  // 1000000000 - ns, std::chrono::system_clock 100 ns
	auto* tm = gmtime(&t);

	//static const char format[] = "yyyy-mm-dd hh:mm:ss.nanoseconds";
	static char timeString[256];
	std::strftime(std::data(timeString), std::size(timeString), "%F %T", tm);

	static char secondFraction[16] = "00000000";
	snprintf(secondFraction, std::size(secondFraction), ".%ld", hundreds_ns.count());
	strcat(timeString, secondFraction);

	return timeString;
}

int          Signal_SendDataPacket(DaqObjectPtr self, double* data, uint64 count)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return EC_INVALID_POINTER;

	try {
		auto signal = dynamic_cast<daq::AppSignal*>(obj);
		if(signal) {
			return (signal->SendDataPacket(data, count), EC_OK);
		}
		return EC_OBJECT_TYPE_MISMATCH;
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}

int          Signal_SendTestDataPacket(DaqObjectPtr self, uint64 count, double sine_range)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return EC_INVALID_POINTER;

	try {
		auto signal = dynamic_cast<daq::AppSignal*>(obj);
		if(signal) {
			return (signal->SendTestDataPacket(count, sine_range), EC_OK);
		}
		return EC_OBJECT_TYPE_MISMATCH;
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}

int          OpenDaqObject_Print   (DaqObjectPtr self, const char* item)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return EC_INVALID_POINTER;

	try {
		if((obj = dynamic_cast<OpenDaqObject*>(obj)) != nullptr) {
			return obj->print(item);
		}
		return EC_OBJECT_TYPE_MISMATCH;
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}

EXPORTFUN int          Signal_LoadDataDescriptorFromJson(DaqObjectPtr self, const char* json)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return EC_INVALID_POINTER;

	try {
		auto signal = dynamic_cast<daq::AppSignal*>(obj);
		if(signal) {
			return signal->LoadDataDescriptorFromJson(json);
		}
		return EC_OBJECT_TYPE_MISMATCH;
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}
EXPORTFUN int          Signal_LoadDataDescriptorFromJsonFile(DaqObjectPtr signal, const char* path)
{
	try {
		auto str = read_file(path);

		return Signal_LoadDataDescriptorFromJson(signal, str.c_str());
	} catch(...) {
		return EC_GENERIC_ERROR;
	}
}

EXPORTFUN const char*  DataDescriptor_SaveToJson(DaqObjectPtr self)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return nullptr;

	try {
		auto desc = dynamic_cast<daq::AppDescriptor*>(obj);
		if(desc) {
			const auto& [it, inserted] =
				string_pool.emplace(desc->SaveConfiguration());
			return it->c_str();
		}
		return nullptr;
	} catch(...) {
		return nullptr;
	}
}
EXPORTFUN int          DataDescriptor_SaveToJsonFile(DaqObjectPtr self, const char* path)
{
	auto obj = (OpenDaqObject*)self;

	if(!contains_uptr(createdPtrs, obj))
		return EC_INVALID_POINTER;

	if(!dynamic_cast<daq::AppDescriptor*>(obj))
		return EC_OBJECT_TYPE_MISMATCH;

	auto str = DataDescriptor_SaveToJson(self);

	if(str) {
		std::ofstream out(path);
		out << str;

		return out.good() ? EC_OK : EC_IO_ERROR;
	}

	return EC_GENERIC_ERROR;
}

