#ifdef HW_BAN

//  diskid32.cpp

//  for displaying the details of hard drives in a command window


//  06/11/00  Lynn McGuire  written with many contributions from others,
//                            IDE drives only under Windows NT/2K and 9X,
//                            maybe SCSI drives later
//  11/20/03  Lynn McGuire  added ReadPhysicalDriveInNTWithZeroRights
//  10/26/05  Lynn McGuire  fix the flipAndCodeBytes function
//  01/22/08  Lynn McGuire  incorporate changes from Gonzalo Diethelm,
//                             remove media serial number code since does 
//                             not work on USB hard drives or thumb drives
//  01/29/08  Lynn McGuire  add ReadPhysicalDriveInNTUsingSmart


//#define PRINTING_TO_CONSOLE_ALLOWED

#include <cctype>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <windows.h>
#include <winioctl.h>

//for some weird reason are with MINGW a few structs missing
#ifdef __MINGW32__
#include "bg_hwguid_win32_cross.h"
#else
#define IOCTL_DISK_GET_DRIVE_GEOMETRY_EX CTL_CODE(IOCTL_DISK_BASE, 0x0028, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif

//char HardDriveSerialNumber [1024];
//char HardDriveModelNumber [1024];
int PRINT_DEBUG = false;


#ifdef PRINTING_TO_CONSOLE_ALLOWED
static void dump_buffer (const char* title,
			const unsigned char* buffer,
			int len);
#endif


void WriteConstantString (char *entry, char *string)
{
}



   //  Required to ensure correct PhysicalDrive IOCTL structure setup
#pragma pack(1)


#define  IDENTIFY_BUFFER_SIZE  512


   //  IOCTL commands
#define  DFP_GET_VERSION          0x00074080
#define  DFP_SEND_DRIVE_COMMAND   0x0007c084
#define  DFP_RECEIVE_DRIVE_DATA   0x0007c088

#define  FILE_DEVICE_SCSI              0x0000001b
#define  IOCTL_SCSI_MINIPORT_IDENTIFY  ((FILE_DEVICE_SCSI << 16) + 0x0501)
#define  IOCTL_SCSI_MINIPORT 0x0004D008  //  see NTDDSCSI.H for definition

#define SMART_GET_VERSION               CTL_CODE(IOCTL_DISK_BASE, 0x0020, METHOD_BUFFERED, FILE_READ_ACCESS)
#define SMART_SEND_DRIVE_COMMAND        CTL_CODE(IOCTL_DISK_BASE, 0x0021, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define SMART_RCV_DRIVE_DATA            CTL_CODE(IOCTL_DISK_BASE, 0x0022, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

   //  GETVERSIONOUTPARAMS contains the data returned from the 
   //  Get Driver Version function.
typedef struct _GETVERSIONOUTPARAMS
{
   BYTE bVersion;      // Binary driver version.
   BYTE bRevision;     // Binary driver revision.
   BYTE bReserved;     // Not used.
   BYTE bIDEDeviceMap; // Bit map of IDE devices.
   DWORD fCapabilities; // Bit mask of driver capabilities.
   DWORD dwReserved[4]; // For future use.
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;


   //  Bits returned in the fCapabilities member of GETVERSIONOUTPARAMS 
#define  CAP_IDE_ID_FUNCTION             1  // ATA ID command supported
#define  CAP_IDE_ATAPI_ID                2  // ATAPI ID command supported
#define  CAP_IDE_EXECUTE_SMART_FUNCTION  4  // SMART commannds supported

   //  Valid values for the bCommandReg member of IDEREGS.
#define  IDE_ATAPI_IDENTIFY  0xA1  //  Returns ID sector for ATAPI.
#define  IDE_ATA_IDENTIFY    0xEC  //  Returns ID sector for ATA.

   // The following struct defines the interesting part of the IDENTIFY
   // buffer:
typedef struct _IDSECTOR
{
   USHORT  wGenConfig;
   USHORT  wNumCyls;
   USHORT  wReserved;
   USHORT  wNumHeads;
   USHORT  wBytesPerTrack;
   USHORT  wBytesPerSector;
   USHORT  wSectorsPerTrack;
   USHORT  wVendorUnique[3];
   CHAR    sSerialNumber[20];
   USHORT  wBufferType;
   USHORT  wBufferSize;
   USHORT  wECCSize;
   CHAR    sFirmwareRev[8];
   CHAR    sModelNumber[40];
   USHORT  wMoreVendorUnique;
   USHORT  wDoubleWordIO;
   USHORT  wCapabilities;
   USHORT  wReserved1;
   USHORT  wPIOTiming;
   USHORT  wDMATiming;
   USHORT  wBS;
   USHORT  wNumCurrentCyls;
   USHORT  wNumCurrentHeads;
   USHORT  wNumCurrentSectorsPerTrack;
   ULONG   ulCurrentSectorCapacity;
   USHORT  wMultSectorStuff;
   ULONG   ulTotalAddressableSectors;
   USHORT  wSingleWordDMA;
   USHORT  wMultiWordDMA;
   BYTE    bReserved[128];
} IDSECTOR, *PIDSECTOR;


typedef struct _SRB_IO_CONTROL
{
   ULONG HeaderLength;
   UCHAR Signature[8];
   ULONG Timeout;
   ULONG ControlCode;
   ULONG ReturnCode;
   ULONG Length;
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;


   // Define global buffers.
BYTE IdOutCmd [sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];


char *ConvertToString (DWORD diskdata [256],
		       int firstIndex,
		       int lastIndex,
		       char* buf);
void PrintIdeInfo (int drive, DWORD diskdata [256], char* HardDriveSerialNumber, char* HardDriveModelNumber);
BOOL DoIDENTIFY (HANDLE, PSENDCMDINPARAMS, PSENDCMDOUTPARAMS, BYTE, BYTE,
                 PDWORD);


   //  Max number of drives assuming primary/secondary, master/slave topology
#define  MAX_IDE_DRIVES  16


int ReadPhysicalDriveInNTWithAdminRights (char* HardDriveSerialNumber, char* HardDriveModelNumber)
{
   int done = FALSE;
   int drive = 0;

   for (drive = 0; drive < MAX_IDE_DRIVES; drive++)
   {
      HANDLE hPhysicalDriveIOCTL = 0;

         //  Try to get a handle to PhysicalDrive IOCTL, report failure
         //  and exit if can't.
      char driveName [256];

      sprintf (driveName, "\\\\.\\PhysicalDrive%d", drive);

         //  Windows NT, Windows 2000, must have admin rights
      hPhysicalDriveIOCTL = CreateFile (driveName,
                               GENERIC_READ | GENERIC_WRITE, 
                               FILE_SHARE_READ | FILE_SHARE_WRITE , NULL,
                               OPEN_EXISTING, 0, NULL);
      // if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
      //    printf ("Unable to open physical drive %d, error code: 0x%lX\n",
      //            drive, GetLastError ());

      if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
      {
#ifdef PRINTING_TO_CONSOLE_ALLOWED
         if (PRINT_DEBUG) 
			 printf ("\n%d ReadPhysicalDriveInNTWithAdminRights ERROR"
					 "\nCreateFile(%s) returned INVALID_HANDLE_VALUE\n",
		 			 __LINE__, driveName);
#endif
      }
      else
      {
         GETVERSIONOUTPARAMS VersionParams;
         DWORD               cbBytesReturned = 0;

            // Get the version, etc of PhysicalDrive IOCTL
         memset ((void*) &VersionParams, 0, sizeof(VersionParams));

         if ( ! DeviceIoControl (hPhysicalDriveIOCTL, DFP_GET_VERSION,
                   NULL, 
                   0,
                   &VersionParams,
                   sizeof(VersionParams),
                   &cbBytesReturned, NULL) )
         {         
#ifdef PRINTING_TO_CONSOLE_ALLOWED
            if (PRINT_DEBUG)
            {
	           DWORD err = GetLastError ();
	           printf ("\n%d ReadPhysicalDriveInNTWithAdminRights ERROR"
		               "\nDeviceIoControl(%d, DFP_GET_VERSION) returned 0, error is %d\n",
		               __LINE__, (int) hPhysicalDriveIOCTL, (int) err);
		    }
#endif
         }

            // If there is a IDE device at number "i" issue commands
            // to the device
         if (VersionParams.bIDEDeviceMap <= 0)
         {
#ifdef PRINTING_TO_CONSOLE_ALLOWED
            if (PRINT_DEBUG)
	            printf ("\n%d ReadPhysicalDriveInNTWithAdminRights ERROR"
		                "\nNo device found at position %d (%d)\n",
		                __LINE__, (int) drive, (int) VersionParams.bIDEDeviceMap);
#endif
         }
         else
         {
            BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
            SENDCMDINPARAMS  scip;
            //SENDCMDOUTPARAMS OutCmd;

			   // Now, get the ID sector for all IDE devices in the system.
               // If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
               // otherwise use the IDE_ATA_IDENTIFY command
            bIDCmd = (VersionParams.bIDEDeviceMap >> drive & 0x10) ? \
                      IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;

            memset (&scip, 0, sizeof(scip));
            memset (IdOutCmd, 0, sizeof(IdOutCmd));

            if ( DoIDENTIFY (hPhysicalDriveIOCTL, 
                       &scip, 
                       (PSENDCMDOUTPARAMS)&IdOutCmd, 
                       (BYTE) bIDCmd,
                       (BYTE) drive,
                       &cbBytesReturned))
            {
               DWORD diskdata [256];
               int ijk = 0;
               USHORT *pIdSector = (USHORT *)
                             ((PSENDCMDOUTPARAMS) IdOutCmd) -> bBuffer;

               for (ijk = 0; ijk < 256; ijk++)
                  diskdata [ijk] = pIdSector [ijk];

               PrintIdeInfo (drive, diskdata, HardDriveSerialNumber, HardDriveModelNumber);

               done = TRUE;
            }
	    }

         CloseHandle (hPhysicalDriveIOCTL);
      }
   }

   return done;
}


//
// IDENTIFY data (from ATAPI driver source)
//

#pragma pack(1)

typedef struct _IDENTIFY_DATA {
    USHORT GeneralConfiguration;            // 00 00
    USHORT NumberOfCylinders;               // 02  1
    USHORT Reserved1;                       // 04  2
    USHORT NumberOfHeads;                   // 06  3
    USHORT UnformattedBytesPerTrack;        // 08  4
    USHORT UnformattedBytesPerSector;       // 0A  5
    USHORT SectorsPerTrack;                 // 0C  6
    USHORT VendorUnique1[3];                // 0E  7-9
    USHORT SerialNumber[10];                // 14  10-19
    USHORT BufferType;                      // 28  20
    USHORT BufferSectorSize;                // 2A  21
    USHORT NumberOfEccBytes;                // 2C  22
    USHORT FirmwareRevision[4];             // 2E  23-26
    USHORT ModelNumber[20];                 // 36  27-46
    UCHAR  MaximumBlockTransfer;            // 5E  47
    UCHAR  VendorUnique2;                   // 5F
    USHORT DoubleWordIo;                    // 60  48
    USHORT Capabilities;                    // 62  49
    USHORT Reserved2;                       // 64  50
    UCHAR  VendorUnique3;                   // 66  51
    UCHAR  PioCycleTimingMode;              // 67
    UCHAR  VendorUnique4;                   // 68  52
    UCHAR  DmaCycleTimingMode;              // 69
    USHORT TranslationFieldsValid:1;        // 6A  53
    USHORT Reserved3:15;
    USHORT NumberOfCurrentCylinders;        // 6C  54
    USHORT NumberOfCurrentHeads;            // 6E  55
    USHORT CurrentSectorsPerTrack;          // 70  56
    ULONG  CurrentSectorCapacity;           // 72  57-58
    USHORT CurrentMultiSectorSetting;       //     59
    ULONG  UserAddressableSectors;          //     60-61
    USHORT SingleWordDMASupport : 8;        //     62
    USHORT SingleWordDMAActive : 8;
    USHORT MultiWordDMASupport : 8;         //     63
    USHORT MultiWordDMAActive : 8;
    USHORT AdvancedPIOModes : 8;            //     64
    USHORT Reserved4 : 8;
    USHORT MinimumMWXferCycleTime;          //     65
    USHORT RecommendedMWXferCycleTime;      //     66
    USHORT MinimumPIOCycleTime;             //     67
    USHORT MinimumPIOCycleTimeIORDY;        //     68
    USHORT Reserved5[2];                    //     69-70
    USHORT ReleaseTimeOverlapped;           //     71
    USHORT ReleaseTimeServiceCommand;       //     72
    USHORT MajorRevision;                   //     73
    USHORT MinorRevision;                   //     74
    USHORT Reserved6[50];                   //     75-126
    USHORT SpecialFunctionsEnabled;         //     127
    USHORT Reserved7[128];                  //     128-255
} IDENTIFY_DATA, *PIDENTIFY_DATA;

#pragma pack()



int ReadPhysicalDriveInNTUsingSmart (char* HardDriveSerialNumber, char* HardDriveModelNumber)
{
   int done = FALSE;
   int drive = 0;

   for (drive = 0; drive < MAX_IDE_DRIVES; drive++)
   {
      HANDLE hPhysicalDriveIOCTL = 0;

         //  Try to get a handle to PhysicalDrive IOCTL, report failure
         //  and exit if can't.
      char driveName [256];

      sprintf (driveName, "\\\\.\\PhysicalDrive%d", drive);

         //  Windows NT, Windows 2000, Windows Server 2003, Vista
      hPhysicalDriveIOCTL = CreateFile (driveName,
                               GENERIC_READ | GENERIC_WRITE, 
                               FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, 
							   NULL, OPEN_EXISTING, 0, NULL);
      // if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
      //    printf ("Unable to open physical drive %d, error code: 0x%lX\n",
      //            drive, GetLastError ());

      if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
      {
#ifdef PRINTING_TO_CONSOLE_ALLOWED
         if (PRINT_DEBUG) 
			 printf ("\n%d ReadPhysicalDriveInNTUsingSmart ERROR"
					 "\nCreateFile(%s) returned INVALID_HANDLE_VALUE\n"
					 "Error Code %d\n",
		 			 __LINE__, driveName, GetLastError ());
#endif
      }
      else
      {
         GETVERSIONINPARAMS GetVersionParams;
         DWORD cbBytesReturned = 0;

            // Get the version, etc of PhysicalDrive IOCTL
         memset ((void*) & GetVersionParams, 0, sizeof(GetVersionParams));

         if ( ! DeviceIoControl (hPhysicalDriveIOCTL, SMART_GET_VERSION,
                   NULL, 
                   0,
     			   &GetVersionParams, sizeof (GETVERSIONINPARAMS),
				   &cbBytesReturned, NULL) )
         {         
#ifdef PRINTING_TO_CONSOLE_ALLOWED
            if (PRINT_DEBUG)
            {
	           DWORD err = GetLastError ();
	           printf ("\n%d ReadPhysicalDriveInNTUsingSmart ERROR"
		               "\nDeviceIoControl(%d, SMART_GET_VERSION) returned 0, error is %d\n",
		               __LINE__, (int) hPhysicalDriveIOCTL, (int) err);
		    }
#endif
         }
         else
         {
			 	// Print the SMART version
           	// PrintVersion (& GetVersionParams);
	           // Allocate the command buffer
			ULONG CommandSize = sizeof(SENDCMDINPARAMS) + IDENTIFY_BUFFER_SIZE;
        	PSENDCMDINPARAMS Command = (PSENDCMDINPARAMS) malloc (CommandSize);
	           // Retrieve the IDENTIFY data
	           // Prepare the command
#define ID_CMD          0xEC            // Returns ID sector for ATA
			Command -> irDriveRegs.bCommandReg = ID_CMD;
			DWORD BytesReturned = 0;
	        if ( ! DeviceIoControl (hPhysicalDriveIOCTL, 
				                    SMART_RCV_DRIVE_DATA, Command, sizeof(SENDCMDINPARAMS),
									Command, CommandSize,
									&BytesReturned, NULL) )
            {
		           // Print the error
		        //PrintError ("SMART_RCV_DRIVE_DATA IOCTL", GetLastError());
	        } 
			else
			{
        	       // Print the IDENTIFY data
                DWORD diskdata [256];
                USHORT *pIdSector = (USHORT *)
                             (PIDENTIFY_DATA) ((PSENDCMDOUTPARAMS) Command) -> bBuffer;

                for (int ijk = 0; ijk < 256; ijk++)
                   diskdata [ijk] = pIdSector [ijk];

                PrintIdeInfo (drive, diskdata, HardDriveSerialNumber, HardDriveModelNumber);
                done = TRUE;
			}
	           // Done
            CloseHandle (hPhysicalDriveIOCTL);
			free (Command);
		 }
      }
   }

   return done;
}

#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)

	//  function to decode the serial numbers of IDE hard drives
	//  using the IOCTL_STORAGE_QUERY_PROPERTY command 
char * flipAndCodeBytes (const char * str,
			 int pos,
			 int flip,
			 char * buf)
{
   int i;
   int j = 0;
   int k = 0;

   buf [0] = '\0';
   if (pos <= 0)
      return buf;

   if ( ! j)
   {
      char p = 0;

      // First try to gather all characters representing hex digits only.
      j = 1;
      k = 0;
      buf[k] = 0;
      for (i = pos; j && str[i] != '\0'; ++i)
      {
	 char c = tolower(str[i]);

	 if (isspace(c))
	    c = '0';

	 ++p;
	 buf[k] <<= 4;

	 if (c >= '0' && c <= '9')
	    buf[k] |= (unsigned char) (c - '0');
	 else if (c >= 'a' && c <= 'f')
	    buf[k] |= (unsigned char) (c - 'a' + 10);
	 else
	 {
	    j = 0;
	    break;
	 }

	 if (p == 2)
	 {
	    if (buf[k] != '\0' && ! isprint(buf[k]))
	    {
	       j = 0;
	       break;
	    }
	    ++k;
	    p = 0;
	    buf[k] = 0;
	 }

      }
   }

   if ( ! j)
   {
      // There are non-digit characters, gather them as is.
      j = 1;
      k = 0;
      for (i = pos; j && str[i] != '\0'; ++i)
      {
	     char c = str[i];

	     if ( ! isprint(c))
	     {
	        j = 0;
	        break;
	     }

	     buf[k++] = c;
      }
   }

   if ( ! j)
   {
      // The characters are not there or are not printable.
      k = 0;
   }

   buf[k] = '\0';

   if (flip)
      // Flip adjacent characters
      for (j = 0; j < k; j += 2)
      {
	     char t = buf[j];
	     buf[j] = buf[j + 1];
	     buf[j + 1] = t;
      }

   // Trim any beginning and end space
   i = j = -1;
   for (k = 0; buf[k] != '\0'; ++k)
   {
      if (! isspace(buf[k]))
      {
	     if (i < 0)
	        i = k;
	     j = k;
      }
   }

   if ((i >= 0) && (j >= 0))
   {
      for (k = i; (k <= j) && (buf[k] != '\0'); ++k)
         buf[k - i] = buf[k];
      buf[k - i] = '\0';
   }

   return buf;
}

int ReadPhysicalDriveInNTWithZeroRights (char* HardDriveSerialNumber, char* HardDriveModelNumber)
{
   int done = FALSE;
   int drive = 0;

   for (drive = 0; drive < MAX_IDE_DRIVES; drive++)
   {
      HANDLE hPhysicalDriveIOCTL = 0;

         //  Try to get a handle to PhysicalDrive IOCTL, report failure
         //  and exit if can't.
      char driveName [256];

      sprintf (driveName, "\\\\.\\PhysicalDrive%d", drive);

         //  Windows NT, Windows 2000, Windows XP - admin rights not required
      hPhysicalDriveIOCTL = CreateFile (driveName, 0,
                               FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                               OPEN_EXISTING, 0, NULL);
      if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
      {
#ifdef PRINTING_TO_CONSOLE_ALLOWED
         if (PRINT_DEBUG)
             printf ("\n%d ReadPhysicalDriveInNTWithZeroRights ERROR"
		             "\nCreateFile(%s) returned INVALID_HANDLE_VALUE\n",
		             __LINE__, driveName);
#endif
      }
      else
      {
		 STORAGE_PROPERTY_QUERY query;
         DWORD cbBytesReturned = 0;
		 char buffer [10000];

         memset ((void *) & query, 0, sizeof (query));
		 query.PropertyId = StorageDeviceProperty;
		 query.QueryType = PropertyStandardQuery;

		 memset (buffer, 0, sizeof (buffer));

         if ( DeviceIoControl (hPhysicalDriveIOCTL, IOCTL_STORAGE_QUERY_PROPERTY,
                   & query,
                   sizeof (query),
				   & buffer,
				   sizeof (buffer),
                   & cbBytesReturned, NULL) )
         {         
			 STORAGE_DEVICE_DESCRIPTOR * descrip = (STORAGE_DEVICE_DESCRIPTOR *) & buffer;
			 char serialNumber [1000];
			 char modelNumber [1000];
             char vendorId [1000];
	         char productRevision [1000];

#ifdef PRINTING_TO_CONSOLE_ALLOWED
             if (PRINT_DEBUG)
             {
                 printf ("\n%d STORAGE_DEVICE_DESCRIPTOR contents for drive %d\n"
		                 "                Version: %ld\n"
		                 "                   Size: %ld\n"
		                 "             DeviceType: %02x\n"
		                 "     DeviceTypeModifier: %02x\n"
		                 "         RemovableMedia: %d\n"
		                 "        CommandQueueing: %d\n"
		                 "         VendorIdOffset: %4ld (0x%02lx)\n"
		                 "        ProductIdOffset: %4ld (0x%02lx)\n"
		                 "  ProductRevisionOffset: %4ld (0x%02lx)\n"
		                 "     SerialNumberOffset: %4ld (0x%02lx)\n"
		                 "                BusType: %d\n"
		                 "    RawPropertiesLength: %ld\n",
		                 __LINE__, drive,
			             (unsigned long) descrip->Version,
		                 (unsigned long) descrip->Size,
		                 (int) descrip->DeviceType,
		                 (int) descrip->DeviceTypeModifier,
		                 (int) descrip->RemovableMedia,
		                 (int) descrip->CommandQueueing,
		                 (unsigned long) descrip->VendorIdOffset,
		                 (unsigned long) descrip->VendorIdOffset,
		                 (unsigned long) descrip->ProductIdOffset,
		                 (unsigned long) descrip->ProductIdOffset,
		                 (unsigned long) descrip->ProductRevisionOffset,
		                 (unsigned long) descrip->ProductRevisionOffset,
		                 (unsigned long) descrip->SerialNumberOffset,
		                 (unsigned long) descrip->SerialNumberOffset,
		                 (int) descrip->BusType,
		                 (unsigned long) descrip->RawPropertiesLength);

	            dump_buffer ("Contents of RawDeviceProperties",
			                 (unsigned char*) descrip->RawDeviceProperties,
			                 descrip->RawPropertiesLength);

	            dump_buffer ("Contents of first 256 bytes in buffer",
			                 (unsigned char*) buffer, 256);
			 }
#endif
             flipAndCodeBytes (buffer,
                               descrip -> VendorIdOffset,
			                   0, vendorId );
	         flipAndCodeBytes (buffer,
			                   descrip -> ProductIdOffset,
			                   0, modelNumber );
	         flipAndCodeBytes (buffer,
			                   descrip -> ProductRevisionOffset,
			                   0, productRevision );
	         flipAndCodeBytes (buffer,
			                   descrip -> SerialNumberOffset,
			                   1, serialNumber );

			 if (0 == HardDriveSerialNumber [0] &&
						//  serial number must be alphanumeric
			            //  (but there can be leading spaces on IBM drives)
				   (isalnum (serialNumber [0]) || isalnum (serialNumber [19])))
			 {
				strcpy (HardDriveSerialNumber, serialNumber);
				strcpy (HardDriveModelNumber, modelNumber);
				done = TRUE;
			 }
#ifdef PRINTING_TO_CONSOLE_ALLOWED
             printf ("\n**** STORAGE_DEVICE_DESCRIPTOR for drive %d ****\n"
		             "Vendor Id = [%s]\n"
		             "Product Id = [%s]\n"
		             "Product Revision = [%s]\n"
		             "Serial Number = [%s]\n",
		             drive,
		             vendorId,
		             modelNumber,
		             productRevision,
		             serialNumber);
#endif
	           // Get the disk drive geometry.
	         memset (buffer, 0, sizeof(buffer));
	         if ( ! DeviceIoControl (hPhysicalDriveIOCTL,
			          IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
			          NULL,
			          0,
			          &buffer,
			          sizeof(buffer),
			          &cbBytesReturned,
			          NULL))
	         {
#ifdef PRINTING_TO_CONSOLE_ALLOWED
                if (PRINT_DEBUG)
	               printf ("\n%d ReadPhysicalDriveInNTWithZeroRights ERROR"
		                   "|nDeviceIoControl(%s, IOCTL_DISK_GET_DRIVE_GEOMETRY_EX) returned 0",
		                   driveName);
#endif
          	 }
	         else
	         {  
#ifdef PRINTING_TO_CONSOLE_ALLOWED
	            DISK_GEOMETRY_EX* geom = (DISK_GEOMETRY_EX*) &buffer;
	            int fixed = (geom->Geometry.MediaType == FixedMedia);
	            __int64 size = geom->DiskSize.QuadPart;

	            printf ("\n**** DISK_GEOMETRY_EX for drive %d ****\n"
		                "Disk is%s fixed\n"
		                "DiskSize = %I64d\n",
		                drive,
		                fixed ? "" : " NOT",
		                size);
#endif
	        }
         }
		 else
		 {
#ifdef PRINTING_TO_CONSOLE_ALLOWED
			 DWORD err = GetLastError ();
			 printf ("\nDeviceIOControl IOCTL_STORAGE_QUERY_PROPERTY error = %d\n", err);
#endif
		 }

         CloseHandle (hPhysicalDriveIOCTL);
      }
   }

   return done;
}


   // DoIDENTIFY
   // FUNCTION: Send an IDENTIFY command to the drive
   // bDriveNum = 0-3
   // bIDCmd = IDE_ATA_IDENTIFY or IDE_ATAPI_IDENTIFY
BOOL DoIDENTIFY (HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
                 PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
                 PDWORD lpcbBytesReturned)
{
      // Set up data structures for IDENTIFY command.
   pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;
   pSCIP -> irDriveRegs.bFeaturesReg = 0;
   pSCIP -> irDriveRegs.bSectorCountReg = 1;
   //pSCIP -> irDriveRegs.bSectorNumberReg = 1;
   pSCIP -> irDriveRegs.bCylLowReg = 0;
   pSCIP -> irDriveRegs.bCylHighReg = 0;

      // Compute the drive number.
   pSCIP -> irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4);

      // The command can either be IDE identify or ATAPI identify.
   pSCIP -> irDriveRegs.bCommandReg = bIDCmd;
   pSCIP -> bDriveNumber = bDriveNum;
   pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;

   return ( DeviceIoControl (hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
               (LPVOID) pSCIP,
               sizeof(SENDCMDINPARAMS) - 1,
               (LPVOID) pSCOP,
               sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,
               lpcbBytesReturned, NULL) );
}


#define  SENDIDLENGTH  sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE


int ReadIdeDriveAsScsiDriveInNT (char* HardDriveSerialNumber, char* HardDriveModelNumber)
{
   int done = FALSE;
   int controller = 0;

   for (controller = 0; controller < 16; controller++)
   {
      HANDLE hScsiDriveIOCTL = 0;
      char   driveName [256];

         //  Try to get a handle to PhysicalDrive IOCTL, report failure
         //  and exit if can't.
      sprintf (driveName, "\\\\.\\Scsi%d:", controller);

         //  Windows NT, Windows 2000, any rights should do
      hScsiDriveIOCTL = CreateFile (driveName,
                               GENERIC_READ | GENERIC_WRITE, 
                               FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                               OPEN_EXISTING, 0, NULL);
      // if (hScsiDriveIOCTL == INVALID_HANDLE_VALUE)
      //    printf ("Unable to open SCSI controller %d, error code: 0x%lX\n",
      //            controller, GetLastError ());

      if (hScsiDriveIOCTL != INVALID_HANDLE_VALUE)
      {
         int drive = 0;

         for (drive = 0; drive < 2; drive++)
         {
            char buffer [sizeof (SRB_IO_CONTROL) + SENDIDLENGTH];
            SRB_IO_CONTROL *p = (SRB_IO_CONTROL *) buffer;
            SENDCMDINPARAMS *pin =
                   (SENDCMDINPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
            DWORD dummy;
   
            memset (buffer, 0, sizeof (buffer));
            p -> HeaderLength = sizeof (SRB_IO_CONTROL);
            p -> Timeout = 10000;
            p -> Length = SENDIDLENGTH;
            p -> ControlCode = IOCTL_SCSI_MINIPORT_IDENTIFY;
            strncpy ((char *) p -> Signature, "SCSIDISK", 8);
  
            pin -> irDriveRegs.bCommandReg = IDE_ATA_IDENTIFY;
            pin -> bDriveNumber = drive;

            if (DeviceIoControl (hScsiDriveIOCTL, IOCTL_SCSI_MINIPORT, 
                                 buffer,
                                 sizeof (SRB_IO_CONTROL) +
                                         sizeof (SENDCMDINPARAMS) - 1,
                                 buffer,
                                 sizeof (SRB_IO_CONTROL) + SENDIDLENGTH,
                                 &dummy, NULL))
            {
               SENDCMDOUTPARAMS *pOut =
                    (SENDCMDOUTPARAMS *) (buffer + sizeof (SRB_IO_CONTROL));
               IDSECTOR *pId = (IDSECTOR *) (pOut -> bBuffer);
               if (pId -> sModelNumber [0])
               {
                  DWORD diskdata [256];
                  int ijk = 0;
                  USHORT *pIdSector = (USHORT *) pId;
          
                  for (ijk = 0; ijk < 256; ijk++)
                     diskdata [ijk] = pIdSector [ijk];

                  PrintIdeInfo (controller * 2 + drive, diskdata, HardDriveSerialNumber, HardDriveModelNumber);

                  done = TRUE;
               }
            }
         }
         CloseHandle (hScsiDriveIOCTL);
      }
   }

   return done;
}


void PrintIdeInfo (int drive, DWORD diskdata [256], char* HardDriveSerialNumber, char* HardDriveModelNumber)
{
   char serialNumber [1024];
   char modelNumber [1024];
   char revisionNumber [1024];
   char bufferSize [32];

#ifdef PRINTING_TO_CONSOLE_ALLOWED
   __int64 sectors = 0;
   __int64 bytes = 0;
#endif

      //  copy the hard drive serial number to the buffer
   ConvertToString (diskdata, 10, 19, serialNumber);
   ConvertToString (diskdata, 27, 46, modelNumber);
   ConvertToString (diskdata, 23, 26, revisionNumber);
   sprintf (bufferSize, "%u", (unsigned int) diskdata [21] * 512);

   if (0 == HardDriveSerialNumber [0] &&
       //  serial number must be alphanumeric
       //  (but there can be leading spaces on IBM drives)
       (isalnum (serialNumber [0]) || isalnum (serialNumber [19])))
   {
      strcpy (HardDriveSerialNumber, serialNumber);
      strcpy (HardDriveModelNumber, modelNumber);
   }

#ifdef PRINTING_TO_CONSOLE_ALLOWED

   printf ("\nDrive %d - ", drive);

   switch (drive / 2)
   {
      case 0: printf ("Primary Controller - ");
              break;
      case 1: printf ("Secondary Controller - ");
              break;
      case 2: printf ("Tertiary Controller - ");
              break;
      case 3: printf ("Quaternary Controller - ");
              break;
   }

   switch (drive % 2)
   {
      case 0: printf (" - Master drive\n\n");
              break;
      case 1: printf (" - Slave drive\n\n");
              break;
   }

   printf ("Drive Model Number________________: [%s]\n",
           modelNumber);
   printf ("Drive Serial Number_______________: [%s]\n",
           serialNumber);
   printf ("Drive Controller Revision Number__: [%s]\n",
           revisionNumber);

   printf ("Controller Buffer Size on Drive___: %s bytes\n",
           bufferSize);

   printf ("Drive Type________________________: ");
   if (diskdata [0] & 0x0080)
      printf ("Removable\n");
   else if (diskdata [0] & 0x0040)
      printf ("Fixed\n");
   else printf ("Unknown\n");
           
		//  calculate size based on 28 bit or 48 bit addressing
		//  48 bit addressing is reflected by bit 10 of word 83
	if (diskdata [83] & 0x400) 
		sectors = diskdata [103] * 65536I64 * 65536I64 * 65536I64 + 
					diskdata [102] * 65536I64 * 65536I64 + 
					diskdata [101] * 65536I64 + 
					diskdata [100];
	else
		sectors = diskdata [61] * 65536 + diskdata [60];
		//  there are 512 bytes in a sector
	bytes = sectors * 512;
	printf ("Drive Size________________________: %I64d bytes\n",
			bytes);

#endif  // PRINTING_TO_CONSOLE_ALLOWED

   char string1 [1000];
   sprintf (string1, "Drive%dModelNumber", drive);
   WriteConstantString (string1, modelNumber);

   sprintf (string1, "Drive%dSerialNumber", drive);
   WriteConstantString (string1, serialNumber);

   sprintf (string1, "Drive%dControllerRevisionNumber", drive);
   WriteConstantString (string1, revisionNumber);

   sprintf (string1, "Drive%dControllerBufferSize", drive);
   WriteConstantString (string1, bufferSize);

   sprintf (string1, "Drive%dType", drive);
   if (diskdata [0] & 0x0080)
      WriteConstantString (string1, "Removable");
   else if (diskdata [0] & 0x0040)
      WriteConstantString (string1, "Fixed");
   else
      WriteConstantString (string1, "Unknown");
}



char *ConvertToString (DWORD diskdata [256],
		       int firstIndex,
		       int lastIndex,
		       char* buf)
{
   int index = 0;
   int position = 0;

      //  each integer has two characters stored in it backwards
   for (index = firstIndex; index <= lastIndex; index++)
   {
         //  get high byte for 1st character
      buf [position++] = (char) (diskdata [index] / 256);

         //  get low byte for 2nd character
      buf [position++] = (char) (diskdata [index] % 256);
   }

      //  end the string 
   buf[position] = '\0';

      //  cut off the trailing blanks
   for (index = position - 1; index > 0 && isspace(buf [index]); index--)
      buf [index] = '\0';

   return buf;
}

#ifdef PRINTING_TO_CONSOLE_ALLOWED
static void dump_buffer (const char* title,
			const unsigned char* buffer,
			int len)
{
   int i = 0;
   int j;

   printf ("\n-- %s --\n", title);
   if (len > 0)
   {
      printf ("%8.8s ", " ");
      for (j = 0; j < 16; ++j)
      {
	    printf (" %2X", j);
      }
      printf ("  ");
      for (j = 0; j < 16; ++j)
      {
	    printf ("%1X", j);
      }
      printf ("\n");
   }
   while (i < len)
   {
      printf("%08x ", i);
      for (j = 0; j < 16; ++j)
      {
	 if ((i + j) < len)
	    printf (" %02x", (int) buffer[i +j]);
	 else
	    printf ("   ");
      }
      printf ("  ");
      for (j = 0; j < 16; ++j)
      {
	 if ((i + j) < len)
	    printf ("%c", isprint (buffer[i + j]) ? buffer [i + j] : '.');
	 else
	    printf (" ");
      }
      printf ("\n");
      i += 16;
   }
   printf ("-- DONE --\n");
}
#endif

extern "C" {
	int getHardDriveSerialNumber (char* HardDriveSerialNumber, char* HardDriveModelNumber) {
		int done = FALSE;
		OSVERSIONINFO version;

		strcpy (HardDriveSerialNumber, "");
		strcpy (HardDriveModelNumber, "");

		memset (&version, 0, sizeof (version));
		version.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		GetVersionEx (&version);
		if (version.dwPlatformId == VER_PLATFORM_WIN32_NT) {
			if (!done)
				done = ReadPhysicalDriveInNTWithAdminRights(HardDriveSerialNumber, HardDriveModelNumber);

			if (!done)
				done = ReadIdeDriveAsScsiDriveInNT(HardDriveSerialNumber, HardDriveModelNumber);

			if (!done)
				done = ReadPhysicalDriveInNTWithZeroRights(HardDriveSerialNumber, HardDriveModelNumber);

			if (!done)
				done = ReadPhysicalDriveInNTUsingSmart(HardDriveSerialNumber, HardDriveModelNumber);
		}

		return done;
	}
};


#endif // HW_BAN
