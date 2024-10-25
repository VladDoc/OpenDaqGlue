#ifdef _WIN32
#include "dlfcn-win32/src/dlfcn.h"
const char* modulename = "opendaq-bridge.dll";
#else
#include <dlfcn.h>
const char* modulename = "./libopendaq-bridge.so";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include "ColoredPrinter.h"
#include "ErrorCodes.h"

#define UGLYCAST(x) (*(void **) (&x))

#define GETFUN(x, handle) (UGLYCAST(x) = dlsym(handle, #x))

typedef void* DaqObjectPtr;

char*		  (*talk)					 (void);
void 		  (*pukToStream) 			 (void);
void          (*StdOut_PipeToString)     (void);
const char*   (*StdOut_GetBufferString)  (void);
void          (*StdOut_Default)          (void);
void          (*StdOut_EraseBuffer)      (void);

DaqObjectPtr (*Instance_New)         	 (void);
void         (*OpenDaqObject_Free)       (DaqObjectPtr self);
void         (*StringPool_Free)       	 (const char* str);

int          (*OpenDaqObject_List)    (DaqObjectPtr self, const char* type);
const char*  (*OpenDaqObject_Get)     (DaqObjectPtr self, const char* item);
int          (*OpenDaqObject_GetCount)(DaqObjectPtr self, const char* itemArray);
int          (*OpenDaqObject_Set)     (DaqObjectPtr self, const char* item, const char* value);
DaqObjectPtr (*OpenDaqObject_Select)  (DaqObjectPtr self, const char* type, uint64_t index);
void         (*OpenDaqObject_Help)    (DaqObjectPtr self);

DaqObjectPtr (*Device_Add)              (DaqObjectPtr device, const char* type, const char* value);
DaqObjectPtr (*Device_AddDevice)        (DaqObjectPtr device, const char* connectionString);
DaqObjectPtr (*Device_AddFunctionBlock) (DaqObjectPtr device, const char* fbId);
int          (*Device_Remove)           (DaqObjectPtr device, const char* type, uint64_t index);
int          (*Device_RemoveDevice)     (DaqObjectPtr device, uint64_t index);
int          (*Device_RemoveFunctionBlock)(DaqObjectPtr device, uint64_t index);

const char*  (*Device_GetAvailableDeviceConnectionString)(DaqObjectPtr device, uint64_t index);
const char*  (*Device_GetAvailableFunctionBlockID)(DaqObjectPtr device, uint64_t index);

int          (*Device_LoadConfiguration)(DaqObjectPtr device, const char* json);
int          (*Device_LoadConfigurationFromFile)(DaqObjectPtr device, const char* json_path);
const char*  (*Device_SaveConfiguration)(DaqObjectPtr device);
int          (*Device_SaveConfigurationToFile)(DaqObjectPtr device, const char* json_path);

int          (*InputPort_Connect)(DaqObjectPtr port, const char* signalid, DaqObjectPtr instance);
int          (*InputPort_Disconnect)(DaqObjectPtr port);

int          (*Signal_Read)(DaqObjectPtr signal, uint64_t NumOfSamples, int timeout);

double*      (*Signal_GetSampleReadings)(DaqObjectPtr signal);
int64_t*     (*Signal_GetSampleTimeStamps)(DaqObjectPtr signal);

int          (*Signal_GetSampleReadingsToArray)(DaqObjectPtr signal, double* array, uint64_t len);
int          (*Signal_GetSampleTimeStampsToArray)(DaqObjectPtr signal, int64_t* array, uint64_t len);

int          (*Signal_GetSampleCountOfRead)(DaqObjectPtr signal);
int          (*Signal_EraseSamples)(DaqObjectPtr signal);
const char*  (*TimeStampToString)(int64_t timestamp);

int          (*Signal_SendDataPacket)(DaqObjectPtr signal, double* data, uint64_t count);
int          (*Signal_SendTestDataPacket)(DaqObjectPtr signal, uint64_t count, double sine_range);




static const char* test_config =
"{\n"
"    \"__type\": \"Instance\",\n"
"    \"rootDevice\": {\n"
"        \"58f4eae6-072b-49ab-9104-019fdc74a42f\": {\n"
"            \"__type\": \"Device\",\n"
"            \"name\": \"openDAQ Client\",\n"
"            \"Dev\": {\n"
"                \"__type\": \"Folder\",\n"
"                \"items\": {\n"
"                    \"miniaudiodev0\": {\n"
"                        \"__type\": \"Device\",\n"
"                        \"Sig\": {\n"
"                            \"__type\": \"Folder\",\n"
"                            \"items\": {\n"
"                                \"time\": {\n"
"                                    \"__type\": \"Signal\",\n"
"                                    \"dataDescriptor\": {\n"
"                                        \"__type\": \"DataDescriptor\",\n"
"                                        \"name\": \"Time\",\n"
"                                        \"sampleType\": 10,\n"
"                                        \"unit\": {\n"
"                                            \"__type\": \"Unit\",\n"
"                                            \"symbol\": \"s\",\n"
"                                            \"name\": \"second\",\n"
"                                            \"quantity\": \"time\"\n"
"                                        },\n"
"                                        \"dimensions\": [],\n"
"                                        \"rule\": {\n"
"                                            \"__type\": \"DataRule\",\n"
"                                            \"ruleType\": 1,\n"
"                                            \"params\": {\n"
"                                                \"__type\": \"Dict\",\n"
"                                                \"values\": [\n"
"                                                    {\n"
"                                                        \"key\": \"delta\",\n"
"                                                        \"value\": 1\n"
"                                                    },\n"
"                                                    {\n"
"                                                        \"key\": \"start\",\n"
"                                                        \"value\": 0\n"
"                                                    }\n"
"                                                ]\n"
"                                            }\n"
"                                        },\n"
"                                        \"origin\": \"\",\n"
"                                        \"tickResolution\": {\n"
"                                            \"__type\": \"Ratio\",\n"
"                                            \"num\": 1,\n"
"                                            \"den\": 44100\n"
"                                        },\n"
"                                        \"metadata\": {\n"
"                                            \"__type\": \"Dict\",\n"
"                                            \"values\": []\n"
"                                        },\n"
"                                        \"structFields\": []\n"
"                                    },\n"
"                                    \"public\": true,\n"
"                                    \"visible\": false\n"
"                                }\n"
"                            }\n"
"                        },\n"
"                        \"IO\": {\n"
"                            \"__type\": \"IoFolder\",\n"
"                            \"items\": {\n"
"                                \"audio\": {\n"
"                                    \"__type\": \"Channel\",\n"
"                                    \"typeId\": \"audio_channel\",\n"
"                                    \"Sig\": {\n"
"                                        \"__type\": \"Folder\",\n"
"                                        \"items\": {\n"
"                                            \"Audio\": {\n"
"                                                \"__type\": \"Signal\",\n"
"                                                \"domainSignalId\": \"/58f4eae6-072b-49ab-9104-019fdc74a42f/Dev/miniaudiodev0/Sig/time\",\n"
"                                                \"dataDescriptor\": {\n"
"                                                    \"__type\": \"DataDescriptor\",\n"
"                                                    \"name\": \"Monitor of Family 17h (Models 10h-1fh) HD Audio Controller Analog Stereo\",\n"
"                                                    \"sampleType\": 1,\n"
"                                                    \"dimensions\": [],\n"
"                                                    \"valueRange\": {\n"
"                                                        \"__type\": \"Range\",\n"
"                                                        \"low\": -2.0,\n"
"                                                        \"high\": 2.0\n"
"                                                    },\n"
"                                                    \"rule\": {\n"
"                                                        \"__type\": \"DataRule\",\n"
"                                                        \"ruleType\": 3,\n"
"                                                        \"params\": {\n"
"                                                            \"__type\": \"Dict\",\n"
"                                                            \"values\": []\n"
"                                                        }\n"
"                                                    },\n"
"                                                    \"origin\": \"\",\n"
"                                                    \"metadata\": {\n"
"                                                        \"__type\": \"Dict\",\n"
"                                                        \"values\": []\n"
"                                                    },\n"
"                                                    \"structFields\": []\n"
"                                                },\n"
"                                                \"public\": true\n"
"                                            }\n"
"                                        }\n"
"                                    }\n"
"                                }\n"
"                            }\n"
"                        }\n"
"                    }\n"
"                }\n"
"            }\n"
"        }\n"
"    }\n"
"}";


void InitFunctions(void* handle)
{

	GETFUN(talk, handle);
	GETFUN(pukToStream, handle);
	GETFUN(StdOut_Default, handle);
	GETFUN(StdOut_GetBufferString, handle);
	GETFUN(StdOut_PipeToString, handle);
	GETFUN(StdOut_EraseBuffer, handle);

	GETFUN(Instance_New, handle);
	GETFUN(OpenDaqObject_Free, handle);
	GETFUN(StringPool_Free, handle);

	GETFUN(OpenDaqObject_List, handle);
	GETFUN(OpenDaqObject_Get, handle);
	GETFUN(OpenDaqObject_GetCount, handle);
	GETFUN(OpenDaqObject_Set, handle);
	GETFUN(OpenDaqObject_Select, handle);
	GETFUN(OpenDaqObject_Help, handle);

	GETFUN(Device_Add, handle);
	GETFUN(Device_AddDevice, handle);
	GETFUN(Device_AddFunctionBlock, handle);
	GETFUN(Device_Remove, handle);
	GETFUN(Device_RemoveDevice, handle);
	GETFUN(Device_RemoveFunctionBlock, handle);

	GETFUN(Device_GetAvailableDeviceConnectionString, handle);
	GETFUN(Device_GetAvailableFunctionBlockID, handle);

	GETFUN(Device_LoadConfiguration, handle);
	GETFUN(Device_LoadConfigurationFromFile, handle);
	GETFUN(Device_SaveConfiguration, handle);
	GETFUN(Device_SaveConfigurationToFile, handle);

	GETFUN(InputPort_Connect, handle);
	GETFUN(InputPort_Disconnect, handle);

	GETFUN(Signal_Read, handle);
	GETFUN(Signal_GetSampleReadings, handle);
	GETFUN(Signal_GetSampleTimeStamps, handle);
	GETFUN(Signal_GetSampleReadingsToArray, handle);
	GETFUN(Signal_GetSampleTimeStampsToArray, handle);
	GETFUN(Signal_GetSampleCountOfRead, handle);
	GETFUN(Signal_EraseSamples, handle);

	GETFUN(Signal_SendDataPacket, handle);
	GETFUN(Signal_SendTestDataPacket, handle);

	GETFUN(TimeStampToString, handle);
}

void SaveToCSV(const char* path, double* values, int size)
{
	FILE* f = fopen(path, "w");
	if(!f) return;

	for(int i = 0; i < size-1; ++i) {
		fprintf(f, "%f,", values[i]);
	}

	fprintf(f, "%f\n", values[size-1]);
}


void Test_CheckStdOutRedirect()
{
	StdOut_PipeToString();
	pukToStream();
	const char* str = StdOut_GetBufferString();
	StdOut_Default();
	printf("%s", str);
	const char* str2 = talk(); // same text
	assert(strcmp(str, str2) == 0);

	StdOut_EraseBuffer();

	Success();
	puts("Test_CheckStdOutRedirect: Success\n");
	ResetColors();
}

void Test_CheckInstance()
{
	DaqObjectPtr instance = Instance_New();
	assert(instance);

	do {
		PrintInfo("Listing available devices: ");
		assert(OpenDaqObject_List(instance, "available-devices") == EC_OK);
	} while(0);

	do {
		PrintInfo("Listing properties: ");
		assert(OpenDaqObject_List(instance, "properties") == EC_OK);
	} while(0);

	do{
		PrintInfo("Help info: ");
		OpenDaqObject_Help(instance);
	}while(0);

	const char* connectionStringDev0 = NULL;

	do {
		PrintInfo("Getting available device connection string: ");

		int count = OpenDaqObject_GetCount(instance, "available-devices");
		printf("Total devices count: %d\n", count);
		int index = 0;
		if(count > 0)
			connectionStringDev0 = Device_GetAvailableDeviceConnectionString(instance, index);
		printf("Connection String for Device[%d]: %s\n", index, connectionStringDev0);
	} while(0);

	//connectionStringDev0 = "daqref://device0";

	DaqObjectPtr dev0 = NULL;
	do {
		PrintInfo("Adding Device:");

		dev0 = Device_AddDevice(instance, connectionStringDev0);
		assert(dev0);
		const char * name = OpenDaqObject_Get(dev0, "name");
		printf("Device Name: %s\n", name);

		StringPool_Free(name);
	} while(0);

	const char* config;
	do {
		PrintInfo("Reading Instance Config:");
		config = Device_SaveConfiguration(instance);
		assert(config);
		printf("%s\n", config);
	} while(0);

	do {
		//PrintInfo("Applying Instance Config:");
		//assert(Device_LoadConfiguration(instance, test_config) == EC_OK);

		//const char* config2 = Device_SaveConfiguration(instance);
		//printf("%s\n", config2);
		//assert(strcmp(config2, test_config) == 0);
		//StringPool_Free(config2);
	} while(0);

	StringPool_Free(config);

	DaqObjectPtr channel;
	DaqObjectPtr signal;

	int num = 1000;

	double data[1000] = {};
	int64_t times[1000] = {};

	do {
		PrintInfo("Reading Samples From Signal");

		channel = OpenDaqObject_Select(dev0, "channel", 0);
		assert(channel);

		signal = OpenDaqObject_Select(channel, "signal", 0);
		assert(signal);

		int current = 0;

		while(current < num) {
			int count = Signal_Read(signal, 20, 20);
			printf("Count: %d\n", count);
			//assert(count > 0);
			if(count == 0) continue;

			if(current + count > num)
				count = num - current;

			Signal_GetSampleReadingsToArray(signal, data+current, count);
			Signal_GetSampleTimeStampsToArray(signal, times+current, count);
			current += count;
		}

		assert(current == num);

		for(int i = 0; i < current; ++i) {
			const char* timestr = TimeStampToString(times[i]);

			printf("%f:%s\n", data[i], timestr);
		}

		SaveToCSV("output.csv", data, num);
	} while(0);


	do {
		PrintInfo("Sending Test Data to Signal");

		Signal_SendTestDataPacket(signal, num, 10.0);
	} while(0);

	do {
		PrintInfo("Sending Data to Signal");

		Signal_SendDataPacket(signal, data, num);
	} while(0);

	OpenDaqObject_Free(signal);
	OpenDaqObject_Free(channel);
	OpenDaqObject_Free(dev0);
	OpenDaqObject_Free(instance);

	Success();
	puts("Test_CheckInstance: Success\n");
	ResetColors();
}

void Test_Main()
{
	Test_CheckStdOutRedirect();
	Test_CheckInstance();
}

int main(void)
{
	char*	error;
	void*	handle;

	handle = dlopen(modulename, RTLD_LAZY);
	if (!handle) {
		fprintf(stderr, "%s\n", dlerror());
		exit(EXIT_FAILURE);
	}

	dlerror();/* Clear any existing error */

	InitFunctions(handle);

	Test_Main();

	error = dlerror();
	if (error != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(EXIT_FAILURE);
	}

	dlclose(handle);
	exit(EXIT_SUCCESS);
}
