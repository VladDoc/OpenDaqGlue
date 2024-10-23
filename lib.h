#pragma once

#if defined(_WIN32) || defined(__WIN32__)
#define  EXPORTFUN extern "C" __declspec(dllexport)
#else
#define EXPORTFUN extern "C"
#endif

EXPORTFUN void pukToStream();
EXPORTFUN char* talk(void);

typedef void* DaqObjectPtr;

typedef unsigned long long uint64;
typedef long long int64;

// Stdout manip
EXPORTFUN void         StdOut_PipeToString     (void);
EXPORTFUN const char*  StdOut_GetBufferString  (void);
EXPORTFUN void         StdOut_Default          (void);
EXPORTFUN void         StdOut_EraseBuffer      (void);

// free any string returned by this dll
EXPORTFUN void         StringPool_Free       (const char* str);

EXPORTFUN DaqObjectPtr Instance_New          (void);

// Polymorphic methods:
EXPORTFUN void         OpenDaqObject_Free    (DaqObjectPtr self);
EXPORTFUN int          OpenDaqObject_List    (DaqObjectPtr self, const char* type); // outputs to console
EXPORTFUN const char*  OpenDaqObject_Get     (DaqObjectPtr self, const char* item);
EXPORTFUN int          OpenDaqObject_GetCount(DaqObjectPtr self, const char* itemArray);
EXPORTFUN int          OpenDaqObject_Set     (DaqObjectPtr self, const char* item, const char* value);
EXPORTFUN DaqObjectPtr OpenDaqObject_Select  (DaqObjectPtr self, const char* type, uint64 index);
EXPORTFUN void         OpenDaqObject_Help    (DaqObjectPtr self); // nullptr will call help on instance
// Device specific methods. Work also for instance
EXPORTFUN DaqObjectPtr Device_Add              (DaqObjectPtr device, const char* type, const char* value);
EXPORTFUN DaqObjectPtr Device_AddDevice        (DaqObjectPtr device, const char* connectionString);
EXPORTFUN DaqObjectPtr Device_AddFunctionBlock (DaqObjectPtr device, const char* fbId);

EXPORTFUN int          Device_Remove           (DaqObjectPtr device, const char* type, uint64 index);
EXPORTFUN int          Device_RemoveDevice     (DaqObjectPtr device, uint64 index);
EXPORTFUN int          Device_RemoveFunctionBlock(DaqObjectPtr device, uint64 index);

EXPORTFUN const char*  Device_GetAvailableDeviceConnectionString(DaqObjectPtr device, uint64 index);
EXPORTFUN const char*  Device_GetAvailableFunctionBlockID(DaqObjectPtr device, uint64 index);

EXPORTFUN int          Device_LoadConfiguration(DaqObjectPtr device, const char* json);
EXPORTFUN int          Device_LoadConfigurationFromFile(DaqObjectPtr device, const char* json_path);

EXPORTFUN const char*  Device_SaveConfiguration(DaqObjectPtr device);
EXPORTFUN int          Device_SaveConfigurationToFile(DaqObjectPtr device, const char* json_path);

EXPORTFUN int          InputPort_Connect(DaqObjectPtr port, const char* signalid, DaqObjectPtr instance);
EXPORTFUN int          InputPort_Disconnect(DaqObjectPtr port);

EXPORTFUN int          Signal_Read(DaqObjectPtr signal, uint64 NumOfSamples, int timeout);

EXPORTFUN double*      Signal_GetSampleReadings(DaqObjectPtr signal);
EXPORTFUN int64*       Signal_GetSampleTimeStamps(DaqObjectPtr signal);
EXPORTFUN int          Signal_GetSampleReadingsToArray(DaqObjectPtr signal, double* array, uint64 len);
EXPORTFUN int          Signal_GetSampleTimeStampsToArray(DaqObjectPtr signal, int64* array, uint64 len);
EXPORTFUN int          Signal_GetSampleCountOfRead(DaqObjectPtr signal);
EXPORTFUN int          Signal_EraseSamples(DaqObjectPtr signal);

/*
Create instance 
Add device 
Add signal
Signal configuration 
Create reader
Create data descriptor
Read data 
Reading with domain 
Create signal pack
Time reader
*/
