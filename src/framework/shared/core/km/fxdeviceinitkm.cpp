/*++

Copyright (c) Microsoft Corporation

Module Name:

    FxDeviceInitKm.cpp

Abstract:
    Internals for WDFDEVICE_INIT

Author:



    
Environment:

    Kernel mode only

Revision History:

--*/

#include "coreprivshared.hpp"

extern "C" {
#include "FxDeviceInitKm.tmh"
}

VOID
WDFDEVICE_INIT::SetPdo(
    __in FxDevice* Parent
    )
{
    InitType = FxDeviceInitTypePdo;

    //
    // Remember the parent so we can store it later in WdfDeviceCreate
    //
    Pdo.Parent = Parent;

    //
    // PDOs *must* have a name.  By setting this flag, the driver writer
    // does not need to know this
    //
    Characteristics |= FILE_AUTOGENERATED_DEVICE_NAME;

    //
    // By default, PDOs are not power pageable b/c they do not know how the
    // stack above them will work.  For a "closed" system where the bus driver
    // knows the stack be loaded on its PDO, this may not be true and it
    // can use WdfDeviceInitSetPowerPageable to set it back.
    //
    // In all current shipping OS's, if the parent is power pageable, the
    // child must be power pagable as well.
    //
    if (Parent->IsPowerPageableCapable() == FALSE) {
        PowerPageable = FALSE;
    }
}

VOID
WDFDEVICE_INIT::AssignIoType(
    _In_ PWDF_IO_TYPE_CONFIG IoTypeConfig
    )
{
    NTSTATUS status;
    
    if (IoTypeConfig->ReadWriteIoType == WdfDeviceIoUndefined || 
        IoTypeConfig->ReadWriteIoType > WdfDeviceIoDirect) {
        status= STATUS_INVALID_PARAMETER;
        DoTraceLevelMessage(
            DriverGlobals, TRACE_LEVEL_ERROR, TRACINGDEVICE,
            "Out of range Read/Write IoType %d, %!status!", 
            IoTypeConfig->ReadWriteIoType, status);
        FxVerifierDbgBreakPoint(DriverGlobals);
        return;
    }

    ReadWriteIoType = IoTypeConfig->ReadWriteIoType;

    return;
}

