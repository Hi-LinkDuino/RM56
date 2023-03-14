/*
	Copyright (C) 2012-2021 Apple Inc. All Rights Reserved.
*/
/*!
	@header			Pairing API
	@discussion		APIs for performing cryptographic pairing between entities.
*/

#ifndef PairingUtils_h
#define PairingUtils_h

#include "CommonServices.h"
#include "LogUtils.h"

#include CF_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#if 0
#pragma mark == Configuration ==
#endif

//===========================================================================================================================

#define PLATFORM_HAS_PAIRING_MANAGER 0
#define PAIRING_UTILS_HAS_BUILT_IN_UI 0
#define PAIRING_UTILS_HAS_APPLE_ID_SUPPORT 0

#if 0
#pragma mark -
#pragma mark == ACLs ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Access control list items to grant or require permission for specific operations.
*/

// Enables admin functionality, such as setting passwords.
#define kPairingACL_Admin "com.apple.admin"

// Enables remote Xcode, remote Console.app, remote Instruments, etc.
#define kPairingACL_Developer "com.apple.developer"

// For diagnosing problems with devices.
#define kPairingACL_DeviceDiagnostics "com.apple.DeviceDiagnostics"

// Enables Continuity Keyboard, Remote.app, and other remote control-like functionality.
#define kPairingACL_RemoteControl "com.apple.RemoteControl"

// Enables remote, live capturing of screen.
#define kPairingACL_ScreenCapture "com.apple.ScreenCapture"

#if 0
#pragma mark -
#pragma mark == Delegate ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Feature flags for pairing.
*/
typedef uint8_t PairingFeatureFlags;
#define kPairingFeatureFlags_None 0
#define kPairingFeatureFlag_MFiPairSetup (1 << 0) // Supports MFi pair-setup.

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Flags for controlling pairing.
*/
typedef uint32_t PairingFlags;
#define kPairingFlags_None 0
#define kPairingFlag_MFi (1 << 0) // 0x0000001: For controller to require proof that accessory has an MFi auth IC.
#define kPairingFlag_Resume (1 << 1) // 0x0000002: To allow saving and resuming sessions.
#define kPairingFlag_System (1 << 2) // 0x0000004: Use system-level pairing instead of per-app pairing.
#define kPairingFlag_PerDevice (1 << 3) // 0x0000008: Use per-device instead of per-iCloud account pairing.
#define kPairingFlag_Transient (1 << 4) // 0x0000010: Use PairSetup for a shared secret, but don't exchange or save long-term keys.
#define kPairingFlag_Info (1 << 5) // 0x0000020: Always include peer info in the requests/responses.
#define kPairingFlag_UserSession (1 << 6) // 0x0000040: Allows system daemons to connect to a per-user session.
#define kPairingFlag_BuiltInUI (1 << 7) // 0x0000080: Use the built-in UI instead of calling showSetupCode/hideSetupCode.
#define kPairingFlag_Visual (1 << 8) // 0x0000100: Use VisualPairing with a camera and display instead of entering a PIN.
#define kPairingFlag_NFC (1 << 9) // 0x0000200: Use NFC to read a PIN instead of asking the user to enter it.
#define kPairingFlag_Siri (1 << 10) // 0x0000400: Use Siri to speak the PIN and the user can enter what they hear.
#define kPairingFlag_Audio (1 << 11) // 0x0000800: Embed PIN in an audio signal and use the microphone to extract it.
#define kPairingFlag_AppleID (1 << 12) // 0x0001000: Use AppleID certificates/signatures for PairVerify/
#define kPairingFlag_MFiAuthIC (1 << 13) // 0x0002000: Supports MFi auth IC.
#define kPairingFlag_MFiSoftwareCert (1 << 14) // 0x0004000: Supports software certificate-based MFi auth.
#define kPairingFlag_MFiToken (1 << 15) // 0x0008000: Supports token-based MFi auth.
#define kPairingFlag_Incorrect (1 << 16) // 0x0010000: Indicates a previously entered setup code was incorrect.
#define kPairingFlag_Throttle (1 << 17) // 0x0020000: Peer is throttling setup attempts. Retry later.
#define kPairingFlag_Match (1 << 18) // 0x0040000: Ask user to confirm the setup code matches the display on both sides.
#define kPairingFlag_Reset (1 << 19) // 0x0080000: Reset the pairing session.
#define kPairingFlag_AppleIDNoSelfProve (1 << 20) // 0x0100000: Don't prove our AppleID.
#define kPairingFlag_AppleIDNoPeerVerify (1 << 21) // 0x0200000: Don't require the peer to prove its AppleID.
#define kPairingFlag_HomeKit (1 << 22) // 0x0400000: Use HomeKit keys.
#define kPairingFlag_UniqueResume (1 << 23) // 0x0800000: Requires a unique peer and self identifer for resume matches.
#define kPairingFlag_SplitSetup (1 << 24) // 0x1000000: PairSetup first with temp key then later to exchange keys.
#define kPairingFlag_HomeKitAccessory (1 << 25) // 0x2000000: Use HomeKit accessory keys.
#define kPairingFlag_HomeKitUser (1 << 26) // 0x4000000: Use HomeKit user keys.

#define kPairingFlag_NonLegacy ( \
    kPairingFlag_System | kPairingFlag_Transient | kPairingFlag_Info | kPairingFlag_Visual | kPairingFlag_NFC | kPairingFlag_Siri | kPairingFlag_Audio | kPairingFlag_AppleID | kPairingFlag_MFiAuthIC | kPairingFlag_MFiSoftwareCert | kPairingFlag_MFiToken | kPairingFlag_AppleIDNoSelfProve | kPairingFlag_AppleIDNoPeerVerify | kPairingFlag_HomeKit | kPairingFlag_SplitSetup | kPairingFlag_HomeKitAccessory | kPairingFlag_HomeKitUser | 0)
#define kPairingFlag_RemoteSafe ( \
    kPairingFlag_Visual | kPairingFlag_NFC | kPairingFlag_Siri | kPairingFlag_Audio | kPairingFlag_AppleID | kPairingFlag_MFiAuthIC | kPairingFlag_MFiSoftwareCert | kPairingFlag_MFiToken | kPairingFlag_AppleIDNoSelfProve | kPairingFlag_AppleIDNoPeerVerify | kPairingFlag_HomeKit | kPairingFlag_SplitSetup | kPairingFlag_HomeKitAccessory | kPairingFlag_HomeKitUser | 0)

#define kPairingFlagsDescriptors \
    "\x00"                       \
    "MFi\0"                      \
    "\x01"                       \
    "Resume\0"                   \
    "\x02"                       \
    "System\0"                   \
    "\x03"                       \
    "PerDevice\0"                \
    "\x04"                       \
    "Transient\0"                \
    "\x05"                       \
    "Info\0"                     \
    "\x06"                       \
    "UserSession\0"              \
    "\x07"                       \
    "BuiltInUI\0"                \
    "\x08"                       \
    "Visual\0"                   \
    "\x09"                       \
    "NFC\0"                      \
    "\x0A"                       \
    "Siri\0"                     \
    "\x0B"                       \
    "Audio\0"                    \
    "\x0C"                       \
    "AppleID\0"                  \
    "\x0C"                       \
    "MFiAuthIC\0"                \
    "\x0C"                       \
    "MFiSWCert\0"                \
    "\x0C"                       \
    "MFiToken\0"                 \
    "\x10"                       \
    "Incorrect\0"                \
    "\x11"                       \
    "Throttle\0"                 \
    "\x12"                       \
    "Match\0"                    \
    "\x13"                       \
    "Reset\0"                    \
    "\x14"                       \
    "AppleIDNoSelfProve\0"       \
    "\x15"                       \
    "AppleIDNoPeerVerify\0"      \
    "\x16"                       \
    "HomeKit\0"                  \
    "\x17"                       \
    "UniqueResume\0"             \
    "\x18"                       \
    "SplitSetup\0"               \
    "\x19"                       \
    "HomeKitAccessory\0"         \
    "\x1A"                       \
    "HomeKitUser\0"              \
    "\x00"

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Feature flags for pairing.
*/
typedef uint32_t PairingPermissions;
#define kPairingPermissionsDefault 0 // Normal user.
#define kPairingPermissionsAdmin (1U << 0) // Admin that is able to add and remove pairings.

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Type of setup code to use.
*/
typedef uint32_t PairingSetupCodeType;
#define kPairingSetupCodeTypeInvalid 0 // Reserved for an invalid value.
#define kPairingSetupCodeType4Digit 1 // 4-digit PIN (e.g. "1234").
#define kPairingSetupCodeType6Digit 2 // 6-digit PIN (e.g. "123456" to display as "123 456").
#define kPairingSetupCodeType8Digit 3 // 8-digit PIN (e.g. "12345678" to display as "123 45 678").
#define kPairingSetupCodeType4Char 4 // 4 character, uppercase, alpha-numeric string (e.g. "C1X4").

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		State of pairing with a device.
*/
typedef uint32_t CUPairState;
#define kCUPairStateUnknown 0 // State hasn't been determined yet.
#define kCUPairStateNotPaired 10 // Not paired.
#define kCUPairStateConjectured 20 // Some indication of being paired (e.g. BLE auth tag matched), but not verified.
#define kCUPairStateVerified 30 // Bi-directional pairing has been PairVerify'd.

STATIC_INLINE const char* CUPairStateToString(CUPairState inState)
{
    switch (inState) {
    case kCUPairStateUnknown:
        return ("Unknown");
    case kCUPairStateNotPaired:
        return ("NotPaired");
    case kCUPairStateConjectured:
        return ("Conjectured");
    case kCUPairStateVerified:
        return ("Verified");
    default:
        return ("?");
    }
}

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Constants for pairing.
*/
#define kPairingMIMEType "application/pairing+tlv8"

// Read/Write keys for encrypting data using pairing session keys.
// See <https://spc.apple.com/Pairing-Spec.html#_aead_algorithm_a_id_aead_algorithm_a>.

#define kPairingPrimaryReadKeySaltPtr "ReadKeySalt"
#define kPairingPrimaryReadKeySaltLen sizeof_string(kPairingPrimaryReadKeySaltPtr)
#define kPairingPrimaryReadKeyInfoPtr "ReadKeyInfo"
#define kPairingPrimaryReadKeyInfoLen sizeof_string(kPairingPrimaryReadKeyInfoPtr)

#define kPairingPrimaryWriteKeySaltPtr "WriteKeySalt"
#define kPairingPrimaryWriteKeySaltLen sizeof_string(kPairingPrimaryWriteKeySaltPtr)
#define kPairingPrimaryWriteKeyInfoPtr "WriteKeyInfo"
#define kPairingPrimaryWriteKeyInfoLen sizeof_string(kPairingPrimaryWriteKeyInfoPtr)

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Generate a NUL-terminated setup code and show it to the user.
	
	The setup code must be in the format XXX-XX-XXX where X is a 0-9 digit in ASCII (e.g. "123-45-678").
	If the setup code is being generated on-the-fly (recommended), it must come from a cryptographic random number generator.
	If the setup code is fixed (e.g. printed on a label and burnt into an EEPROM) then it must have been generated using 
	cryptographic random number generator during manufacturing (i.e. don't use a simple counter, manufacture date, region
	of origin, etc. since that can significantly improve an attackers ability to guess it).
*/
typedef OSStatus (*PairingShowSetupCode_f)(PairingFlags inFlags, char* inBuffer, size_t inMaxLen, void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Hide any setup code that may be visible for this session.
*/
typedef void (*PairingHideSetupCode_f)(void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Prompt the user for a setup code.
	
	The expectation is that this callback will display a dialog and immediately return. When the user enters the setup code, 
	it should be set with PairingSessionSetSetupCode and then PairingSessionExchange should be called to resume the exchange 
	process. If the user cancels the setup code entry dialog, the pairing session can just be released.
	
	@param		inFlags				Flags for setup code prompt.
	@param		inDelaySeconds		< 0 means no delay. >= 0 means the UI must wait N seconds before trying again.
	@param		inContext			Context pointer provided by the delegate.
*/
typedef OSStatus (*PairingPromptForSetupCode_f)(PairingFlags inFlags, int32_t inDelaySeconds, void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Optionally copy the identifier, get the Ed25519 public key, and/or get the secret key of this device.
*/
typedef OSStatus (*PairingCopyIdentity_f)(
    Boolean inAllowCreate,
    char** outIdentifier,
    uint8_t outPK[32],
    uint8_t outSK[32],
    void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Find a peer's Ed25519 public key by its identifier.
*/
typedef OSStatus (*PairingFindPeer_f)(
    const void* inIdentifierPtr,
    size_t inIdentifierLen,
    uint8_t outPK[32],
    uint64_t* outPermissions,
    void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Process a resume request and return a resume response.
*/
typedef OSStatus (*PairingResumeRequest_f)(
    const void* inPeerIdentifierPtr,
    size_t inPeerIdentifierLen,
    const void* inRequestPtr,
    size_t inRequestLen,
    uint8_t** outResponsePtr,
    size_t* outResponseLen,
    void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Process a resume response.
*/
typedef OSStatus (*PairingResumeResponse_f)(const void* inRequestPtr, size_t inRequestLen, void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Save a peer's Ed25519 public key.
*/
typedef OSStatus (*PairingSavePeer_f)(
    const void* inIdentifierPtr,
    size_t inIdentifierLen,
    const uint8_t inPK[32],
    void* inContext);

// PairingDelegate

typedef struct
{
    void* context;
    PairingShowSetupCode_f showSetupCode_f;
    PairingHideSetupCode_f hideSetupCode_f;
    PairingPromptForSetupCode_f promptForSetupCode_f;
    PairingCopyIdentity_f copyIdentity_f;
    PairingFindPeer_f findPeer_f;
    PairingSavePeer_f savePeer_f;
    PairingResumeRequest_f resumeRequest_f;
    PairingResumeResponse_f resumeResponse_f;

} PairingDelegate;

#define PairingDelegateInit(PTR) memset((PTR), 0, sizeof(PairingDelegate))

#if 0
#pragma mark -
#pragma mark == PairingSession ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Type of pairing session.
*/
typedef uint32_t PairingSessionType;
#define kPairingSessionType_None 0 // Used for accessing pairing info without actually pairing.
#define kPairingSessionType_SetupClient 1 // Client-side of pair-setup (side that enters setup code).
#define kPairingSessionType_SetupServer 2 // Server-side of pair-setup (side that displays setup code).
#define kPairingSessionType_VerifyClient 3 // Client-side of pair-verify.
#define kPairingSessionType_VerifyServer 4 // Server-side of pair-verify.
#define kPairingSessionType_ResumeClient 5 // Client-side of pair-resume.
#define kPairingSessionType_ResumeServer 6 // Server-side of pair-resume.

#define kPairingSessionType_AddPairingClient 9 // Client-side of AddPairing.
// Inputs:
//		kPairingSessionPropertyControllerIdentifier	-- Identifier of controller to add.
//		kPairingSessionPropertyControllerPublicKey	-- Public key of controller to add.
//		kPairingSessionPropertyPermissions			-- Permissions to grant controller.
// Outputs: None

#define kPairingSessionType_AddPairingServer 10 // Server-side of AddPairing.
// Use PairingSessionSetAddPairingHandler to handle AddPairing requests.

#define kPairingSessionType_RemovePairingClient 11 // Client-side of RemovePairing.
// Inputs:  kPairingSessionPropertyControllerIdentifier -- Identifier of controller to remove.
// Outputs: None

#define kPairingSessionType_RemovePairingServer 12 // Server-side of RemovePairing.
// Use PairingSessionSetRemovePairingHandler to handle RemovePairing requests.

#define kPairingSessionType_ListPairingsClient 13 // Client-side of ListPairings.
// Inputs:	None
// Outputs:

#define kPairingSessionType_ListPairingsServer 14 // Server-side of ListPairings.
// Use PairingSessionSetListPairingsHandler to handle ListPairings requests.

STATIC_INLINE bool PairingSessionTypeIsClient(PairingSessionType inType)
{
    switch (inType) {
    case kPairingSessionType_SetupClient:
    case kPairingSessionType_VerifyClient:
    case kPairingSessionType_ResumeClient:
    case kPairingSessionType_AddPairingClient:
    case kPairingSessionType_RemovePairingClient:
    case kPairingSessionType_ListPairingsClient:
        return (true);

    default:
        return (false);
    }
}

STATIC_INLINE const char* PairingSessionTypeToString(PairingSessionType inType)
{
    switch (inType) {
    case kPairingSessionType_None:
        return ("None");
    case kPairingSessionType_SetupClient:
        return ("PairSetupClient");
    case kPairingSessionType_SetupServer:
        return ("PairSetupServer");
    case kPairingSessionType_VerifyClient:
        return ("PairVerifyClient");
    case kPairingSessionType_VerifyServer:
        return ("PairVerifyServer");
    case kPairingSessionType_ResumeClient:
        return ("PairResumeClient");
    case kPairingSessionType_ResumeServer:
        return ("PairResumeServer");
    case kPairingSessionType_AddPairingClient:
        return ("AddPairingClient");
    case kPairingSessionType_AddPairingServer:
        return ("AddPairingServer");
    case kPairingSessionType_RemovePairingClient:
        return ("RemovePairingClient");
    case kPairingSessionType_RemovePairingServer:
        return ("RemovePairingServer");
    case kPairingSessionType_ListPairingsClient:
        return ("ListPairingsClient");
    case kPairingSessionType_ListPairingsServer:
        return ("ListPairingsServer");
    default:
        return ("?");
    }
}

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Constants related to pairing sessions.
*/
#define kPairingKey_ACL "acl" // [Dictionary] Key/value pairs for approved operations.
#define kPairingKey_AltIRK "altIRK" // [Data] AltIRK.
#define kPairingKey_AccountID "accountID" // [String] Pairing account identifier UUID string.
#define kPairingKey_HostID "HostID" // [String] Host ID.
#define kPairingKey_Identifier "identifier" // [String] Identifier of the peer.
#define kPairingKey_Info "info" // [Dictionary] Extra info about the peer.
#define kPairingKey_MAC "mac" // [Data:6] Primary MAC address of the peer.
#define kPairingKey_Model "model" // [String] Hardware model of the peer.
#define kPairingKey_Name "name" // [String] Name of the peer.
#define kPairingKey_Permissions "permissions" // [Number:PairingPermissions] Permissions.
#define kPairingKey_PublicKey "pk" // [Data] Public key.
#define kPairingKey_SecretKey "sk" // [Data] Secret key.
#define kPairingKey_UDID "udid" // [String:40] 40 hex byte UDID.
#define kPairingKey_WiFiMAC "wifiMAC" // [Data:6] WiFi MAC address.

// Constants for deriving session encryption keys.

#define kPairingControlKeySaltPtr "Control-Salt"
#define kPairingControlKeySaltLen sizeof_string(kPairingControlKeySaltPtr)
#define kPairingControlKeyReadInfoPtr "Control-Read-Encryption-Key"
#define kPairingControlKeyReadInfoLen sizeof_string(kPairingControlKeyReadInfoPtr)
#define kPairingControlKeyWriteInfoPtr "Control-Write-Encryption-Key"
#define kPairingControlKeyWriteInfoLen sizeof_string(kPairingControlKeyWriteInfoPtr)

// [Boolean] Indicates if the pairing peer has been registered by HomeKit.
#define kPairingKeychainInfoKey_HomeKitRegistered CFSTR("homeKitRegistered")

// [Number:PairingPermissions] Permissions for the controller.
#define kPairingKeychainInfoKey_Permissions CFSTR("permissions")

// Keychain parameters and macros for system-level pairing.

#define kPairingKeychainAccessGroup_System CFSTR("com.apple.pairing")
#define kPairingKeychainAccountIdentityTypeStr_System CFSTR("Account Pairing Identity")
#define kPairingKeychainDeviceIdentityTypeStr_System CFSTR("Device Pairing Identity")
#define kPairingKeychainAccountPeerTypeStr_System CFSTR("Account Paired Peer")
#define kPairingKeychainDevicePeerTypeStr_System CFSTR("Device Paired Peer")

// Keychain parameters and macros for AirPlay/CarPlay pairing.

#define kPairingKeychainAccessGroup_AirPlay CFSTR("com.apple.airplay.pairing")
#define kPairingKeychainIdentityType_AirPlay 0x41504964 // 'APId'
#define kPairingKeychainIdentityLabel_AirPlay CFSTR("AirPlay Pairing Identity")
#define kPairingKeychainPeerType_AirPlay 0x41505072 // 'APPr'
#define kPairingKeychainPeerLabel_AirPlay CFSTR("Paired AirPlay Accessory")
#define kPairingKeychainFlags_AirPlay (kPairingKeychainFlag_iCloudIdentity | kPairingKeychainFlag_iCloudPeers)

#define kPairingKeychainIdentityType_AirPlay_HomeKit 0x4150484B // 'APHK'
#define kPairingKeychainIdentityLabel_AirPlay_HomeKit CFSTR("AirPlay-HomeKit Pairing Identity")
#define kPairingKeychainPeerType_AirPlay_HomeKit 0x41505048 // 'APPH'
#define kPairingKeychainPeerLabel_AirPlay_HomeKit CFSTR("Paired AirPlay-HomeKit Accessory")

#define PairingSessionSetKeychainInfo_AirPlay(SESSION) \
    PairingSessionSetKeychainInfo((SESSION),           \
        kPairingKeychainAccessGroup_AirPlay,           \
        kPairingKeychainIdentityType_AirPlay,          \
        kPairingKeychainIdentityLabel_AirPlay,         \
        kPairingKeychainIdentityLabel_AirPlay,         \
        kPairingKeychainPeerType_AirPlay,              \
        kPairingKeychainPeerLabel_AirPlay,             \
        kPairingKeychainPeerLabel_AirPlay,             \
        kPairingKeychainFlags_AirPlay)

// Keychain parameters and macros for HomeKit pairing.

#define kPairingKeychainAccessGroup_HomeKit CFSTR("com.apple.hap.pairing")
#define kPairingKeychainIdentityType_HomeKit 0x68617043 // 'hapC'
#define kPairingKeychainIdentityType_HomeKitV2 0x68617063 // 'hapc'
#define kPairingKeychainIdentityLabel_HomeKit CFSTR("HomeKit Pairing Identity")
#define kPairingKeychainPeerType_HomeKit 0x68617041 // 'hapA'
#define kPairingKeychainPeerLabel_HomeKit CFSTR("Paired HomeKit Accessory")
#define kPairingKeychainFlags_HomeKit (kPairingKeychainFlag_iCloudIdentity | kPairingKeychainFlag_iCloudPeers)

#define PairingSessionSetKeychainInfo_HomeKit(SESSION) \
    PairingSessionSetKeychainInfo((SESSION),           \
        kPairingKeychainAccessGroup_HomeKit,           \
        kPairingKeychainIdentityType_HomeKit,          \
        kPairingKeychainIdentityLabel_HomeKit,         \
        kPairingKeychainIdentityLabel_HomeKit,         \
        kPairingKeychainPeerType_HomeKit,              \
        kPairingKeychainPeerLabel_HomeKit,             \
        kPairingKeychainPeerLabel_HomeKit,             \
        kPairingKeychainFlags_HomeKit)

// Keychain parameters and macros for Remote app pairing.

#define kPairingKeychainAccessGroup_RemoteApp CFSTR("com.apple.MediaRemote.pairing")
#define kPairingKeychainIdentityType_RemoteApp 0x48444964 // 'HDId'
#define kPairingKeychainIdentityLabel_RemoteApp CFSTR("Remote App Pairing Identity")
#define kPairingKeychainPeerType_RemoteApp 0x48445072 // 'HDPr'
#define kPairingKeychainPeerLabel_RemoteApp CFSTR("Remote App Paired Peer")
#define kPairingKeychainFlags_RemoteApp kPairingKeychainFlags_None

#define PairingSessionSetKeychainInfo_RemoteApp(SESSION) \
    PairingSessionSetKeychainInfo((SESSION),             \
        kPairingKeychainAccessGroup_RemoteApp,           \
        kPairingKeychainIdentityType_RemoteApp,          \
        kPairingKeychainIdentityLabel_RemoteApp,         \
        kPairingKeychainIdentityLabel_RemoteApp,         \
        kPairingKeychainPeerType_RemoteApp,              \
        kPairingKeychainPeerLabel_RemoteApp,             \
        kPairingKeychainPeerLabel_RemoteApp,             \
        kPairingKeychainFlags_RemoteApp)

// Misc Keychain parameters.

#define kPairingKeychainAccessGroup_None CFSTR("")

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Creates a new pairing session. Use CFRelease to release when you're done with it.
*/
typedef struct PairingSessionPrivate* PairingSessionRef;

CFTypeID PairingSessionGetTypeID(void);
OSStatus PairingSessionCreate(PairingSessionRef* outSession, const PairingDelegate* inDelegate, PairingSessionType inType);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Invalidates the session and clears any strong references to break retain cycles.
*/
void PairingSessionInvalidate(PairingSessionRef inSession);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		For PairVerify, the ACL granted on success. For PairVerify, the ACL required to succeed.
*/
OSStatus PairingSessionSetACL(PairingSessionRef inSession, CFDictionaryRef inACL);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Sets the dispatch queue to use for handlers.
*/
void PairingSessionSetDispatchQueue(PairingSessionRef inSession, dispatch_queue_t inQueue);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Copies any extra data securely received from the peer via pair-setup.
*/
uint8_t* PairingSessionCopyExtraData(PairingSessionRef inSession, size_t* outLen, OSStatus* outErr);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Sets extra data to be securely sent to the peer via pair-setup.
*/
OSStatus PairingSessionSetExtraData(PairingSessionRef inSession, const void* inPtr, size_t inLen);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Set flags to control pairing.
*/
void PairingSessionSetFlags(PairingSessionRef inSession, PairingFlags inFlags);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Sets the identifier for this side of a pairing transaction.
*/
OSStatus PairingSessionSetIdentifier(PairingSessionRef inSession, const void* inPtr, size_t inLen);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Returns info about the peer, if it provided any during PairSetup.
*/
CF_RETURNS_RETAINED
CFDictionaryRef PairingSessionCopyPeerInfo(PairingSessionRef inSession, OSStatus* outErr);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Additional info to add to the peer's pairing record when PairSetup completes successfully.
*/
OSStatus PairingSessionSetAdditionalPeerInfo(PairingSessionRef inSession, CFDictionaryRef inInfo);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Additional info to send to the peer during PairSetup for it to store in its pairing record.
*/
OSStatus PairingSessionSetAdditionalSelfInfo(PairingSessionRef inSession, CFDictionaryRef inInfo);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Sets the strings and types used when getting and storing items in the Keychain.
	
	inAccessGroup			Keychain access group to use for accessing items. Defaults to "com.apple.pairing".
	inIdentityType			Type used for programmatic searches of pairing Keychain identities. Defaults to 'prId'.
	inIdentityLabel			Prefix for the label of the identity Keychain item. Defaults to "Pairing Identity".
	inIdentityDescription	Describes how the Keychain identity item is used. Defaults to "Pairing Identity".
	inPeerType				Type used for programmatic searches of pairing Keychain identities. Defaults to 'prId'.
	inPeerLabel				Prefix for the label of the peer Keychain item. Defaults to "Paired Peer".
	inPeerDescription		Describes how Keychain peer item are used. Defaults to "Paired Peer".
	
*/
typedef uint32_t PairingKeychainFlags;
#define kPairingKeychainFlags_None 0
#define kPairingKeychainFlag_iCloudIdentity (1 << 0) // Sync identity Keychain item to the iCloud Keychain.
#define kPairingKeychainFlag_iCloudPeers (1 << 1) // Sync peer Keychain items to the iCloud Keychain.
#define kPairingKeychainFlag_HighSecurity (1 << 3) // Only allow keys to be accessed when device is unlocked.

void PairingSessionSetKeychainInfo(
    PairingSessionRef inSession,
    CFStringRef inAccessGroup, // May be NULL to not set.
    uint32_t inIdentityType, // May be 0 to not set.
    CFStringRef inIdentityLabel, // May be NULL to not set.
    CFStringRef inIdentityDescription, // May be NULL to not set.
    uint32_t inPeerType, // May be 0 to not set.
    CFStringRef inPeerLabel, // May be NULL to not set.
    CFStringRef inPeerDescription, // May be NULL to not set.
    PairingKeychainFlags inFlags);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Sets the log category to use for HTTP message logging.
*/
void PairingSessionSetLogging(PairingSessionRef inSession, LogCategory* inLogCategory);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Sets the maximum number of pairing attempts before the system is locked out and requires a reset.
	
	Defaults to 0 to use an exponentially increasing delay instead of a fixed count.
*/
void PairingSessionSetMaxTries(PairingSessionRef inSession, int inMaxTries);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Sets the maximum number of bytes in the data output by PairingSessionExchange. Defaults to SIZE_MAX.
*/
OSStatus PairingSessionSetMTU(PairingSessionRef inSession, size_t inMTU);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Returns a malloc'd, null-terminated copy of the peer identifier string or NULL and kNotFoundErr if it's not set.
	
	For clients, the peer identifier is only available after pair-setup has completed successfully.
	For servers, the peer identifier is only available after pair-verify has completed successfully.
*/
char* PairingSessionCopyPeerIdentifier(PairingSessionRef inSession, size_t* outLen, OSStatus* outErr);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Gets the resume session ID. Only valid after pair resume completes.
*/
OSStatus PairingSessionGetResumeInfo(PairingSessionRef inSession, uint64_t* outSessionID);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Sets the session ID to use for a pair-resume and request data to include.
*/
OSStatus
PairingSessionSetResumeInfo(
    PairingSessionRef inSession,
    uint64_t inSessionID,
    const void* inResumeRequestPtr,
    size_t inResumeRequestLen);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Sets the max number of resume sessions to track.
*/
OSStatus PairingSessionSetMaxResumeSessions(PairingSessionRef inSession, uint32_t inMaxSessions);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Sets the number of seconds resume state remains valid.
*/
OSStatus PairingSessionSetResumeTTL(PairingSessionRef inSession, uint32_t inSeconds);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Sets the setup code.
*/
OSStatus PairingSessionSetSetupCode(PairingSessionRef inSession, const void* inPtr, size_t inLen);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Derives an encryption key from the pair-verify shared secret.
*/
OSStatus
PairingSessionDeriveKey(
    PairingSessionRef inSession,
    const void* inSaltPtr,
    size_t inSaltLen,
    const void* inInfoPtr,
    size_t inInfoLen,
    size_t inKeyLen,
    uint8_t* outKey);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Exchanges messages with a peer for pairing.
	
	@param		inInputPtr		Input data from the peer.
	@param		inInputLen		Number of bytes of input data.
	@param		outOutputPtr	Malloc'd response data to send to the peer. Caller must free when done.
	@param		outOutputLen	Number of bytes of response data.
	@param		outDone			Receives true if the exchange has completed successfully. Keep calling until error or done.
*/
OSStatus
PairingSessionExchange(
    PairingSessionRef inSession,
    const void* inInputPtr,
    size_t inInputLen,
    uint8_t** outOutputPtr,
    size_t* outOutputLen,
    Boolean* outDone);

#if 0
#pragma mark -
#pragma mark == Handlers ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Handler to invoke when an event occurs.
*/
typedef uint32_t PairingSessionEventType;
#define kPairingSessionEventTypeInvalid 0 // Reserved for an invalid event.
#define kPairingSessionEventTypeInvalidated 1 // Session invalidated. This is the final event.
#define kPairingSessionEventTypeContinue 2 // Handler should continue by calling PairingSessionExchange.

typedef void (*PairingSessionEventHandler_f)(PairingSessionEventType inEvent, CFDictionaryRef inInfo, void* inContext);
void PairingSessionSetEventHandler(PairingSessionRef inSession, PairingSessionEventHandler_f inHandler, void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Handler to add a pairing relationship for an additional controller.
	
	Keys in the controller info dictionary:
	
	kPairingKey_Identifier	-- Identifier of additional controller to add.
	kPairingKey_Permissions	-- Permissions to give additional controller.
	kPairingKey_PublicKey	-- Public key of additional controller to add.
*/
typedef OSStatus (*PairingSessionAddPairingHandler_f)(CFDictionaryRef inControllerInfo, void* inContext);

void PairingSessionSetAddPairingHandler(
    PairingSessionRef inSession,
    PairingSessionAddPairingHandler_f inHandler,
    void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Handler to remove the pairing relationship of an additional controller.
	
	Keys in the controller info dictionary:
	
	kPairingKey_Identifier	-- Identifier of additional controller to remove.
*/
typedef OSStatus (*PairingSessionRemovePairingHandler_f)(CFDictionaryRef inControllerInfo, void* inContext);

void PairingSessionSetRemovePairingHandler(
    PairingSessionRef inSession,
    PairingSessionRemovePairingHandler_f inHandler,
    void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Handler to list all pairing.
	
	Array of dictionaries. Each dictionary has the following keys:
	
	kPairingKey_Identifier	-- Identifier of additional controller to add.
	kPairingKey_Permissions	-- Permissions to give additional controller.
	kPairingKey_PublicKey	-- Public key of additional controller to add.
*/
typedef CFArrayRef (*PairingSessionListPairingsHandler_f)(OSStatus* outErr, void* inContext);

void PairingSessionSetListPairingsHandler(
    PairingSessionRef inSession,
    PairingSessionListPairingsHandler_f inHandler,
    void* inContext);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Handler to invoke to create an MFi signature using the MFi auth IC or MFi software auth private key.
*/
typedef CFDataRef (*PairingSessionCreateSignatureHandler_f)(
    PairingFlags inFlags,
    const void* inChallengePtr,
    size_t inChallengeLen,
    OSStatus* outErr,
    void* inContext);
void PairingSessionSetCreateSignatureHandler(
    PairingSessionRef inSession,
    PairingSessionCreateSignatureHandler_f inHandler,
    void* inContext);

#if 0
#pragma mark -
#pragma mark == Identity and Peers ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Deletes the pairing identity. This API is only used by test tools.
*/
OSStatus PairingSessionDeleteIdentity(PairingSessionRef inSession);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Copies the pairing identity.
	
	@param		inAllowCreate		If true and the identity doesn't exist, it'll be created.
	@param		outIdentifier		Receives malloc'd pointer to NUL-terminated identifier of identity. May be NULL.
	@param		outPK				Receives the 32-byte public key. May be NULL.
	@param		outSK				Optionally receives the 32-byte public key. May be NULL.
*/
OSStatus
PairingSessionCopyIdentity(
    PairingSessionRef inSession,
    Boolean inAllowCreate,
    char** outIdentifier,
    uint8_t* outPK,
    uint8_t* outSK);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Search for a peer by identifier and returns a copy of it if found.
	
	See kPairingKey_* for descriptions of the keys in each peer dictionary.
*/
CF_RETURNS_RETAINED
CFDictionaryRef
PairingSessionCopyPeer(
    PairingSessionRef inSession,
    const void* inIdentifierPtr,
    size_t inIdentifierLen,
    OSStatus* outErr);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Returns an array of peer dictionaries. Returns an empty array if there are no peers.
	
	See kPairingKey_* for descriptions of the keys in each peer dictionary.
*/
CF_RETURNS_RETAINED
CFArrayRef PairingSessionCopyPeers(PairingSessionRef inSession, OSStatus* outErr);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Deletes the pairing peer.
	
	@param		inIdentifierPtr		Ptr to UTF-8 of peer to remove. If NULL, all peers are removed.
	@param		inIdentifierLen		Number of bytes in inIdentifierPtr. May be kSizeCString if inIdentifierPtr is NUL terminated.
*/
OSStatus PairingSessionDeletePeer(PairingSessionRef inSession, const void* inIdentifierPtr, size_t inIdentifierLen);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Finds a peer in the persistent store.
	
	@param		inIdentifierPtr		Ptr to UTF-8 of peer to find.
	@param		inIdentifierLen		Number of bytes in inIdentifierPtr. May be kSizeCString if inIdentifierPtr is NUL terminated.
	@param		outPK				Receives the 32-byte public key.
*/
OSStatus PairingSessionFindPeer(PairingSessionRef inSession, const void* inIdentifierPtr, size_t inIdentifierLen, uint8_t* outPK);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Saves a peer to the persistent store.
	
	@param		inIdentifierPtr		Ptr to UTF-8 of peer to save.
	@param		inIdentifierLen		Number of bytes in inIdentifierPtr. May be kSizeCString if inIdentifierPtr is NUL terminated.
	@param		inPK				Public key of the peer.
*/
OSStatus
PairingSessionSavePeer(
    PairingSessionRef inSession,
    const void* inIdentifierPtr,
    size_t inIdentifierLen,
    const uint8_t* inPK);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Updates the info dictionary for a peer.
*/
OSStatus
PairingSessionUpdatePeerInfo(
    PairingSessionRef inSession,
    const void* inIdentifierPtr,
    size_t inIdentifierLen,
    CFDictionaryRef inInfo);

#if 0
#pragma mark -
#pragma mark == Properties ==
#endif

//---------------------------------------------------------------------------------------------------------------------------
/*!	@defgrou	Properties for a PairingSession.
*/

// [Data] DER-encoded, X.509 certificate.
#define kPairingSessionPropertyCertificate CFSTR("certificate")

// [Data/String] Identifier of the controller for AddPairing/RemovePairing.
#define kPairingSessionPropertyControllerIdentifier CFSTR("controllerIdentifier")

// [Data:32] Long-term Ed25519 public key for AddPairing/RemovePairing.
#define kPairingSessionPropertyControllerPublicKey CFSTR("controllerPK")

// [String] MFi product type identifier string.
#define kPairingSessionPropertyMFiProductType CFSTR("mfiProductType")

// [Dictionary] MFi token info from Apple's token server. May contain multiple tokens, etc.
#define kPairingSessionPropertyMFiTokenInfo CFSTR("mfiTokenInfo")

// [Array:Dictionary] Pairings from a ListPairings response. See kPairingKey_* for dictionary keys.
#define kPairingSessionPropertyPairings CFSTR("pairings")

// [Number:PairingPermissions] Permissions for AddPairing.
#define kPairingSessionPropertyPermissions CFSTR("permissions")

// [String] Accessory serial number.
#define kPairingSessionPropertySerialNumber CFSTR("serialNumber")

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Returns a CFRetain'd property or NULL if the property isn't found.
*/
CF_RETURNS_RETAINED
CFTypeRef PairingSessionCopyProperty(PairingSessionRef inSession, CFStringRef inKey, OSStatus* outErr);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Sets a property.
*/
OSStatus PairingSessionSetProperty(PairingSessionRef inSession, CFStringRef inKey, CFTypeRef inValue);

//---------------------------------------------------------------------------------------------------------------------------
/*!	@brief		Unit test.
*/
void PairingUtilsTest(void);

#ifdef __cplusplus
}
#endif

#endif // PairingUtils_h
