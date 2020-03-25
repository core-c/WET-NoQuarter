#ifdef HW_BAN

#include "q_shared.h"

#include "crc32.h"
#include "md5.h"
#include "xorcrypt.h"

#ifdef WIN32

int getHardDriveSerialNumber (char* HardDriveSerialNumber, char* HardDriveModelNumber);

char* getHWInfo() {
	//reserve 50 bytes for each buffer, yes its a nasty way to do, but va memory is a lot harder to intercept from a cheat
	char* serialNumber = va("                                                  ");
	char* modelNumber = va("                                                  ");

	if (getHardDriveSerialNumber(serialNumber, modelNumber)) {
		return va("Harddrive serialnumber: %s\nHarddrive model: %s\n", serialNumber, modelNumber);
	}
	else {
		return va("%s\n", "Retreiving of the hardware info FAILED!");
	}
}

char* getHWGuid() {
	char *buffer = NULL;
	int i;

	struct MD5Context context;
	unsigned char checksum[16];

	//reserve 50 bytes for each buffer, yes its a nasty way to do, but va memory is a lot harder to intercept from a cheat
	char* serialNumber = va("                                                  ");
	char* modelNumber = va("                                                  ");

	if (!getHardDriveSerialNumber(serialNumber, modelNumber)) {
		Com_Error(ERR_FATAL, "Can't build hardware profile!");

		serialNumber = va("UNKNOWN HDD SERIAL NUMBER");
	}

	MD5Init (&context);
	MD5Update (&context, (unsigned char*) serialNumber, strlen (serialNumber));
	MD5Final (checksum, &context);

	buffer = NULL;
	for (i = 0; i < 16; i++) {
		if (buffer == NULL) {
			buffer = va("%02X", (unsigned int) checksum[i]);
		}
		else {
			buffer = va("%s%02X", buffer, (unsigned int) checksum[i]);
		}
	}

	return buffer;
}
#else
#error For this platform there is no HWBan implementation provided!
#endif

char* getEncryptedHWGuid(const char* serverName, unsigned int serverPort, int clientNum) {
	char *buffer = va(" ");
	struct xor128_state state;

	xor_crypt_init(&state, calculateCRC32(serverName, strlen(serverName)), serverPort * serverPort, calculateCRC32(serverName, strlen(serverName)) * clientNum, calculateCRC32(serverName, strlen(serverName)) * serverPort);

	if (xor_encrypt(&state, (const unsigned char*) getHWGuid(), 32, (unsigned char*) buffer, 1024)) {
		return NULL;
	}

	return buffer;
}

char* getDecryptedHWGuid(const char* hwGuid, const char* serverName, unsigned int serverPort, int clientNum) {
	char *buffer = va(" ");
	struct xor128_state state;

	xor_crypt_init(&state, calculateCRC32(serverName, strlen(serverName)), serverPort * serverPort, calculateCRC32(serverName, strlen(serverName)) * clientNum, calculateCRC32(serverName, strlen(serverName)) * serverPort);

	if (xor_decrypt(&state, (const unsigned char*) hwGuid, strlen(hwGuid), (unsigned char*) buffer, 1024)) {
		return NULL;
	}

	return buffer;
}
#endif // HW_BAN

