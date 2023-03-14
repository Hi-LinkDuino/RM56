/*
	Copyright (C) 2010-2021 Apple Inc. All Rights Reserved. Not to be used or disclosed without permission from Apple.
*/

#ifndef __BonjourBrowser_h__
#define __BonjourBrowser_h__

#include "CommonServices.h"
#include "DebugServices.h"

#include CF_HEADER
#include LIBDISPATCH_HEADER

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@group		BonjourDeviceKeys
	@abstract	Keys for device info.
*/
#define kBonjourDeviceKey_DeviceID "deviceID" //! [String] Persistent, globally unique ID for the device.
#define kBonjourDeviceKey_DNSName "dnsName" //! [String] DNS name for connecting (e.g. "My\032Device._airplay._tcp.local.%4").
#define kBonjourDeviceKey_Domain "domain" //! [String] Domain where service was found (e.g. "local.").
#define kBonjourDeviceKey_InterfaceIndex "ifindex" //! [uint32_t] Index of the interface the service was found.
#define kBonjourDeviceKey_InterfaceName "ifname" //! [String] Name of the inferface the service was found (e.g. "en0").
#define kBonjourDeviceKey_Name "name" //! [String] User-friendly, display name of the device.
#define kBonjourDeviceKey_P2P "p2p" //! [Boolean] True if this service is on a P2P interface.
#define kBonjourDeviceKey_P2POnly "p2pOnly" //! [Boolean] True if this device is only available on P2P interfaces.
#define kBonjourDeviceKey_RawName "rawName" //! [String] True name advertised via Bonjour (not parsed to remore prefixes, etc.).
#define kBonjourDeviceKey_RemovedServices "removedServices" //! [Array] Services that were removed.
#define kBonjourDeviceKey_ServiceType "serviceType" //! [String] Bonjour service type (e.g. "_airplay._tcp").
#define kBonjourDeviceKey_Services "services" //! [Array] Array of per-service dictionaries.
#define kBonjourDeviceKey_TransportType "transportType" //! [Number:NetTransportType] Transport type of the interface service found on.
#define kBonjourDeviceKey_TXT "txt" //! [Data] Raw TXT record data. Use TXTRecord* APIs to parse it.
#define kBonjourDeviceKey_WiFi "wifi" //! [Boolean] True if this service is on a WiFi interface.

#define kBonjourBrowserFlag_StandardID (UINT64_C(1) << 59) // DeviceID uses "id" as the key.
#define kBonjourBrowserFlag_Traffic (UINT64_C(1) << 61) // Traffic reduction enabled (background/detailed).
#define kBonjourBrowserFlag_Background (UINT64_C(1) << 62) // Do a "presence-only" browse.
#define kBonjourBrowserFlag_P2P (UINT64_C(1) << 63) // Get/use a P2P interface.

//---------------------------------------------------------------------------------------------------------------------------
/*!	@group		BonjourBrowser
	@abstract	Browser for Bonjour services.
*/
typedef struct BonjourBrowser* BonjourBrowserRef;

CFTypeID BonjourBrowserGetTypeID(void);
OSStatus BonjourBrowser_Create(BonjourBrowserRef* outBrowser, const char* inLabel);
#define BonjourBrowser_Forget(X) ForgetCustomEx((X), BonjourBrowser_Stop, CFRelease)

OSStatus
BonjourBrowser_Start(
    BonjourBrowserRef inBrowser,
    const char* inServiceType,
    const char* inDomain,
    const char* inIfName,
    uint64_t inFlags);
void BonjourBrowser_Stop(BonjourBrowserRef inBrowser);
CF_RETURNS_RETAINED
CFArrayRef BonjourBrowser_CopyDevices(BonjourBrowserRef inBrowser, OSStatus* outErr);
void BonjourBrowser_ReconfirmDevice(BonjourBrowserRef inBrowser, CFDictionaryRef inDeviceInfo);
void BonjourDevice_Reconfirm(CFDictionaryRef inDeviceInfo);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	BonjourBrowser_SetDispatchQueue
	@abstract	Sets the dispatch queue to use for callbacks.
	@discussion	If no dispatch queue is set (or NULL is specified here), the main queue will be used.
*/
void BonjourBrowser_SetDispatchQueue(BonjourBrowserRef inBrowser, dispatch_queue_t inDispatchQueue);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	BonjourBrowser_SetEventHandler
	@abstract	Sets the function to call when an event occurs.
	@discussion	Invoked on the dispatch queue specified by BonjourBrowser_SetDispatchQueue (defaults to the main queue).
*/
typedef uint32_t BonjourBrowserEventType;
#define kBonjourBrowserEventType_AddOrUpdateDevice 1 //! Device was discovered or updated. Event info contains device info.
#define kBonjourBrowserEventType_RemoveDevice 2 //! Device disappeared from the network. Event info contains device info.
#define kBonjourBrowserEventType_Stop 3 //! Browser was stopped. Event info is undefined.
#define kBonjourBrowserEventType_Restarted 4 //! Bonjour server restarted. All previous results must be invalidated.

#define BonjourBrowserEventTypeToString(X) ( \
    ((X) == kBonjourBrowserEventType_AddOrUpdateDevice) ? "Add" : ((X) == kBonjourBrowserEventType_RemoveDevice) ? "Remove" : ((X) == kBonjourBrowserEventType_Stop) ? "Stop" : ((X) == kBonjourBrowserEventType_Restarted) ? "Restarted" : "UNKNOWN")

typedef void (*BonjourBrowserEventHandlerFunc)(BonjourBrowserEventType inEventType, CFDictionaryRef inEventInfo, void* inContext);

void BonjourBrowser_SetEventHandler(BonjourBrowserRef inBrowser, BonjourBrowserEventHandlerFunc inFunc, void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	BonjourDevice_CopyCFString
	@abstract	Copies a CFString from the TXT record of the device.
*/
CF_RETURNS_RETAINED
CFStringRef BonjourDevice_CopyCFString(CFDictionaryRef inDeviceInfo, const char* inTXTKey, OSStatus* outErr);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	BonjourDevice_GetBitListValue
	@abstract	Gets a BitList value from the TXT record.
	@discussion	Parses a comma-separated string of bit numbers (e.g. "0,2" -> 0x5 since bit 0 and bit 2 are set).
*/
uint32_t BonjourDevice_GetBitListValue(CFDictionaryRef inDeviceInfo, const char* inTXTKey, OSStatus* outErr);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	BonjourDevice_GetDeviceID
	@abstract	Gets a globally unique, persistent identifier for the device as a scalar for easy comparisons.
	@discussion	This can be used to track devices, remember them in the prefs, etc.
				Note: outDeviceID may be NULL if you only get about the scalar return value version of it.
	@result		64-bit scalar device ID (e.g. 48-bit MAC address, zero-prefixed to 64 bits).
*/
uint64_t BonjourDevice_GetDeviceID(CFDictionaryRef inDeviceInfo, uint8_t outDeviceID[6], OSStatus* outErr);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	BonjourDevice_GetDNSName
	@abstract	Gets the best DNS name for the device based to the flags passed in.
*/
OSStatus BonjourDevice_GetDNSName(CFDictionaryRef inDeviceInfo, uint64_t inFlags, char* inBuf, size_t inMaxLen);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	BonjourDevice_GetBoolean / BonjourDevice_GetInt64
	@abstract	Gets a 64-bit integer value from the specifiec TXT record key.
	@discussion	This tries to be smart about parsing so "true" -> 1, "false" -> 0, 0x-prefix is parsed as hex, etc.
*/
#define BonjourDevice_GetBoolean(INFO, KEY, OUT_ERR) \
    ((BonjourDevice_GetInt64((INFO), (KEY), 0, (OUT_ERR)) != 0) ? true : false)
int64_t BonjourDevice_GetInt64(CFDictionaryRef inDeviceInfo, const char* inTXTKey, int inBase, OSStatus* outErr);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	BonjourDevice_MergeInfo
	@abstract	Merges old and new device info into a new dictionary.
	
	@param		ioDeviceInfo		Ptr to existing device info. May point to NULL. Old info released. Caller must release output.
	@param		inNewDeviceInfo		New device info to merge in.
*/
OSStatus BonjourDevice_MergeInfo(CFDictionaryRef* ioDeviceInfo, CFDictionaryRef inNewDeviceInfo);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	BonjourDevice_RemoveInterfaceInfo
	@abstract	Removes service entries with the specified 
	
	@param		ioDeviceInfo	Ptr to existing device info. May point to NULL. Old info released. Caller must release output.
	@param		inIfName		Name of interface to remove (e.g. "en0").
	@param		inRemoveOthers	If true, remove all interfaces other than inIfName. If false, remove only inIfName interfaces.
*/
OSStatus BonjourDevice_RemoveInterfaceInfo(CFDictionaryRef* ioDeviceInfo, const char* inIfName, Boolean inRemoveOthers);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@function	BonjourBrowser_Test
	@abstract	Unit test.
*/
OSStatus BonjourBrowser_Test(void);

#ifdef __cplusplus
}
#endif

#endif // __BonjourBrowser_h__
