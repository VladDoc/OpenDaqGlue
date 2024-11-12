#pragma once

enum ErrorCodes
{
    EC_OK                        =  0,
    EC_SUCCESS                   =  0,
    EC_GENERIC_ERROR             = -1,
    EC_INVALID_HANDLE            = -2,
    EC_INVALID_POINTER           = -2,
    EC_METHOD_NOT_IMPLEMENTED    = -3,
    EC_OBJECT_TYPE_MISMATCH      = -4,
    EC_PROPERTY_DOESNT_EXIST     = -5,
    EC_ARRAY_OUT_OF_BOUNDS       = -6,
    EC_CONNECTION_STRING_INVALID = -7,
    EC_FUNCTION_BLOCK_ID_INVALID = -7,
    EC_SIGNAL_ID_INVALID         = -7,
    EC_NOT_AVAILABLE             = -8,
    EC_IO_ERROR                  = -9,
    EC_UNINITIALIZED             = -10,
    EC_INSUFFICIENT_SIZE         = -11,
    EC_INVALID_JSON              = -12,
    EC_OPENDAQ_ERROR             = -13
};
