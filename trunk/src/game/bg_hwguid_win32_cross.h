#ifdef HW_BAN

#ifndef __BG_HWGUID_WIN32_CROSS_H__
#define __BG_HWGUID_WIN32_CROSS_H__

//This file contains a few structs and enums to cross compile the harddrive detection code
//For some reason it's not included in mingw while in visual studio you get redefinition error's

// Status returned from driver
typedef struct _DRIVERSTATUS
{
	BYTE  bDriverError;  //  Error code from driver, or 0 if no error.
	BYTE  bIDEStatus;    //  Contents of IDE Error register.
	//  Only valid when bDriverError is SMART_IDE_ERROR.
	BYTE  bReserved[2];  //  Reserved for future expansion.
	DWORD  dwReserved[2];  //  Reserved for future expansion.
} DRIVERSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS;

//  IDE registers
typedef struct _IDEREGS
{
	BYTE bFeaturesReg;       // Used for specifying SMART "commands".
	BYTE bSectorCountReg;    // IDE sector count register
	BYTE bSectorNumberReg;   // IDE sector number register
	BYTE bCylLowReg;         // IDE low order cylinder value
	BYTE bCylHighReg;        // IDE high order cylinder value
	BYTE bDriveHeadReg;      // IDE drive/head register
	BYTE bCommandReg;        // Actual IDE command.
	BYTE bReserved;          // reserved for future use.  Must be zero.
} IDEREGS, *PIDEREGS, *LPIDEREGS;

// Structure returned by PhysicalDrive IOCTL for several commands
typedef struct _SENDCMDOUTPARAMS
{
	DWORD         cBufferSize;   //  Size of bBuffer in bytes
	DRIVERSTATUS  DriverStatus;  //  Driver status structure.
	BYTE          bBuffer[1];    //  Buffer of arbitrary length in which to store the data read from the                                                       // drive.
} SENDCMDOUTPARAMS, *PSENDCMDOUTPARAMS, *LPSENDCMDOUTPARAMS;

//  SENDCMDINPARAMS contains the input parameters for the 
//  Send Command to Drive function.
typedef struct _SENDCMDINPARAMS
{
	DWORD     cBufferSize;   //  Buffer size in bytes
	IDEREGS   irDriveRegs;   //  Structure with drive register values.
	BYTE bDriveNumber;       //  Physical drive number to send 
	//  command to (0,1,2,3).
	BYTE bReserved[3];       //  Reserved for future expansion.
	DWORD     dwReserved[4]; //  For future use.
	BYTE      bBuffer[1];    //  Input buffer.
} SENDCMDINPARAMS, *PSENDCMDINPARAMS, *LPSENDCMDINPARAMS;

typedef enum _STORAGE_PROPERTY_ID {
	StorageDeviceProperty                   = 0,
	StorageAdapterProperty,
	StorageDeviceIdProperty,
	StorageDeviceUniqueIdProperty,
	StorageDeviceWriteCacheProperty,
	StorageMiniportProperty,
	StorageAccessAlignmentProperty,
	StorageDeviceSeekPenaltyProperty,
	StorageDeviceTrimProperty,
	StorageDeviceWriteAggregationProperty 
} STORAGE_PROPERTY_ID, *PSTORAGE_PROPERTY_ID;

typedef enum _STORAGE_QUERY_TYPE {
	PropertyStandardQuery     = 0,
	PropertyExistsQuery,
	PropertyMaskQuery,
	PropertyQueryMaxDefined 
} STORAGE_QUERY_TYPE, *PSTORAGE_QUERY_TYPE;

typedef enum _STORAGE_BUS_TYPE {
	BusTypeUnknown       = 0x00,
	BusTypeScsi          = 0x01,
	BusTypeAtapi         = 0x02,
	BusTypeAta           = 0x03,
	BusType1394          = 0x04,
	BusTypeSsa           = 0x05,
	BusTypeFibre         = 0x06,
	BusTypeUsb           = 0x07,
	BusTypeRAID          = 0x08,
	BusTypeiSCSI         = 0x09,
	BusTypeSas           = 0x0A,
	BusTypeSata          = 0x0B,
	BusTypeMaxReserved   = 0x7F 
} STORAGE_BUS_TYPE, *PSTORAGE_BUS_TYPE;

typedef struct _STORAGE_DEVICE_DESCRIPTOR {
	ULONG  Version;
	ULONG  Size;
	UCHAR  DeviceType;
	UCHAR  DeviceTypeModifier;
	BOOLEAN  RemovableMedia;
	BOOLEAN  CommandQueueing;
	ULONG  VendorIdOffset;
	ULONG  ProductIdOffset;
	ULONG  ProductRevisionOffset;
	ULONG  SerialNumberOffset;
	STORAGE_BUS_TYPE  BusType;
	ULONG  RawPropertiesLength;
	UCHAR  RawDeviceProperties[1];
} STORAGE_DEVICE_DESCRIPTOR, *PSTORAGE_DEVICE_DESCRIPTOR;

typedef struct _STORAGE_PROPERTY_QUERY {
	STORAGE_PROPERTY_ID PropertyId;
	STORAGE_QUERY_TYPE  QueryType;
	BYTE                AdditionalParameters[1];
} STORAGE_PROPERTY_QUERY, *PSTORAGE_PROPERTY_QUERY;

typedef struct _GETVERSIONINPARAMS {
	UCHAR bVersion;
	UCHAR bRevision;
	UCHAR bReserved;
	UCHAR bIDEDeviceMap;
	ULONG fCapabilities;
	ULONG dwReserved[4];
} GETVERSIONINPARAMS, *PGETVERSIONINPARAMS, *LPGETVERSIONINPARAMS;

#endif

#endif // HW_BAN
