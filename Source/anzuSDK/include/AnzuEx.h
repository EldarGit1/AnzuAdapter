//----------------------------------------------------------------------------------------------------------------------------------

/** @file      AnzuEx.h
 * @author     Micahel Badichi, anzu.io.
 *
 * @brief      Customization helper functions for Anzu Native SDK.
 *
 * @details    This "C" header file contains the API allowing a game engine to customize Anzu SDK default behaviour.
 */

//----------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include "Anzu.h"
#include <cstdint>

/**
 * @brief Magic number base for all anzu internal API calls.
 */
#define INTERNAL_DEBUGGING_PARAM_BASE       0xC0DE5AFE

/**
* @fn       void (*EngineLoggerCallback_t)(const char * logmsg)
*
* @brief    Log function signature.
*
* @param    logmsg log line to be printed.
*
* @return   none.
*/
typedef void (ANZU_CALLCONV *EngineLoggerCallback_t)(const char * logmsg);

/**
* @fn       void* (*CustomMallocImpl_t)(size_t size)
*
* @brief    malloc API signature.
*
* @param    size number of bytes to allocate.
*
* @return   void* address of allocated memory or NULL if failed allocating.
*/
typedef void* (ANZU_CALLCONV *CustomMallocImpl_t)( size_t size );

/**
* @fn       void (*CustomFreeImpl_t)(void * ptr)
*
* @brief    free API signature.
*
* @param    ptr memory base address to free.
*
* @return   none.
*/
typedef void (ANZU_CALLCONV *CustomFreeImpl_t)( void * ptr );

/**
* @fn       void* (*CustomReallocImpl_t)(void * ptr, size_t size)
*
* @brief    realloc API signature.
*
* @param    ptr old allocation base address.
* @param    size number of bytes to allocate re-allocate.
*
* @return   void* address of allocated memory or NULL if failed reallocating.
*/
typedef void* (ANZU_CALLCONV *CustomReallocImpl_t)( void * ptr, size_t size );

/**
 * @brief Web-host action type.
 */
typedef enum {
    WHA_LOAD = 0,
    WHA_EVAL = 1,
    WHA_CLOSE = 2,
    WHA_HTML = 3
} WebHostAction_t;

typedef void (ANZU_CALLCONV *CustomWebHostImpl_t)( WebHostAction_t action, const char * code );

typedef void (ANZU_CALLCONV *CustomDownloadApprovalRespond_t)( int id, bool approva );
typedef void (ANZU_CALLCONV *CustomDownloadApprovalCallback_t)( int id, const char *url, const char *outPath );

typedef void (ANZU_CALLCONV *HttpDownloadCallback_t)( void * userData, int responseCode, const char * error );
typedef void (ANZU_CALLCONV *HttpRequestCallback_t)( void * userData, bool success, const char * response );
typedef void (ANZU_CALLCONV *HttpDownloadCancelCallback_t)( void * userData );
typedef void (ANZU_CALLCONV *HttpRequestCancelCallback_t)( void * userData );

typedef void (ANZU_CALLCONV *HttpDownloadStatsCallback_t)( const char * url, float downloadTime );

typedef void (ANZU_CALLCONV *MediaPoolCallback_t)( const char * originUrl, const char * filePath, unsigned long fileSize, const char * sha256 );

typedef void (ANZU_CALLCONV *CustomHttpRequest_t)( const char * url, const char * userAgent, bool isPost, const char * postData, HttpRequestCallback_t finishCB, void * userData, const char * contentType, bool ignoreReply );
typedef void (ANZU_CALLCONV *CustomHttpDownload_t)( const char * url, const char * userAgent, const char * outPath, HttpDownloadCallback_t finishCB, void * userData );
typedef void (ANZU_CALLCONV *CustomCancelAllWebRequests_t)();

typedef void (ANZU_CALLCONV *LogicPerformanceQueryFn_t)( long * numAllocs, long * numAllocsMax, long * bytesAllocd, long * bytesAllocdMax );

/**
 * @brief This can be used to control affinity of threads.
 */
typedef enum {
    LOGIC_PROCESSOR = 0,
    PNG_DECODER = 1,
    THEORA_DECODER = 2,
    THEORA_DECODER_STREAMER = 3,
    HTTPX_TASK = 4,
    EVENT_REPORTER = 5,
    GENERAL_SCHEDULER = 6,
    STB_IMAGE_DECODER = 7,
    DETEX_DECODER = 8,
    NATIVE_VIDEO_DECODER = 9,
    SOCKET_DEBUG_LOGGER = 10,
    LOGIC_CALLBACK = 11,
    NATIVE_VIDEO_PLAYER_DECODER = 12,       //in PS4 this is what we set in AVPlayer init structure -> videoDecoderAffinity
    NATIVE_VIDEO_PLAYER_DEMUXER = 13,       //in PS4 this is what we set in AVPlayer init structure -> demuxerAffinity
    NATIVE_VIDEO_PLAYER_CONTROLLER = 14,    //in PS4 this is what we set in AVPlayer init structure -> controllerAffinity
    NATIVE_VIDEO_PLAYER_FILE_STREAM = 15,   //in PS4 this is what we set in AVPlayer init structure -> fileStreamingAffinity
    WEB_VIEW = 16,
    DEBUG_SERVICES = 17,
    DISCOVERY_BROADCAST = 18,               //only available on logic debug enabled clients, works on android and some ios devices (depending on version on platform)
} AnzuThreadType_t;

typedef unsigned int (ANZU_CALLCONV *CustomThreadsAffinityCallback_t)( AnzuThreadType_t threadType );

typedef int (ANZU_CALLCONV *PlatformGPUMemoryAllocator_t)( void ** outPtr, unsigned int memSize, unsigned int memAlignment );
typedef int (ANZU_CALLCONV *PlatformGPUMemoryReleaser_t)( void * inPtr, uint32_t size );
typedef int (ANZU_CALLCONV *PlatformSharedMemoryAllocator_t)( void ** outPtr, unsigned int memSize, unsigned int memAlignment );
typedef int (ANZU_CALLCONV *PlatformSharedMemoryReleaser_t)( void * inPtr, uint32_t size );

typedef void (ANZU_CALLCONV *CustomOpenUrlImpl_t)( const char * url );

/**
 * @brief Each of the internal functions is represented by this enum.
 */
typedef enum {
    IDPK__CUSTOM_L1_ADDRESS								= (INTERNAL_DEBUGGING_PARAM_BASE+0),    ///< parmData is char *
    IDPK__ALLOW_ENGINE_LOAD_FROM_DISK					= (INTERNAL_DEBUGGING_PARAM_BASE+1),    ///< parmData is bool
    IDPK__DISABLE_IMAGE_DECODE_TO_MEMORY				= (INTERNAL_DEBUGGING_PARAM_BASE+2),    ///< parmData is bool
    IDPK__ENABLE_RESCALE_TO_TARGET						= (INTERNAL_DEBUGGING_PARAM_BASE+3),    ///< parmData is bool
    IDPK__DISABLE_GPU_DIRECT_UPDATES					= (INTERNAL_DEBUGGING_PARAM_BASE+4),    ///< parmData is bool
    IDPK__DISABLE_MQTT_CLIENT							= (INTERNAL_DEBUGGING_PARAM_BASE+5),    ///< parmData is bool
    IDPK__CUSTOM_THREAD_AFFINITY                    	= (INTERNAL_DEBUGGING_PARAM_BASE+6),    ///< parmData is callback with CustomThreadsAffinityCallback_t signature
    IDPK__LOG_TO_FILE									= (INTERNAL_DEBUGGING_PARAM_BASE+7),    ///< parmData is char *
    IDPK__ENABLE_DECODE_TO_TEXTURE						= (INTERNAL_DEBUGGING_PARAM_BASE+8),    ///< parmData is bool
    IDPK__ENGINE_LOGGER_CALLBACK						= (INTERNAL_DEBUGGING_PARAM_BASE+9),    ///< parmData is callback with EngineLoggerCallback_t signature
    IDPK__APPLICATION_BINARY_VERSION					= (INTERNAL_DEBUGGING_PARAM_BASE+10),   ///< parmData is char *
    IDPK__MAX_WEB_THREAD_POOL_SIZE						= (INTERNAL_DEBUGGING_PARAM_BASE+11),   ///< parmData is int
    IDPK__USE_OPT_WEBHOST								= (INTERNAL_DEBUGGING_PARAM_BASE+12),   ///< parmData is bool
    IDPK__ENABLE_HIGH_QUALITY_PVRTC_RESCALE				= (INTERNAL_DEBUGGING_PARAM_BASE+13),   ///< parmData is bool
    IDPK__MAX_PVRTC_SIZE								= (INTERNAL_DEBUGGING_PARAM_BASE+14),   ///< parmData is int
    IDPK__DETECT_REAL_USER_AGENT						= (INTERNAL_DEBUGGING_PARAM_BASE+15),   ///< parmData is bool
    IDPK__TEXTURE_SCALE_MODE							= (INTERNAL_DEBUGGING_PARAM_BASE+16),   ///< parmData is int
    IDPK__DETEX_ENABLE_LOAD_MIPMAPS						= (INTERNAL_DEBUGGING_PARAM_BASE+17),   ///< parmData is bool
    IDPK__WRITABLE_FOLDER_LOCATION						= (INTERNAL_DEBUGGING_PARAM_BASE+18),   ///< parmData is char *
    IDPK__CACHE_EXPIRATION_SIZE							= (INTERNAL_DEBUGGING_PARAM_BASE+19),   ///< parmData is int
    IDPK__LOG_TO_SOCKET_ADDRESS							= (INTERNAL_DEBUGGING_PARAM_BASE+20),   ///< parmData is char *
    IDPK__CUSTOM_MALLOC									= (INTERNAL_DEBUGGING_PARAM_BASE+21),   ///< parmData is callback with CustomMallocImpl_t signature (same as malloc)
    IDPK__CUSTOM_FREE									= (INTERNAL_DEBUGGING_PARAM_BASE+22),   ///< parmData is callback with CustomFreeImpl_t signature (same as free)
    IDPK__CUSTOM_REALLOC								= (INTERNAL_DEBUGGING_PARAM_BASE+23),   ///< parmData is callback with CustomReallocImpl_t signature (same as realloc)
    IDPK__ADD_SUPPORTED_IMAGE_MIME_TYPE					= (INTERNAL_DEBUGGING_PARAM_BASE+24),   ///< parmData is char *
    IDPK__ADD_SUPPORTED_IMAGE_FILE_EXTENTION			= (INTERNAL_DEBUGGING_PARAM_BASE+25),   ///< parmData is char *
    IDPK__CACHE_EXPIRATION_SECONDS						= (INTERNAL_DEBUGGING_PARAM_BASE+26),   ///< parmData is int
    IDPK__CLEAN_CACHE_ON_SHUTDOWN						= (INTERNAL_DEBUGGING_PARAM_BASE+27),   ///< parmData is bool
    IDPK__SECURE_HTTPS_ONLY								= (INTERNAL_DEBUGGING_PARAM_BASE+28),   ///< parmData is bool
    IDPK__ENABLE_LOCATION_SERVICES						= (INTERNAL_DEBUGGING_PARAM_BASE+29),   ///< parmData is bool
    IDPK__GARBAGE_COLLECTION_INTERVAL					= (INTERNAL_DEBUGGING_PARAM_BASE+30),   ///< parmData is int
    IDPK__DISABLE_NET_EVENTS							= (INTERNAL_DEBUGGING_PARAM_BASE+31),   ///< parmData is bool
    IDPK__ENABLE_LOGIC_CACHE							= (INTERNAL_DEBUGGING_PARAM_BASE+32),   ///< parmData is bool
    IDPK__ENABLE_PERSISTENT_UDID						= (INTERNAL_DEBUGGING_PARAM_BASE+33),   ///< parmData is bool
    IDPK__ENABLE_STAGING_CONFIG							= (INTERNAL_DEBUGGING_PARAM_BASE+34),   ///< parmData is bool
    IDPK__CUSTOM_DOWNLOAD_APPROVAL						= (INTERNAL_DEBUGGING_PARAM_BASE+35),   ///< parmData is callback with CustomDownloadApprovalCallback_t signature, returns the decision making callback
    IDPK__CUSTOM_SERVER_ACCESS_INITIATION				= (INTERNAL_DEBUGGING_PARAM_BASE+36),   ///< parmData is 0: disabled (default), 1: enabled, 2:issue the request
    IDPK__CUSTOM_HTTP_REQUEST							= (INTERNAL_DEBUGGING_PARAM_BASE+37),   ///< parmData is callback with CustomHttpRequest_t signature
    IDPK__CUSTOM_HTTP_DOWNLOAD							= (INTERNAL_DEBUGGING_PARAM_BASE+38),   ///< parmData is callback with CustomHttpDownload_t signature
    IDPK__CUSTOM_CANCEL_ALL_WEB_REQUESTS				= (INTERNAL_DEBUGGING_PARAM_BASE+39),   ///< parmData is callback with CustomCancelAllWebRequests_t signature
    IDPK__ADD_SUPPORTED_VIDEO_MIME_TYPE					= (INTERNAL_DEBUGGING_PARAM_BASE+40),   ///< parmData is char *
    IDPK__ADD_SUPPORTED_VIDEO_FILE_EXTENTION			= (INTERNAL_DEBUGGING_PARAM_BASE+41),   ///< parmData is char *
    IDPK__SET_CUSTOM_VIDEO_PLAYBACK_PROGRESS			= (INTERNAL_DEBUGGING_PARAM_BASE+42),   ///< parmData is CustomVideoPlayerProgress_t *
    IDPK__SET_CUSTOM_PLATFORM_GPU_MEMORY_ALLOCATOR		= (INTERNAL_DEBUGGING_PARAM_BASE+43),   ///< parmData is a pointer to external GPU memory allocator function.
    IDPK__SET_CUSTOM_PLATFORM_GPU_MEMORY_RELEASER		= (INTERNAL_DEBUGGING_PARAM_BASE+44),   ///< parmData is a pointer to external GPU memory releaser function.
    IDPK__SET_CUSTOM_PLATFORM_SHARED_MEMORY_ALLOCATOR	= (INTERNAL_DEBUGGING_PARAM_BASE+45),   ///< parmData is a pointer to external shared (accessible both by CPU and GPU) memory allocator function.
    IDPK__SET_CUSTOM_PLATFORM_SHARED_MEMORY_RELEASER	= (INTERNAL_DEBUGGING_PARAM_BASE+46),   ///< parmData is a pointer to external shared (accessible both by CPU and GPU) memory releaser function.
    IDPK__SET_REPORTING_AGGREGATION_INTERVAL            = (INTERNAL_DEBUGGING_PARAM_BASE+47),   ///< parmData is unsigned int
    IDPK__HTTP_DOWNLOAD_STATS                           = (INTERNAL_DEBUGGING_PARAM_BASE+48),   ///< parmData is callback with HttpDownloadStatsCallback_t
    IDPK__MIN_REQUIRED_FREE_SPACE                       = (INTERNAL_DEBUGGING_PARAM_BASE+49),   ///< parmData is int
    IDPK__CUSTOM_OPENURL                                = (INTERNAL_DEBUGGING_PARAM_BASE+50),   ///< parmData is callback with CustomOpenUrlImpl_t signature
    IDPK__MEDIA_POOL_INFO                               = (INTERNAL_DEBUGGING_PARAM_BASE+51),   ///< parmData is callback with MediaPoolCallback_t signature
    IDPK__LOGIC_DEBUG                                   = (INTERNAL_DEBUGGING_PARAM_BASE+52),   ///< parmData is char *
    IDPK__CUSTOM_WEBHOST                                = (INTERNAL_DEBUGGING_PARAM_BASE+53),   ///< parmData is callback with CustomWebHostImpl_t signature
    IDPK__ADD_CUSTOM_OPENURL_SCHEME                     = (INTERNAL_DEBUGGING_PARAM_BASE+54),   ///< parmData is char *
    IDPK__GET_LOGIC_PERFORMANCE_QUERY_FN                = (INTERNAL_DEBUGGING_PARAM_BASE+55),   ///< parmData is a pointer to receive the LogicPerformanceQueryFn_t API entrypoint
    IDPK__ENABLE_JSON_BEAUTIFY                          = (INTERNAL_DEBUGGING_PARAM_BASE+56),   ///< parmData is bool
    IDPK__DISABE_REPORTING_APP_ACTIVATION_STATUS        = (INTERNAL_DEBUGGING_PARAM_BASE+57),   ///< parmData is bool
    IDPK__ENABLE_STARTUP_PROCEDURE_WHEN_REACTIVATING    = (INTERNAL_DEBUGGING_PARAM_BASE+58),   ///< parmData is bool
    IDPK__ENABLE_WEBHOST_DEBUG                          = (INTERNAL_DEBUGGING_PARAM_BASE+59),   ///< parmData is bool
    IDPK__IMPERSONATED_PLATFORM                         = (INTERNAL_DEBUGGING_PARAM_BASE+60),   ///< parmData is char *
    IDPK__DISABLE_ANDROID_CUSTOM_TABS			        = (INTERNAL_DEBUGGING_PARAM_BASE+61),   ///< parmData is bool
    } InternalDebuggingParmKey_t;

/**
 * @brief Structure expected for custom video decoder progress reporting
 * supported by `IDPK__SET_CUSTOM_VIDEO_PLAYBACK_PROGRESS`
 */
#pragma pack(push, 4)
typedef struct {
    int channelId;          ///< the channel ID the playback applies to
    float totalDuration;    ///< total clip duration in seconds
    float currentPosition;  ///< current progress in seconds
} CustomVideoPlayerProgress_t;
#pragma pack(pop)

/**
 * @brief Commands supported by `IDPK__CUSTOM_SERVER_ACCESS_INITIATION`
 */
typedef enum {
    CSAIC_OFF = 0,          ///< Avoid issuing server sync requests
    CSAIC_ON = 1,           ///< Enable automatic issuing of server sync requests (default)
    CSAIC_INITIATE = 2      ///< Attemt server sync now
} CustomServerAccessInititationCommand_t;

#undef INTERNAL_DEBUGGING_PARAM_BASE

/**
 * @brief Business logic no longer runs inside a webview client,
 * we use a lightweight javascript engine instead
 */
#define Anzu_Internal_SetUseAltJavascriptEngine(__X__) /* deprecated, now always uses alt javascript engine */

static inline void Anzu_Internal_SetCustomL1Address( const char * l1addr ) {
    Anzu_InternalDebugging( (void *)IDPK__CUSTOM_L1_ADDRESS, const_cast<char*>(l1addr) );
}

/**
 * Media file is exposed in the `texture::info` message data.
 *
 * @param allow
 */
static inline void Anzu_Internal_SetAllowEngineLoadFromDisk( bool allow ) {
    Anzu_InternalDebugging( (void *)IDPK__ALLOW_ENGINE_LOAD_FROM_DISK, (void *)(unsigned long long)(allow?1:0) );
}

static inline void Anzu_Internal_SetDisableImageDecodeToMemory( bool disable ) {
    Anzu_InternalDebugging( (void *)IDPK__DISABLE_IMAGE_DECODE_TO_MEMORY, (void *)(unsigned long long)(disable?1:0) );
}

static inline void Anzu_Internal_SetEnableRescaleToTarget( bool enable ) {
    Anzu_InternalDebugging( (void *)IDPK__ENABLE_RESCALE_TO_TARGET, (void *)(unsigned long long)(enable?1:0) );
}

static inline void Anzu_Internal_SetDisableGpuDirectUpdates( bool disable ) {
    Anzu_InternalDebugging( (void *)IDPK__DISABLE_GPU_DIRECT_UPDATES, (void *)(unsigned long long)(disable?1:0) );
}

/**
 * Disables the MQTT client listener.
 *
 * @param disable
 */
static inline void Anzu_Internal_SetDisableMqttClient( bool disable ) {
    Anzu_InternalDebugging( (void *)IDPK__DISABLE_MQTT_CLIENT, (void *)(unsigned long long)(disable?1:0) );
}

static inline void Anzu_Internal_SetCustomThreadAffinityCallback( CustomThreadsAffinityCallback_t callback ) {
    Anzu_InternalDebugging( (void *)IDPK__CUSTOM_THREAD_AFFINITY, (void *)callback );
}

static inline void Anzu_Internal_SetLogToFile( const char * filename ) {
    Anzu_InternalDebugging( (void *)IDPK__LOG_TO_FILE, const_cast<char*>(filename) );
}

static inline void Anzu_Internal_SetEnableDecodeToTexture( bool enable ) {
    Anzu_InternalDebugging( (void *)IDPK__ENABLE_DECODE_TO_TEXTURE, (void *)(unsigned long long)(enable?1:0) );
}

static inline void Anzu_Internal_SetEngineLoggerCallback( EngineLoggerCallback_t callback ) {
    Anzu_InternalDebugging( (void *)IDPK__ENGINE_LOGGER_CALLBACK, (void *)callback );
}

static inline void Anzu_Internal_SetApplicationBinaryVersion( const char * version ) {
    Anzu_InternalDebugging( (void *)IDPK__APPLICATION_BINARY_VERSION, const_cast<char*>(version) );
}

/**
 * Set maximum number of threads used to make web requests.
 *
 * @param size
 */
static inline void Anzu_Internal_SetMaxWebThreadPoolSize( int size ) {
    Anzu_InternalDebugging( (void *)IDPK__MAX_WEB_THREAD_POOL_SIZE, (void*)(unsigned long long)size );
}

static inline void Anzu_Internal_SetUseOptWebHost( bool useopt ) {
    Anzu_InternalDebugging( (void *)IDPK__USE_OPT_WEBHOST, (void *)(unsigned long long)(useopt?1:0) );
}

static inline void Anzu_Internal_SetEnableHighQualityPvrtcRescale( bool enable ) {
    Anzu_InternalDebugging( (void *)IDPK__ENABLE_HIGH_QUALITY_PVRTC_RESCALE, (void *)(unsigned long long)(enable?1:0) );
}

static inline void Anzu_Internal_SetMaxPvrtcSize( int size ) {
    Anzu_InternalDebugging( (void *)IDPK__MAX_PVRTC_SIZE, (void*)(unsigned long long)size );
}

static inline void Anzu_Internal_SetEnableDetectRealUserAgent( bool enable ) {
    Anzu_InternalDebugging( (void *)IDPK__DETECT_REAL_USER_AGENT, (void *)(unsigned long long)(enable?1:0) );
}

static inline void Anzu_Internal_SetTextureScaleMode( int mode ) {
    Anzu_InternalDebugging( (void *)IDPK__TEXTURE_SCALE_MODE, (void*)(unsigned long long)mode );
}

static inline void Anzu_Internal_SetEnableDetexLoadMipmaps( bool enable ) {
    Anzu_InternalDebugging( (void *)IDPK__DETEX_ENABLE_LOAD_MIPMAPS, (void *)(unsigned long long)(enable?1:0) );
}

/**
 * Overrides the data folder of the Anzu SDK.
 *
 * @param folder
 */
static inline void Anzu_Internal_SetWritableFolderLocation( const char * folder ) {
    Anzu_InternalDebugging( (void *)IDPK__WRITABLE_FOLDER_LOCATION, const_cast<char*>(folder) );
}

static inline void Anzu_Internal_SetCacheExpirationSize( int size ) {
    Anzu_InternalDebugging( (void *)IDPK__CACHE_EXPIRATION_SIZE, (void*)(unsigned long long)size );
}

static inline void Anzu_Internal_SetMinRequiredFreeSpace( int size ) {
    Anzu_InternalDebugging( (void *)IDPK__MIN_REQUIRED_FREE_SPACE, (void*)(unsigned long long)size );
}

static inline void Anzu_Internal_SetLogToSocketAddress( const char * addr ) {
    Anzu_InternalDebugging( (void *)IDPK__LOG_TO_SOCKET_ADDRESS, const_cast<char*>(addr) );
}

static inline void Anzu_Internal_LogicDebug( const char * code ) {
    Anzu_InternalDebugging( (void *)IDPK__LOGIC_DEBUG, const_cast<char*>(code) );
}

static inline void Anzu_Internal_ImpersonatedPlatform( const char * name ) {
    Anzu_InternalDebugging( (void *)IDPK__IMPERSONATED_PLATFORM, const_cast<char*>(name) );
}

static inline void Anzu_Internal_SetCustomMallocHandler( CustomMallocImpl_t handler ) {
    Anzu_InternalDebugging( (void *)IDPK__CUSTOM_MALLOC, (void *)handler );
}

static inline void Anzu_Internal_SetCustomFreeHandler( CustomFreeImpl_t handler ) {
    Anzu_InternalDebugging( (void *)IDPK__CUSTOM_FREE, (void *)handler );
}

static inline void Anzu_Internal_SetCustomReallocHandler( CustomReallocImpl_t handler ) {
    Anzu_InternalDebugging( (void *)IDPK__CUSTOM_REALLOC, (void *)handler );
}

static inline void Anzu_Internal_SetCustomOpenUrlHandler( CustomOpenUrlImpl_t handler ) {
    Anzu_InternalDebugging( (void *)IDPK__CUSTOM_OPENURL, (void *)handler );
}

static inline void Anzu_Internal_SetDisableAndroidCustomTabs( bool disable ) {
    Anzu_InternalDebugging( (void *)IDPK__DISABLE_ANDROID_CUSTOM_TABS, (void *)(unsigned long long)(disable?1:0) );
}

/**
 * Adding support additional image files by mime-type.
 *
 * @param type
 */
static inline void Anzu_Internal_AddSupportedImageMimeType( const char * type ) {
    Anzu_InternalDebugging( (void *)IDPK__ADD_SUPPORTED_IMAGE_MIME_TYPE, const_cast<char*>(type) );
}

/**
 * Adding support additional image files by file extension.
 *
 * @param ext
 */
static inline void Anzu_Internal_AddSupportedImageFileExtention( const char * ext ) {
    Anzu_InternalDebugging( (void *)IDPK__ADD_SUPPORTED_IMAGE_FILE_EXTENTION, const_cast<char*>(ext) );
}

/**
 * Adding support additional video files by mime-type.
 *
 * @param type
 */
static inline void Anzu_Internal_AddSupportedVideoMimeType( const char * type ) {
    Anzu_InternalDebugging( (void *)IDPK__ADD_SUPPORTED_VIDEO_MIME_TYPE, const_cast<char*>(type) );
}

/**
 * Adding support additional video files by file extension.
 *
 * @param ext
 */
static inline void Anzu_Internal_AddSupportedVideoFileExtention( const char * ext ) {
    Anzu_InternalDebugging( (void *)IDPK__ADD_SUPPORTED_VIDEO_FILE_EXTENTION, const_cast<char*>(ext) );
}

/**
 * Used to report progress and duration of video back to the SDK.
 *
 * @param pgs
 */
static inline void  Anzu_Internal_SetCustomVideoPlaybackProgress( CustomVideoPlayerProgress_t * pgs ) {
    Anzu_InternalDebugging( (void *)IDPK__SET_CUSTOM_VIDEO_PLAYBACK_PROGRESS, (void *)pgs );
}

static inline void Anzu_Internal_SetCacheExpirationSeconds( int secs ) {
    Anzu_InternalDebugging( (void *)IDPK__CACHE_EXPIRATION_SECONDS, (void*)(unsigned long long)secs );
}

static inline void Anzu_Internal_SetCleanCacheOnShutdown( bool enable ) {
    Anzu_InternalDebugging( (void *)IDPK__CLEAN_CACHE_ON_SHUTDOWN, (void *)(unsigned long long)(enable?1:0) );
}

static inline void Anzu_Internal_SetPreventNonSecureHttpCalls( bool enable ) {
    Anzu_InternalDebugging( (void *)IDPK__SECURE_HTTPS_ONLY, (void *)(unsigned long long)(enable?1:0) );
}

static inline void Anzu_Internal_SetEnableLocationServices( bool enable ) {
    Anzu_InternalDebugging( (void *)IDPK__ENABLE_LOCATION_SERVICES, (void *)(unsigned long long)(enable?1:0) );
}

static inline void Anzu_Internal_SetGarbageCollectionInterval( int ms ) {
    Anzu_InternalDebugging( (void *)IDPK__GARBAGE_COLLECTION_INTERVAL, (void*)(unsigned long long)ms );
}

static inline void Anzu_Internal_SetDisableNetEvents(bool disable) {
    Anzu_InternalDebugging( (void *)IDPK__DISABLE_NET_EVENTS, (void *)(unsigned long long)(disable ? 1 : 0) );
}

static inline void Anzu_Internal_SetEnableLogicCaching(bool enable) {
    Anzu_InternalDebugging( (void *)IDPK__ENABLE_LOGIC_CACHE, (void *)(unsigned long long)(enable ? 1 : 0) );
}

static inline void Anzu_Internal_SetEnablePersistentUDID(bool enable) {
    Anzu_InternalDebugging( (void *)IDPK__ENABLE_PERSISTENT_UDID, (void *)(unsigned long long)(enable ? 1 : 0) );
}

static inline void Anzu_Internal_SetEnableStagingConfig(bool enable) {
    Anzu_InternalDebugging( (void *)IDPK__ENABLE_STAGING_CONFIG, (void *)(unsigned long long)(enable ? 1 : 0) );
}

static inline CustomDownloadApprovalRespond_t Anzu_Internal_SetCustomDownloadApproval( CustomDownloadApprovalCallback_t customHandler ) {
    return (CustomDownloadApprovalRespond_t)Anzu_InternalDebugging( (void *)IDPK__CUSTOM_DOWNLOAD_APPROVAL, (void *)customHandler );
}

static inline void Anzu_Internal_CustomServerAccessInitiate( CustomServerAccessInititationCommand_t cmd ) {
    Anzu_InternalDebugging( (void *)IDPK__CUSTOM_SERVER_ACCESS_INITIATION, (void *)(unsigned long long)cmd );
}

static inline void Anzu_Internal_SetCustomHttpRequest( CustomHttpRequest_t customHandler ) {
    Anzu_InternalDebugging( (void *)IDPK__CUSTOM_HTTP_REQUEST, (void *)customHandler );
}

static inline void Anzu_Internal_SetCustomHttpDownload( CustomHttpDownload_t customHandler ) {
    Anzu_InternalDebugging( (void *)IDPK__CUSTOM_HTTP_DOWNLOAD, (void *)customHandler );
}

static inline void Anzu_Internal_SetCustomCancelAllWebRequests( CustomCancelAllWebRequests_t customHandler ) {
    Anzu_InternalDebugging( (void *)IDPK__CUSTOM_CANCEL_ALL_WEB_REQUESTS, (void *)customHandler );
}

static inline void Anzu_Internal_SetCustomPlatformMemoryAllocator ( PlatformGPUMemoryAllocator_t *allocator ) {
    Anzu_InternalDebugging( (void*)IDPK__SET_CUSTOM_PLATFORM_GPU_MEMORY_ALLOCATOR, (void*)allocator );
}

static inline void Anzu_Internal_SetCustomPlatformMemoryReleaser( PlatformGPUMemoryReleaser_t* releaser ) {
    Anzu_InternalDebugging( (void*)IDPK__SET_CUSTOM_PLATFORM_GPU_MEMORY_RELEASER, (void*)releaser );
}

static inline void Anzu_Internal_SetCustomPlatformSharedMemoryAllocator( PlatformSharedMemoryAllocator_t allocator ) {
    Anzu_InternalDebugging( (void*)IDPK__SET_CUSTOM_PLATFORM_SHARED_MEMORY_ALLOCATOR, (void*)allocator );
}

static inline void Anzu_Internal_SetCustomPlatformSharedMemoryReleaser( PlatformSharedMemoryReleaser_t releaser ) {
    Anzu_InternalDebugging( (void*)IDPK__SET_CUSTOM_PLATFORM_SHARED_MEMORY_RELEASER, (void*)releaser );
}

static inline void Anzu_Internal_SetReportingAggregationInterval( unsigned int ms ) {
    Anzu_InternalDebugging( (void*)IDPK__SET_REPORTING_AGGREGATION_INTERVAL, (void*)(unsigned long long)ms );
}

static inline void Anzu_Internal_SetHttpDownloadStatsCallback( HttpDownloadStatsCallback_t callback ) {
    Anzu_InternalDebugging( (void *)IDPK__HTTP_DOWNLOAD_STATS, (void *)callback );
}

static inline void Anzu_Internal_SetMediaPoolCallback( MediaPoolCallback_t callback ) {
    Anzu_InternalDebugging( (void *)IDPK__MEDIA_POOL_INFO, (void *)callback );
}

static inline void Anzu_Internal_SetCustomWebHost( CustomWebHostImpl_t customHandler ) {
    Anzu_InternalDebugging( (void *)IDPK__CUSTOM_WEBHOST, (void *)customHandler );
}

static inline void Anzu_Internal_AddCustomOpenUrlScheme( const char * scheme ) {
    Anzu_InternalDebugging( (void *)IDPK__ADD_CUSTOM_OPENURL_SCHEME, const_cast<char*>(scheme) );
}

static inline void Anzu_Internal_GetLogicPerformanceQueryFn( LogicPerformanceQueryFn_t * outFn ) {
    Anzu_InternalDebugging( (void *)IDPK__GET_LOGIC_PERFORMANCE_QUERY_FN, (void *)outFn );
}

/**
 * Enables pretty JSON messages.
 *
 * @param enable
 */
static inline void Anzu_Internal_SetEnableJsonBeautify( bool enable ) {
    Anzu_InternalDebugging( (void *)IDPK__ENABLE_JSON_BEAUTIFY, (void *)(unsigned long long)(enable ? 1 : 0) );
}

/**
 * Enables WebHost debugging.
 *
 * @param enable
 */
static inline void Anzu_Internal_SetEnableWebhostDebug( bool enable ) {
    Anzu_InternalDebugging( (void *)IDPK__ENABLE_WEBHOST_DEBUG, (void *)(unsigned long long)(enable ? 1 : 0) );
}
