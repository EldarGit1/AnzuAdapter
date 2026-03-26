//----------------------------------------------------------------------------------------------------------------------------------

/** @file       Anzu.h
  * @author     Micahel Badichi, anzu.io.
  *
  * @brief      Main header file for Anzu Native SDK.
  *
  * @details    This "C" header file contains the API allowing a game engine to integrate with Anzu SDK.
  *             All function symbols in this file are exported when creating shared libraries of the native SDK.
  */

//----------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include <stdio.h>

#ifndef _WINDOWS
#define _WINDOWS 1
#endif

/** Platform specific calling convention. */
#if defined(_WINDOWS)
#   define ANZU_CALLCONV __cdecl
#elif defined(ANDROID) || defined(__ANDROID__)
#   ifdef __arm__
#       define ANZU_CALLCONV
#   else
#       define ANZU_CALLCONV __cdecl
#   endif
#elif defined(ORBIS) || defined(PROSPERO)
#   define ANZU_CALLCONV __attribute__((cdecl))
#elif defined(__APPLE__)
#   import "TargetConditionals.h"
#   if TARGET_OS_IOS
#       define ANZU_CALLCONV /* __attribute__((cdecl)) */
#   else
#       define ANZU_CALLCONV __attribute__((cdecl))
#   endif
#else
#   define ANZU_CALLCONV
#endif

/** Defines that external linkage is applied. */
#ifdef __cplusplus
#define EXTN extern "C"
#else
#define EXTN
#endif

/** Defines that method is exported in shared library. */
#ifdef ANZU_STATIC
#   define EXPORT
#else
#   if defined(_WINDOWS) || defined(ORBIS) || defined(_DURANGO) || defined(PROSPERO)
#       define EXPORT __declspec(dllexport)
#   else
#       define EXPORT __attribute__((visibility("default")))
#   endif
#endif

//----------------------------------------------------------------------------------------------------------------------------------

/** Defines current SDK version. */
#define ANZU_SDK_VERSION         "6.48"

/** Defines the max supported JSON messaging protocol version. */
#define ANZU_PROTOCOL_VERSION    "1.0"

//----------------------------------------------------------------------------------------------------------------------------------

// IF you want to exclude anzu functionality and implementation for a specific target, you can create a dummy implementation
// by creating a c/cpp source with the following code:
//   #define ANZU_DUMMY
//   #include "Anzu.h"

#ifndef ANZU_DUMMY
    #define ANZU_DUMMY_IMP(...)
#else
    #define ANZU_DUMMY_IMP(...) {return __VA_ARGS__;}
#endif

//----------------------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Operation to perform on numeric or string metrics.
 *
 * @details Allows applying computation on metrics using Anzu_MetricSet, Anzu_MetricSetS, Anzu_SystemMetricSet and
 *          Anzu_SystemMetricSetS.
 *          Metrics allows to create complex campaign funnels, improve targeting, rewards, and many other cool stuff.
 *          Metrics values are persistent.
 *          Using empty keys is not allowed, and will not be stored.
 */
typedef enum  {
    VMO_SET  = 0,   ///< For numeric metrics, Just set the value as is.
    VMO_ADD  = 1,   ///< For numeric metrics, Add the value to the existing value.
    VMO_SUB  = 2,   ///< For numeric metrics, Subtract the value from the existing value.
    VMO_AVG  = 3,   ///< For numeric metrics, Account the value, when metric is queried, the result will be the average of all values ever accounted.
    VMO_STR  = 4,   ///< For string metrics, Store the provided string.
    VMO_DATE = 5    ///< For numeric metrics, Store the date as Epoch, if value is 'NOW' the current time is stored, if value is 'TODAY' the time at the beginning of current day is stored.
} Anzu_MetricOperation_t;

/**
 * @brief   Hash algorithms to use with Hashed - Metrics.
 */
typedef enum {
    MHA_SHA1 = 0,   ///< SHA-1 hash
    MHA_SHA256 = 1  ///< SHA-256 hash
} Anzu_MetricHashAlgorithm_t;

/**
 * @brief   Bit-fields specifying the characteristics of a channel.
 *
 * @details These fields specify the nature of the channel, what media types can he accept and whether he has a default fallback
 *          media, whether it can change during session, the desired texture compression format.
 */
typedef enum {
    VTP_LOGO        = 1<<0,                                     ///< This channel can accept logo images (with alpha channel), logo channels are supposed to blend over other textures, and are commonly static.
    VTP_IMAGE       = 1<<1,                                     ///< This channel can accept image content.
    VTP_VIDEO       = 1<<2,                                     ///< This channel can accept video content.
    VTP_WEB         = 1<<3,                                     ///< This channel can accept web content.
    VTP_INTERACTIVE = 1<<4,                                     ///< This channel supports interactive content.
    VTP_STATIC      = 1<<5,                                     ///< Once channel plays a media, no other media will be assigned to this channel during his lifetime, this is designed for posters (non-digital billboards),
    VTP_SUPPORTS_RB = 1<<6,                                     ///< Some platform decoders have their red-blue colors swapped, in order to provide proper buffers, the SDK might swizzle them for you, however, if your shader supports RB swizzling, setting this field avoid the SDK doing the swizzling at the 1st place. in case shader swizzling is required, the texture:info will let you know.
    VTP_GENERATED   = 1<<7,                                     ///< Machine generated channel names should have this bit set, it prevents the ad server from 'remembering' the name and showing it in dashboard selection boxes.
    VTP_HAS_FALLBACK= 1<<8,                                     ///< Channels that have fallback media prepared in the case no ad is available, should set this flag.
    //format, uses bits 9,10,11,12
    VTP_FORMAT_MASK = (1<<9) | (1<<10) | (1<<11) | (1<<12),     ///< Mask for the texture compression format expected for this channel.
    VTP_RGBA8888    = 0<<9,                                     ///< Use 32bpp uncompressed textures 8 bit per component, RGBA. (logos need this format).
    VTP_DXT1        = 1<<9,                                     ///< Use 4bpp BC1 (DXT1), no alpha channel, very common on desktops and consoles. (commonly used for images).
    VTP_RGB565      = 2<<9,                                     ///< Use 16bpp RGB 5:6:5, (commonly used for videos or video+image combinations).
    VTP_ETC1        = 3<<9,                                     ///< Use 4bpp ETC1, no alpha channel, common on android devices, (commonly used for images).
    VTP_PVRTC1      = 4<<9,                                     ///< Use 4bpp PVRTC, no alpha channel, common on ios devices, (commonly used for images).
    VTP_ETC2_RGB    = 5<<9,                                     ///< Use 4bpp ETC2, no alpha channel, common on android devices, (commonly used for images).
    VTP_ASTC        = 6<<9,                                     ///< Use 8bpp ASTC, no alpha channel, common on OpenGL3.0 enabled devices, (commonly used for images).
    VTP_RGBA8888_SN = 7<<9,                                     ///< Use 32bpp uncompressed textures 8 bit per component, RGBA, Signed, Normalized. Required for MipMapping on DX11.
    //we can use values 0-15
    //next bit available is 13
    VTP_MIPMAPS     = 1<<13                                     ///< This channel should also load / generate mipmap data.
} Anzu_TexturePermission_t;

/**
 * @brief   Expected audio buffer format.
 *
 * @details The SDK can produce audio samples in 32bit float or 16bit short per audio channel.
 */
typedef enum {
    ASF_FLOAT = 0,  ///< Use 32bit signed float to represent an audio sample.
    ASF_SHORT = 1   ///< Use 16bit signed short to represent an audio sample.
} AudioSampleFormat_t;

/**
 * @brief   Required log level.
 *
 * @details Sets the log level for SDK log messages, filter logs depending on their severity.
 */
typedef enum {
    LL_DEBUG   = 0,     ///< Log everything.
    LL_VERBOSE = 1,     ///< Log non-debug messages.
    LL_WARNING = 2,     ///< Log only warning and error messages.
    LL_ERROR   = 3,     ///< Log only error messages.
    LL_NONE    = 4      ///< No logging.
} Anzu_LogLevel_t;

/**
 * @brief   Return code for  rendering function.
 *
 * @details Return codes used for direct rendering function implementations.
 */
typedef enum {
    SDK_IS_NOT_INITIALIZED_YET = -2,                                ///< When OnRenderingEvent called, but SDK context was not initialized
    EVENT_ID_NOT_FOUND = -1,                                        ///< Missing event ID
    SUCCESS = 0,                                                    ///< Successful operation
    ANIMATED_TEXTURE_NOT_FOUND = 1,                                 ///< Missing animated texture object
    TEXTURE_BUFFER_NOT_FOUND = 2,                                   ///< Missing texture buffer
    TEXTURE_BUFFER_AND_ANIMATED_TEXTURE_NOT_FOUND = 3,              ///< Missing animated texture and texture buffer
    PROVIDED_RESOURCE_SIZE_IS_DIFFERENT_FROM_OUR_BUFFER_SIZE = 4,   ///< Invalid resource size
    FAILED_TO_OBTAIN_IMMEDIATE_CONTEXT = 5,                         ///< Failed to get immediate context
    PROVIDED_RESOURCE_USAGE_TYPE_IS_UNSUPPORTED = 6,                ///< Invalid resource usage requested
    PROVIDED_CONTEXT_IS_INVALID = 7,                                ///< Provided invalid context to UpdateNativeTexture
    PROVIDED_CONTEXT_SIZE_IS_INVALID = 8,                           ///< Insufficient userDataSize provided to UpdateNativeTexture
    NO_CUSTOM_TEXTURE_UPDATE_CALLBACK_CONFIGURED = 9,               ///< No custom texture update callback was configured
    FAILED_CREATING_GPU_RESOURCE = 10,                              ///< Failed creating GPU context
    WHAT_A_TERRIBLE_FAILURE_LOL = 0x7FFFFFFF                        ///< Something else
} Anzu_RenderingEventReturnCodes_t;

//----------------------------------------------------------------------------------------------------------------------------------

//always better working with callbacks if you can instead of relying on GetMessages
//reason 1: you can handle events asap, reason 2: no JSON encoding/decoding for common messages

//----------------------------------------------------------------------------------------------------------------------------------

/**
 * @fn      void (*AnzuCustomTextureUpdateCallback_t)(void *userdata, void * nativeTexturePtr, int width, int height, void * data, int size)
 *
 * @brief   Texture uploader function signature.
 *
 * @param   userdata Context value passed to Anzu__Texture_NativeRenderer_AssignCustomHandler.
 * @param   nativeTexturePtr The texture ID specified in Anzu__Texture_NativeRenderer_GetRenderID.
 * @param   width The width of the texture.
 * @param   height The height of the texture.
 * @param   data Bytes of the texture.
 * @param   size The size of the data buffer in bytes.
 *
 * @return  none.
 */
typedef void (ANZU_CALLCONV *AnzuCustomTextureUpdateCallback_t)(void *userdata, void * nativeTexturePtr, int width, int height, void * data, int size);

#pragma pack(push, 4)
/**
 * @brief   Structure which defines a single texture mip specification.
 *
 * @details An array of this structure is provided in the AnzuCustomTextureUpdateCallbackEx_t callback
 *          used by the Anzu__Texture_NativeRenderer_AssignCustomHandlerEx API.
 */
typedef struct {
    int width;      ///< The pixel width of the mip
    int height;     ///< The pixel height of the mip
    int size;       ///< The size of the mip data in bytes
    void * data;    ///< The mip texture data
} MipInfo_t;
#pragma pack(pop)

/**
 * @fn      void (*AnzuCustomTextureUpdateCallbackEx_t)(void *userdata, void * nativeTexturePtr, int mipCount, MipInfo_t ** mipInfos)
 *
 * @brief   Texture uploader function signature.
 *
 * @param   userdata Context value passed to Anzu__Texture_NativeRenderer_AssignCustomHandler.
 * @param   nativeTexturePtr The texture ID specified in Anzu__Texture_NativeRenderer_GetRenderID.
 * @param   mipCount Number of entries in mipInfos.
 * @param   mipInfos an array of MipInfo_t pointers with entries describing each mip information.
 *
 * @return  none.
 */
typedef void (ANZU_CALLCONV *AnzuCustomTextureUpdateCallbackEx_t)(void *userdata, void * nativeTexturePtr, int mipCount, MipInfo_t ** mipInfos);

/**
 * @fn      void (*AnzuLogCallback_t)( void * userdata, Anzu_LogLevel_t level, const char * msg )
 *
 * @brief   Log callback function signature.
 *
 * @param   userdata Context value passed to Anzu_RegisterLogCallback.
 * @param   level The severity level of the log message.
 * @param   msg The zero terminated log string.
 *
 * @return  none.
 */
typedef void (ANZU_CALLCONV *AnzuLogCallback_t)( void * userdata, Anzu_LogLevel_t level, const char * msg );

/**
 * @fn      void (*AnzuNetworkCallback_t)( void * userdata, bool isBusy )
 *
 * @brief   Network activity callback function signature.
 *
 * @param   userdata Context value passed to Anzu_RegisterNetworkCallback.
 * @param   isBusy Boolean specifying whether Anzu performs network activity.
 *
 * @return  none.
 */
typedef void (ANZU_CALLCONV *AnzuNetworkCallback_t)( void * userdata, bool isBusy );

/**
 * @fn      void (*AnzuNewMessageCallback_t)( void * userdata, const char * msg )
 *
 * @brief   New message callback function signature.
 *
 * @param   userdata Context value passed to Anzu_RegisterMessageCallback.
 * @param   msg The stringified JSON message as zero terminated string.
 *
 * @return  none.
 */
typedef void (ANZU_CALLCONV *AnzuNewMessageCallback_t)( void * userdata, const char * msg );

/**
 * @fn      void (*AnzuNewMessageEventCallback_t)( void * userdata )
 *
 * @brief   New message event callback function signature.
 *
 * @param   userdata Context value passed to Anzu_RegisterMessageEventCallback.
 *
 * @return  none.
 */
typedef void (ANZU_CALLCONV *AnzuNewMessageEventCallback_t)( void * userdata );

/**
 * @fn      void (*AnzuTextureUpdatedCallback_t)( void * userdata, int channelId, int token )
 *
 * @brief   Texture updated (dirty) callback function signature.
 *
 * @param   userdata Context value passed to Anzu_RegisterTextureUpdateCallback.
 * @param   channelId The Channel ID this message applies to.
 * @param   token The token that should be verified against the currently set playback token (by handling texture:init).
 *
 * @return  none.
 */
typedef void (ANZU_CALLCONV *AnzuTextureUpdatedCallback_t)( void * userdata, int channelId, int token );

/**
 * @fn      void (*AnzuTextureImpressionCallback_t)( void * userdata, int channelId, int token )
 *
 * @brief   Channel impression callback function signature.
 *
 * @param   userdata Context value passed to Anzu_RegisterTextureImpressionCallback().
 * @param   channelId Identifier of channel the callback called for.
 * @param   token Serial and unique playback identifier this callback applies to.
 *
 * @return  none.
 */
typedef void (ANZU_CALLCONV *AnzuTextureImpressionCallback_t)( void * userdata, int channelId, int token );

/**
 * @fn      void (*AnzuTexturePlacementProgressCallback_t)( void * userdata, int channelId, int token, float fullness )
 *
 * @brief   Channel impression progress callback function signature.
 *
 * @param   userdata Context value passed to Anzu_RegisterTexturePlacementProgressCallback().
 * @param   channelId Identifier of channel the callback called for.
 * @param   token Serial and unique playback identifier this callback applies to.
 * @param   fullness Time creative was visible by the user out of total time needed to count an impression.
 *
 * @return  none.
 */
typedef void (ANZU_CALLCONV *AnzuTexturePlacementProgressCallback_t)( void * userdata, int channelId, int token, float fullness );

/**
 * @fn      void (*AnzuTexturePlaybackProgressCallback_t)( void * userdata, int channelId, int token, float fullness )
 *
 * @brief   Channel creative playback progress callback function signature.
 *
 * @param   userdata Context value passed to Anzu_RegisterTexturePlaybackProgressCallback().
 * @param   channelId Identifier of channel the callback called for.
 * @param   token Serial and unique playback identifier this callback applies to.
 * @param   fullness Sum of played frames out of total video length.
 *
 * @return  none.
 */
typedef void (ANZU_CALLCONV *AnzuTexturePlaybackProgressCallback_t)( void * userdata, int channelId, int token, float fullness );

/**
 * @fn      void (*AnzuTexturePlaybackCompleteCallback_t)( void * userdata, int channelId, int token )
 *
 * @brief   Channel playback complete callback function signature.
 *
 * @param   userdata Context value passed to Anzu_RegisterTexturePlaybackCompleteCallback().
 * @param   channelId Identifier of channel the callback called for.
 * @param   token Serial and unique playback identifier this callback applies to.
 *
 * @return  none.
 */
typedef void (ANZU_CALLCONV *AnzuTexturePlaybackCompleteCallback_t)( void * userdata, int channelId, int token );

//when init arrives (either through callback or through get messages) it means a new playback has initiated
//all texture related messages should be respected only if the token match the token provided through this message
//the next message regarding this playback will happen after the decoder tried to open the media, you might then get an texture:error if there was problem
//or texture:info containing information about the texture dimensions need to be prepared and other stuff related to the playback

/**
 * @fn      void (*AnzuTextureInitCallback_t)( void * userdata, int channelId, int token )
 *
 * @brief   Texture Initialization callback function signature.
 *
 * @param   userdata Context value passed to Anzu_RegisterTextureInitCallback.
 * @param   channelId The Channel ID this texture initialization event applies to.
 * @param   token The playback token this channel should respect for next events regarding this channel.
 *
 * @return  none.
 */
typedef void (ANZU_CALLCONV *AnzuTextureInitCallback_t)( void * userdata, int channelId, int token );

/**
 * @fn      void (*AnzuTexturePlaybackInfoCallback_t)( void * userdata, const char* info )
 *
 * @brief   Channel playback info callback function signature.
 *
 * @param   userdata Context value passed to Anzu_RegisterTexturePlaybackInfoCallback.
 * @param   info The stringified JSON playback info as zero terminated string.
 *
 * @return  none.
 */
typedef void (ANZU_CALLCONV *AnzuTexturePlaybackInfoCallback_t)(void* userdata, const char* info );

/**
 * @fn      void (*AnzuTextureAuditColorCallback_t)( void * userdata, int channelId, int token, float timePos, float normX, float normY, unsigned int rgba )
 *
 * @brief   Texture color auditing callback function signature.
 *
 * @param   userdata Context value passed to Anzu_RegisterTextureAuditColorCallback.
 * @param   channelId The Channel ID this color audit request applies to.
 * @param   token The playback token this color audit request applies to.
 * @param   timePos The media time position this audit request applies to.
 * @param   normX Normalized X position in placement of the pixel this audit request applies to.
 * @param   normY Normalized Y position in placement of the pixel this audir request applies to.
 * @param   rgba Optional RGBA8888 color value expected for perfect color match, if the A byte is 0, this value is unknown.
 *
 * @return none.
 */
typedef void (ANZU_CALLCONV *AnzuTextureAuditColorCallback_t)( void * userdata, int channelId, int token, float timePos, float normX, float normY, unsigned int rgba );

/**
 * @fn      const char * (*GetKeystoreItem_t)( void * userdata, const char * key )
 *
 * @brief   Keystore get function override signature.
 *
 * @param   userdata Context value passed to Anzu_SetKeystoreOverride.
 * @param   key The keystore key name, which value is queried.
 *
 * @return  The zero terminated queried value, empty string if no such key exists.
 */
typedef const char * (ANZU_CALLCONV *GetKeystoreItem_t)( void * userdata, const char * key );

/**
 * @fn      void (*SetKeystoreItem_t)( void * userdata, const char * key, const char * value )
 *
 * @brief   Keystore set function override signature.
 *
 * @param   userdata Context value passed to Anzu_SetKeystoreOverride.
 * @param   key The keystore key name.
 * @param   value The zero terminated value to set for that key.
 *
 * @return  none.
 */
typedef void (ANZU_CALLCONV *SetKeystoreItem_t)( void * userdata, const char * key, const char * value );

/**
 * @fn      void (*UriSchemaHookCallback_t)( void * userdata, bool isPost, const char * uri, const char * body )
 *
 * @brief   callurl schema hook registration callback function signature.
 *
 * @param   userdata Context value passed to Anzu_RegisterUriSchemaHook.
 * @param   isPost boolean specifying if the call url action was a GET or a POST request.
 * @param   uri the uri following the registered schema passed to a callurl action.
 * @param   body the body of the request (in case POST request is issued.
 *
 * @return  none.
 */
typedef void (ANZU_CALLCONV *UriSchemaHookCallback_t)( void * userdata, bool isPost, const char * uri, const char * body );

//----------------------------------------------------------------------------------------------------------------------------------

/**
 * @fn     float Anzu_GetVersionFloat( void )
 *
 * @brief  Returns SDK version as float.
 *
 * @return SDK version as float number.
 */
EXTN EXPORT float ANZU_CALLCONV Anzu_GetVersionFloat( void ) ANZU_DUMMY_IMP(0);

/**
 * @fn      bool Anzu_RequireCompatibleInterfaceVersion( const char * version )
 *
 * @brief   Allows selecting interface protocol version.
 *
 * @details Currently there is only 1 version supported: "1.0".
 *          In general, all new features added are not mandatory for normal operation of the SDK. When feature replaces an
 *          existing behaviour, it only replaces it if *activated*, for example in logs if you did'nt register for a direct
 *          callback, you still get them the old ways through JSON messages.
 *          New message types, subtypes or fields added to JSON messages are never mandatory to implement unless using new
 *          features.
 *          Version will only change if there are breaking changes that can affect proper behavior.
 *          The SDK might support multiple versions if breaking changes are made, so it will adopt the the version requested.
 *          You should always ask for the latest version you know to successfully work with.
 *          The default behavior is the SDK assuming the engine is compatible with the latest interface protocol version.
 *
 * @param   version the requested protocol version, should be "1.0".
 *
 * @return  Boolean specifying whether the protocol version is supported, true means its supported.
 */
EXTN EXPORT bool ANZU_CALLCONV Anzu_RequireCompatibleInterfaceVersion( const char * version ) ANZU_DUMMY_IMP(false);

/**
 * @fn      bool Anzu_Initialize( const char * appid, const char * appkey, bool isDevelopment )
 *
 * @brief   Initialize the SDK.
 *
 * @details If you need analytics, remote content management, and ad support, you must call this function at the start of your
 *          app.
 *          This function initializes the SDK with your app credentials and gets ready for ad serving.
 *          You should call this function only after you set all your callbacks or custom overrides.
 *
 * @param   appid The application identifier for this title, as defined in the Anzu dashboard.
 * @param   appkey The server generated key provided by the Anzu dashboard.
 * @param   isDevelopment set to true when making a development build, false when making a distribution build
 *
 * @return  Boolean specifying if parameters pass basic validation, the function should always succeed as long as
 *          appid / appkey are not NULL.
 *
 * @remarks Publishers gets their app key through Anzu's publisher management console.
 */
EXTN EXPORT bool ANZU_CALLCONV Anzu_Initialize( const char * appid, const char * appkey, bool isDevelopment ) ANZU_DUMMY_IMP(false);

/**
 * @fn      void Anzu_Uninitialize( void )
 *
 * @brief   Uninitialize the SDK.
 *
 * @return  Integer specifying uninitialize status, this is always 0.
 */
EXTN EXPORT int ANZU_CALLCONV Anzu_Uninitialize( void ) ANZU_DUMMY_IMP(0);

/**
 * @fn      void Anzu_ApplicationActive( bool isActive )
 *
 * @brief   Informs the SDK when application enters or leaves background.
 *
 * @details Call this function when game is paused / resumed.
 *
 * @param   isActive Should be true when application enters foreground, false when application enters background.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_ApplicationActive( bool isActive ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_ApplicationQuit( void )
 *
 * @brief   Informs the SDK when application is about to terminate.
 *
 * @details Call this function when game quits.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_ApplicationQuit( void ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_SetDeveloperKey( const char * devkey )
 *
 * @brief   Used internally by development environment to identify developer runs inside the editor.
 *
 * @details This allows the backend to learn (and respect) new metadata about placements.
 *          You should never expose your devkey in public builds. specifying the devkey is not a *must* it simply allows anzu
 *          to mark placements (channels) as 'verified' knowing that they actually came from the publisher and not invented
 *          by malicious flooding robot.
 *
 * @param   devkey The developer key (provided by the anzu team or through REST api).
 *
 * @return  none.
 *
 * @remarks This function does not provide success or failure code by design as the validity of this key is only treated by
 *          the server.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_SetDeveloperKey( const char * devkey ) ANZU_DUMMY_IMP();

/**
 * @fn      int Anzu__Texture_CreateInstance( const char * channelName, const char * tags, int permissions, float width, float height, bool shrinkToFit, int audioSampleRate )
 *
 * @brief   Creates a reference to a new channel or existing channel with the same channelName.
 *
 * @details Calling this function creates a reference to a new / existing channel object mapped to channelName.
 *          The channel object is reference counted based on channelName, when creating multiple references to the same channel,
 *          be sure to pass the same parameters.
 *
 * @param   channelName The channel name. each channel is responsible for a single texture. this parameter cannot be NULL or
 *          empty string.
 * @param   tags Comma separated list of tags for this channel, this parameter can be NULL if no tags are assigned for this
 *          channel.
 * @param   permissions Bitwise combination of *Anzu_TexturePermission_t* flags identifying the nature and capabilities of the
 *          channel.
 * @param   width The width of the channel, in imaginary units. this value along with height reflects the channel's aspect
 *          ratio to the SDK when making decisions about choosing the right media served.
 * @param   height The height of the channel, in imaginary units.
 * @param   shrinkToFit Can this channel adapt to new aspect ratios (by for example altering the mesh scale or vertices).
 * @param   audioSampleRate When channel can project audio, this is specifies the expected audio sample rate, when no audio
 *          is needed, set to 0.
 *
 * @return  Integer representing the channel ID. when the function fails, the returned value is 0.
 *
 * @remarks When working with audio, you are expected to call *Anzu__Texture_FillAudioBuffers* periodically and mix the audio
 *          buffers with your (3d?) audio mixer.
 *          If you do support audio for that channel, you get notified about the existence of audio in the media (not all
 *          medias has audio) in the texture:info message.
 */
EXTN EXPORT int ANZU_CALLCONV Anzu__Texture_CreateInstance( const char * channelName, const char * tags, int permissions, const float width, const float height, const bool shrinkToFit, const int audioSampleRate ) ANZU_DUMMY_IMP(0);

/**
 * @fn      int Anzu__Texture_CreateInstanceForTexture( const char * channelName, const char * tags, const int permissions, const float width, const float height, const bool shrinkToFit, const int audioSampleRate, const int forceWidth, const int forceHeight )
 *
 * @brief   Same as *Anzu__Texture_CreateInstance* but allows forcing an explicit texture resolution.
 *
 * @details Some engines cannot resize their textures once they are set, this function will rescale all the buffers to fit to
 *          the dimensions specified in the forceWidth and forceHeight parameters.
 *
 * @param   channelName The channel name. each channel is responsible for a single texture. this parameter cannot be NULL or
 *          empty string.
 * @param   tags Comma separated list of tags for this channel, this parameter can be NULL if no tags are assigned for this
 *          channel.
 * @param   permissions Bitwise combination of *Anzu_TexturePermission_t* flags identifying the nature and capabilities of the
 *          channel.
 * @param   width The width of the channel, in imaginary units. this value along with height reflects the channel's aspect
 *          ratio to the SDK when making decisions about choosing the right media served.
 * @param   height The height of the channel, in imaginary units.
 * @param   shrinkToFit Can this channel adapt to new aspect ratios (by for example altering the mesh scale or vertices).
 * @param   audioSampleRate When channel can project audio, this is specifies the expected audio sample rate, when no audio
 *          is needed, set to 0.
 * @param   forceWidth The texture width that should be enforced on media buffers for this channel.
 * @param   forceHeight The texture height that should be enforced on media buffers for this channel.
 *
 * @return  Integer representing the channel ID. when the function fails, the returned value is 0.
 *
 * @remarks When forceWidth and forceHeight are 0, no enforcing is performed on resolution and result is the same as calling
 *          the *Anzu__Texture_CreateInstance* function. See *Anzu__Texture_CreateInstance* for additional remarks.
 */
EXTN EXPORT int ANZU_CALLCONV Anzu__Texture_CreateInstanceForTexture( const char * channelName, const char * tags, int permissions, const float width, const float height, const bool shrinkToFit, const int audioSampleRate, const unsigned int forceWidth, const unsigned int forceHeight ) ANZU_DUMMY_IMP(0);

/**
 * @fn      bool Anzu__Texture_RemoveInstance( int channelId )
 *
 * @brief   Remove reference for channel, when no references remain, removes the channel
 *
 * @param   channelId The ID of the channel.
 *
 * @return  boolean indicating whether this was the last reference to that channel, false if this channel is still referenced,
 *          true if this was the last reference.
 */
EXTN EXPORT bool ANZU_CALLCONV Anzu__Texture_RemoveInstance( int channelId ) ANZU_DUMMY_IMP(false);

/**
 * @fn      void Anzu__Texture_NativeRenderer_AssignCustomHandler( AnzuCustomTextureUpdateCallback_t callback, void * userdata)
 *
 * @brief   Allows setting up a custom texture update handler.
 *
 * @details Can be used for direct texture buffer updates using the Game engine's own update function.
 *
 * @param   callback Callback function that uploads the buffer to the texture.
 * @param   userdata Context of the callback function.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu__Texture_NativeRenderer_AssignCustomHandler( AnzuCustomTextureUpdateCallback_t callback, void * userdata) ANZU_DUMMY_IMP();

/**
 * @fn        void Anzu__Texture_NativeRenderer_AssignCustomHandlerEx( AnzuCustomTextureUpdateCallbackEx_t callback, void * userdata)
 *
 * @brief    Allows setting up a custom texture update handler.
 *
 * @details Can be used for direct texture buffer updates using the Game engine's own update function.
 *          This function in comparison to Anzu__Texture_NativeRenderer_AssignCustomHandler provide all the mipmaps if exists, rather than mip 0 only
 *
 * @param   callback The callback function that uploads the buffer to the texture.
 * @param   userdata Context to deliver to the callback function.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu__Texture_NativeRenderer_AssignCustomHandlerEx( AnzuCustomTextureUpdateCallbackEx_t callback, void * userdata) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu__Texture_NativeRenderer_SetExpectedFormat( int format )
 *
 * @brief   Set the expected red / blue order on RGBA8888
 *
 * @details Choose between RGBA and BGRA color ordering.
 *
 * @param   format 0 - RGBA format (default), 1 - BGRA format.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu__Texture_NativeRenderer_SetExpectedFormat( int format ) ANZU_DUMMY_IMP();

/**
 * @fn      void * Anzu__Texture_NativeRenderer_GetRenderCallback( const char * engine, void * info )
 *
 * @brief   Returns a Native renderer callback function for direct GPU texture updates.
 *
 * @details Using direct texture updates allows efficient texture updates without copying the prepared channel's compressed
 *          image buffer into an intermediate buffer allocated by the engine.
 *          The resulting callback should be called with the render ID provided by *Anzu__Texture_NativeRenderer_GetRenderID*
 *          at the proper time during the rendering pipeline, in which it is optimal and valid to update the texture.
 *
 * @param   engine String describing the graphics engine. Currently supported values are:
 *          "direct3d9"  - DirectX 9 (Windows)
 *          "direct3d11" - DirectX 11 (Windows)
 *          "opengl1"    - OpenGL 1.x (Almost all platforms and OSs)
 *          "opengl"     - OpenGL / ES 2.0+ (Almost all platforms and OSs)
 *          "metal"      - Metal (Apple devices)
 *          "vulkan"      - Vulkan (Android devices) *
 *          "ps4"      - GNM (PlayStation 4 consoles) *
 * @param   info This value is ignored and should be NULL on all engines except direct3d11, in which, this value should be
 *          the ID3D11Device * ptr.
 *
 * @return  NULL if no native renderer is supported for that engine on this device, if engine was valid, a pointer to a
 *          function with the signature void OnRenderingEvent( int renderId ) is returned.
 *          should be called with the render ID provided by *Anzu__Texture_NativeRenderer_GetRenderID*
 *          at the proper time during the rendering pipeline, in which it is optimal and valid to update the texture.
 *
 * @remarks New rendering engines can be added or demand.
 *          On Unity3D engine interation, info can be NULL even on direct3d11.
 */
EXTN EXPORT void * ANZU_CALLCONV Anzu__Texture_NativeRenderer_GetRenderCallback( const char * engine, void * info ) ANZU_DUMMY_IMP(NULL);

/**
 * @fn      bool Anzu__Texture_NativeRenderer_DoesSupportFormat( const char * format )
 *
 * @brief   Check if the native renderer know how to handle the specified texture compression format.
 *
 * @details If the native renderer does not know how to handle the format, choose a different format that is better compatible
 *          with your platform, or use Anzu__Texture_UpdateData and use your engine texture update function.
 *
 * @param   format String describing the format. Currently supported formats values are:
 *          "RGBA8888" - 32bpp RGBA (or BGRA when passing 1 to *Anzu__Texture_NativeRenderer_SetExpectedFormat*) 8:8:8:8, alpha
 *                       enabled, supported by all native renderers.
 *          "RGB565"   - 16bpp RGB 5:6:5, no alpha, supported by most native renderers.
 *          "DXT1"     - 4bpp RGB BC1 (DXT1), no alpha, supported by most desktops and consoles.
 *          "BC1"      - same as "DXT1"
 *          "ETC1"     - 4bpp RGB ETC1, no alpha, supported by most android devices.
 *          "ETC2_RGB" - 4bpp RGB ETC2, no alpha, supported by recent android devices.
 *          "PVRTC1"   - 4bpp RGB PVRTC1, no alpha, supported by all iOS devices.
 *          "ASTC"     - 8bpp ASTC 4x4, no alpha, supported by newer devices.
 *
 * @return  Boolean specifying true if format is supported, false if not supported.
 *
 * @remarks New formats can be added on demand.
 */
EXTN EXPORT bool ANZU_CALLCONV Anzu__Texture_NativeRenderer_DoesSupportFormat( const char * format ) ANZU_DUMMY_IMP(false);

/**
 * @fn      int Anzu__Texture_NativeRenderer_GetRenderID( int channelId, void * nativeTexturePtr, int width, int height, void * data, int size )
 *
 * @brief   Acquires a render ID for Native Renderer - a direct GPU texture updates handler.
 *
 * @details This is the optimized way for updating the texture, When issuing a call provided with the returned render ID
 *          as the parameter to the callback returned by *Anzu__Texture_NativeRenderer_GetRenderCallback*, the channel's
 *          buffer containing the dirty compressed texture buffer, will be uploaded to the GPU texture memory.
 *
 * @param   channelId The channel's ID.
 * @param   nativeTexturePtr This is the native texture handle, This value should be:
 *          GLuint with the GL_TEXTURE_2D texture handle on OpenGL.
 *          ID3D11Texture2D* on DirectX 11.
 *          IDirect3DTexture9* on DirectX 9.
 *          id<MTLTexture> on Metal.
 * @param   width The current width of the texture.
 * @param   height The current height of the texture.
 * @param   data additional data required for native rendering (nullptr on most platforms).
 * @param   size size in bytes of additional data provided (0 on most platforms).
 *
 * @return  A render ID used for direct texture updates, or 0 if there is no qualified buffer for this texture.
 *
 * @remarks data/size fields are required on some platforms, please discuss the anzu team about the use of these prameters.
 */
EXTN EXPORT int ANZU_CALLCONV Anzu__Texture_NativeRenderer_GetRenderID( int channelId, void * nativeTexturePtr, int width, int height, void * data, int size) ANZU_DUMMY_IMP(0);

/**
 * @fn      bool Anzu__Texture_NativeRenderer_CanGenerateNativeTexture( const char * engine, void * info )
 *
 * @brief   Returns true if texture can be generated by anzu and set up texture update parameters to allow
 *          direct texture updates on the GPU.
 *
 * @details Setting up and using direct texture updates allows efficient texture updates without
 *          copying the prepared channel's compressed image buffer into an intermediate buffer
 *          allocated by the engine. False return value means textures must be created by user
 *          while true return value means you can tell the sdk to create the texture for it's own use.
 *          Even if the return value is false direct texture update still may be supported.
 *          To check it you will need to call *Anzu__Texture_NativeRenderer_GetUpdaterCallback()*
 *
 * @param   engine String describing the graphics engine. Currently supported values are:
 *          "direct3d9"  - DirectX 9 (Windows)
 *          "direct3d11" - DirectX 11 (Windows)
 *          "opengl1"    - OpenGL 1.x (Almost all platforms and OSs)
 *          "opengl"     - OpenGL / ES 2.0+ (Almost all platforms and OSs)
 *          "metal"      - Metal (Apple devices)
 *          "vulkan"     - Vulkan (Android devices) *
 *          "ps4"        - GNM (PlayStation 4 consoles) *
 *
 * @param   info This value is ignored and should be NULL on all engines except direct3d11, in which, this value should be
 *          the ID3D11Device * ptr.
 *
 * @return  Boolean specifying true if SDK can generate and destroy textures for current engine.
 *
 * @remarks New rendering engines can be added or demand.
 *          On Unity3D engine interation, info can be NULL even on direct3d11.
 */
EXTN EXPORT bool ANZU_CALLCONV Anzu__Texture_NativeRenderer_CanGenerateNativeTexture(const char * engine, void * info) ANZU_DUMMY_IMP(false);


/**
 * @fn      int Anzu__Texture_NativeRenderer_CreateNative_PrepareTextureCreationParams( int id, int width, int height )
 *
 * @brief   This function returns a task ID that will help to generate the texture. It must be supplied when calling for creation function on render thread.
 *
 * @param   id Channel id
 * @param   width Width of the requested texture
 * @param   height Height of the requested texture
 *
 * @return  A task ID that should be supplied later to the callback that is returned by Anzu__Texture_NativeRenderer_CreateNative_GetCallbackForRenderThread
 *
 * @remarks The format of the new texture will be derrived from the channel info
 */
EXTN EXPORT int ANZU_CALLCONV Anzu__Texture_NativeRenderer_CreateNative_PrepareTextureCreationParams(int id, int width, int height) ANZU_DUMMY_IMP(0);

/**
 * @fn      void * Anzu__Texture_NativeRenderer_GetCreatorCallback( void )
 *
 * @brief   This function returns a callback that will generate the texture. It must be called only on render thread.
 *
 * @return  The following delegate function : void (* func)(int resourceID).
 *
 * @remarks Use this callback if you prefer that Anzu will generate the texture for you.
 */
EXTN EXPORT void * ANZU_CALLCONV Anzu__Texture_NativeRenderer_GetCreatorCallback(void) ANZU_DUMMY_IMP(NULL);

/**
 * @fn      void * Anzu__Texture_NativeRenderer_CreateNative_GetNativeResource( int rID )
 *
 * @brief   After you have created the GPU resource you can get it by calling this function 
 *          and supplying the EventID you got from 
 *          Anzu__Texture_NativeRenderer_CreateNative_PrepareTextureCreationParams.
 *
 * @return  The pointer to the GPU resource that was created by the delegate you got from Anzu__Texture_NativeRenderer_GetCreatorCallback.
 */
EXTN EXPORT void * ANZU_CALLCONV Anzu__Texture_NativeRenderer_CreateNative_GetNativeResource(int rID) ANZU_DUMMY_IMP(NULL);


/**
 * @fn      void * Anzu__Texture_NativeRenderer_GetUpdaterCallback( void )
 *
 * @details Using direct texture updates allows efficient texture updates without copying the prepared channel's compressed
 *          image buffer into an intermediate buffer allocated by the engine.
 *          The resulting callback should be called with the render ID provided by *Anzu__Texture_NativeRenderer_GetRenderID*
 *          at the proper time during the rendering pipeline, in which it is optimal and valid to update the texture.
 *
 * @brief   Returns a Native renderer callback function for direct GPU texture updates.
 *
 * @return  NULL if no native renderer is supported for that engine on this device, if engine was valid, a pointer to a
 *          function with the signature void OnRenderingEvent( int renderId ) is returned.
 *          The returned functions should be called with the render ID provided by *Anzu__Texture_NativeRenderer_GetRenderID*
 *          at the proper time during the rendering pipeline, in which it is optimal and valid to update the texture.
 */
EXTN EXPORT void * ANZU_CALLCONV Anzu__Texture_NativeRenderer_GetUpdaterCallback(void) ANZU_DUMMY_IMP(NULL);

/**
 * @fn      void * Anzu__Texture_NativeRenderer_GetDestroyerCallback( void )
 *          
 * @brief   Gets a delegate that when called with appropriate eventID will release a GPU resource.
 *          
 * @details You should save the callback, then call Anzu__Texture_NativeRenderer_CreateNative_PrepareTextureDeletionParams while 
 *          supplying it with pointer. You will receive back and int that you should supply to the delegate.
 *          
 *          
 * @return  Returns a delegate to a function that receives a GPU resource and releases it. The delegate must be called on a rendering thread.
 *          
 * @remarks Before calling the delegate, you must call Anzu__Texture_NativeRenderer_CreateNative_PrepareTextureDeletionParams to get proper eventID
 *          
 */

EXTN EXPORT void * ANZU_CALLCONV Anzu__Texture_NativeRenderer_GetDestroyerCallback( void ) ANZU_DUMMY_IMP(NULL);

/**
 * @fn      int Anzu__Texture_NativeRenderer_CreateNative_PrepareTextureDeletionParams(void * )
 *
 * @brief   Receives an int assosiated with a GPU pointer, and releases the GPU resource.
 *
 * @param   texPtr The texture ptr returned by the creation API.
 *
 * @return  Returns a delegate to a function that receives a GPU resource and releases it.The delegate must be run on a rendering thread.
 *
 * @remarks Make sure to call the returned function only on resources previously created by Anzu
 *
 */

EXTN EXPORT int ANZU_CALLCONV Anzu__Texture_NativeRenderer_CreateNative_PrepareTextureDeletionParams(void * texPtr) ANZU_DUMMY_IMP(0);

/**
 * @fn      int Anzu__Texture_UpdateData( int channelId, void * pixels, int width, int height )
 *
 * @brief   Provides a way of getting a copy of the channel's texture buffer, or pool for it's dirty state.
 *
 * @details Use this function to update buffer if you cannot use NativeRenderer. Also it is more efficient to register for
 *          Texture update callbacks and get notified when a channel gets dirty, than to pool for dirty flag.
 *
 * @param   channelId The channel's ID.
 * @param   pixels Pointer to a buffer that can contain the channel's texture buffer, the pixels buffer capacity to be (at
 *          least) width * height * bpp (bits per pixel) / 8 bytes in size. If NULL is provided, no buffer update is performed,
 *          but the returned value still reflects whether the texture was (and still is) dirty.
 * @param   width The pixel width of the buffer.
 * @param   height The pixel height of the buffer.
 *
 * @return  0 if no update is needed, 1 if texture data was dirty and buffer copy was (or should be, in the case pixels is NULL)
 *          was performed.
 *
 * @remarks It is much more efficient to use NativeRenderer. it saves you memory and CPU time.
 */
EXTN EXPORT int ANZU_CALLCONV Anzu__Texture_UpdateData( int channelId, void * pixels, int width, int height ) ANZU_DUMMY_IMP(0);

/**
 * @fn     bool Anzu__Texture_ResetTags( int channelId, const char * tags )
 *
 * @brief  Allows resetting channel's tags.
 *
 * @param  channelId The ID of the channel.
 * @param  tags String of tags, separated by commas. if NULL is provided, tags are cleared (same as empty string)
 *
 * @return boolean specifying true if successful operation or false if error.
 */
EXTN EXPORT bool ANZU_CALLCONV Anzu__Texture_ResetTags( int channelId, const char * tags ) ANZU_DUMMY_IMP(false);

/**
 * @fn     bool Anzu__Texture_ResetPermissions( int channelId, int permissions )
 *
 * @brief  Allows resetting channel's permission.
 *
 * @param  channelId The ID of the channel.
 * @param  permissions Bitwise combination of *Anzu_TexturePermission_t* flags identifying the nature and capabilities of the
 *          channel.
 *
 * @return boolean specifying true if successful operation or false if error.
 *
 * @remarks Use with care, consult the anzu team on best practices.
 */
EXTN EXPORT bool ANZU_CALLCONV Anzu__Texture_ResetPermissions( int channelId, int permissions ) ANZU_DUMMY_IMP(false);

/**
 * @fn      void Anzu__Texture_Resync( int channelId )
 *
 * @brief   Discard ongoing channel's media playback, and attempt changing to a new ad.
 *
 * @param   channelId The ID of the channel.
 *
 * @return  none.
 *
 * @remarks By discarding the current ad, you might lose an impression. Also, showing an image for a too short period of time
 *          or swapping a video in the middle of a clip might lead to a bad user experience.
 *          There are some cases where calling this function makes sense, especially when creating complex self served funnels
 *          using campaign metrics or tags.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu__Texture_Resync( int channelId ) ANZU_DUMMY_IMP();

/**
 * @fn      bool Anzu__Texture_PlayFilename( int channelId, const char * filename, bool loop, bool paused )
 *
 * @brief   Play a media (on most cases - the fallback media) on the channel
 *
 * @details Calling this method allows you to play you own media on the channel. This is mostly used to play a fallback media
 *          If the at the given time no media arrived, and we want the texture to show non blank image.
 *
 * @param   channelId The ID of the channel.
 * @param   filename The full path for the media file to play.
 * @param   loop Should the playback loop once reaches to the end of the clip.
 * @param   paused Should it start in paused state.
 *
 * @return  Boolean indicating if playback operation started successfully, true meaning success, false meaning failure.
 *
 * @remarks You should start the media paused, and resume it on texture:info event to be persistent with how on handle playback
 *          both when it is initiated by the SDK or your engine.
 */
EXTN EXPORT bool ANZU_CALLCONV Anzu__Texture_PlayFilename( int channelId, const char * filename, bool loop, bool paused ) ANZU_DUMMY_IMP(false);

/**
 * @fn      bool Anzu__Texture_StopPlayback( int channelId )
 *
 * @brief   Stop a media playback.
 *
 * @details For a successful integration, there is no good reason ever to use this call. If you manage your own playbacks, You
 *          can use it though.
 *
 * @param   channelId The ID of the channel.
 *
 * @return  Boolean indicating success. true for success, false for failure.
 */
EXTN EXPORT bool ANZU_CALLCONV Anzu__Texture_StopPlayback( int channelId ) ANZU_DUMMY_IMP(false);

/**
 * @fn      void Anzu__Texture_PausePlayback( int channelId )
 *
 * @brief   Pause a media playback.
 *
 * @details For a successful integration, there is no good reason ever to use this call. If you manage your own playbacks, You
 *          can use it though.
 *
 * @param   channelId The ID of the channel.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu__Texture_PausePlayback( int channelId ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu__Texture_ResumePlayback( int channelId )
 *
 * @brief   Resume a media playback.
 *
 * @details The glue code should resume media playback once the preparation of the texture is done. Once you set up your
 *          texture in response to texture info event, you should resume playback for the channel.
 *
 * @param   channelId The ID of the channel.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu__Texture_ResumePlayback( int channelId ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu__Texture_SetVisibility( int channelId, bool isVisible )
 *
 * @brief   Inform the SDK that a game object with managed texture enters or leaves visible screen area.
 *
 * @details The engine should call this method whenever a placement carrying anzu channel enters or leaves the screen area.
 *          The default state for a newly created placement is invisible. The SDK discards several operations (mostly in case
 *          video is played) when the channel is not visible.
 *
 * @param   channelId The ID of the channel.
 * @param   isVisible value should be true if object is in visible screen space, false if out of visible space.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu__Texture_SetVisibility( int channelId, bool isVisible ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu__Texture_SetVisibilityScore( int channelId, float screenSum, float planeSum, float minAngle )
 *
 * @brief   Inform the SDK about the visibility status of the game object.
 *
 * @details All channels must report their screen score and object scores 5 times a second.
 *          The way to perform computations of these variables differ between different game engines, however,
 *          the concepts are the same. It is important to provide the SDK with visibility
 *          status or the SDK will not count impressions properly, and when audited can be marked as fraud.
 *
 * @param   channelId The ID of the channel.
 * @param   screenSum The relative space the visible part of the placement (which should be a rectangular quad or plane),
 *          normalized between 0 to 1, 0 meaning nothing it consumes nothing on the display area (invisible from sight), while
 *          1 meaning it covers the entire display.
 * @param   planeSum The relative area from the placement that managed as anzu channel which is visible, normalized between
 *          0 to 1, 0 meaning nothing of the placement is visible, 1 meaning the entire placement is visible.
 * @param   minAngle The Angle of the camera in respect to the placement, 0 means it is right in front (facing) of the
 *          placement, 90 means it is facing the placement right from it's side, 180 means it is facing the placement's back.
 *
 * @return  none.
 *
 * @remarks The way to measure logos (which are blended into textures and most of the time does not apply to rectangular planes
 *          but to complex meshes) the calculation is different, and also their pricing model, which is fixed.
 *          Please discuss the measure procedures with anzu team.
 *          When placement is invisible, it is valid to report once with screenSum and planeSum set as 0, and keep reporting
 *          once the placement becomes visible again.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu__Texture_SetVisibilityScore( int channelId, float screenSum, float planeSum, float minAngle ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu__Texture_Interact( int channelId, int button, const char * name )
 *
 * @brief   Inform the SDK that the player interacted with the placement.
 *
 * @details Anzu SDK does not dictate how interaction takes place, this is something that is totally up to the game publisher
 *          Some games might decide clicking a placement is the right way, while other games might used controls that more
 *          natural to the game UI.
 *
 * @param   channelId The ID of the channel.
 * @param   button This value should be 0 for 'main click' interaction, and 1 for 'alt click' interaction (and is optional).
 *          On most cases, both values would be treated the same.
 * @param   name The name of the game object in which the interaction occurred. (optional, can be NULL)
 *
 * @return  none.
 *
 * @remarks Most ad creatives have a clickable URL address for interactions, you can also define your own actions triggered
 *          by interacting with placements. Many 'playable' modules are also interaction supported.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu__Texture_Interact( int channelId, int button, const char * name ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu__Texture_InteractXY( int channelId, float x, float y, int button, const char * name )
 *
 * @brief   Inform the SDK that the player interacted with the placement on a specific position.
 *
 * @details This can be used to allow positioned clicks on HTML pages loaded as textures and playable modules.
 *
 * @param   channelId The ID of the channel.
 * @param   x The normalized horizontal position 'click' was performed, 0 is left, 1 is right.
 * @param   y The normalized vertical position 'click' was performed, 0 is top, 1 is bottom.
 * @param   button This value should be 0 for 'main click' interaction, and 1 for 'alt click' interaction (and is optional).
 *          On most cases, both values would be treated the same.
 * @param   name The name of the game object in which the interaction occurred. (optional, can be NULL)
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu__Texture_InteractXY( int channelId, float x, float y, int button, const char * name ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_SetAudioSampleFormat( AudioSampleFormat_t format )
 *
 * @brief   Sets up the engine's expected audio sample format.
 *
 * @details Use this function if you support audio to select the audio sample format expected by the engine when receiving
 *          audio buffers. You should call it at the initialization stage.
 *
 * @param   format The audio sample format.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_SetAudioSampleFormat( AudioSampleFormat_t format ) ANZU_DUMMY_IMP();

/**
 * @fn      bool Anzu__Texture_FillAudioBuffers( int channelId, void * buffer, int framesToRead, int sampleRate, int channels )
 *
 * @brief   Allows passing audioable channel audio buffer for mixing with the game engine's sound system.
 *
 * @details You should periodically call this method to dequeue audio buffers and queue them in your engine's audio mixer.
 *          There is no need to pool audio buffers if the texture:info:audio parameter is false, as the clip does not contain
 *          audio.
 *
 * @param   channelId The ID of the audioable channel.
 * @param   buffer A memory buffer on which audio sample would be written, it should be large enough to contain at least
 *          framesToRead * channels * sample size bytes.
 * @param   framesToRead The number of frames needed.
 * @param   sampleRate The engine's expected audio samplerate. The SDK will do live conversion if needed.
 * @param   channels The engine's expected number of audio channels. The SDK will do live conversion if needed.
 *
 * @return  true if the buffer was filled successfully, false if not enough frames are ready.
 */
EXTN EXPORT bool ANZU_CALLCONV Anzu__Texture_FillAudioBuffers( int channelId, void * buffer, int framesToRead, int sampleRate, int channels ) ANZU_DUMMY_IMP(false);

/**
 * @fn      void Anzu_SetKeystoreOverride( GetKeystoreItem_t getfn, SetKeystoreItem_t setfn, void * userdata )
 *
 * @brief   Provide engine with the ability to replace the SDK's default key-store implementation.
 *
 * @details The default key-store implementation creates and access a file named anzu.db in the anzu cache folder. If you erase
 *          this file manually of by cleaning the entire cache, you should provide an alternative key-store API.
 *
 * @param   getfn A callback for string-to-string get operations.
 * @param   setfn A callback for string-to-string set operations.
 * @param   userdata Context value to be passed to the getfn and setfn callbacks.
 *
 * @return  none.
 *
 * @remarks If you wish to override default key-store, this method must be called before initialization.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_SetKeystoreOverride( GetKeystoreItem_t getfn, SetKeystoreItem_t setfn, void * userdata ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_RegisterMessageCallback( AnzuNewMessageCallback_t callback, void * userdata )
 *
 * @brief   Register a direct callback for receiving new messages.
 *
 * @details You should prefer working with callbacks, since they allow you to handle the message as soon as possible, rather
 *          than have messages queued on SDK and queried using GetMessages or GetMessagesMalloced.
 *
 * @param   callback The callback for handling a new message.
 * @param   userdata Context value to be passed back to the AnzuNewMessageCallback_t callback.
 *
 * @return  none.
 *
 * @remarks This callback will deliver you the message and remove it from the queue if it was queued.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_RegisterMessageCallback( AnzuNewMessageCallback_t callback, void * userdata ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_RegisterMessageEventCallback( AnzuNewMessageEventCallback_t callback, void * userdata )
 *
 * @brief   Register a direct callback for receiving the availability of new message.
 *
 * @details You might want to use this when using GetMessages or GetMessagesMalloc to avoid pooling fore new messages where
 *          there are no messages pending.
 *
 * @param   callback The callback for handling a new message availability.
 * @param   userdata Context value to be passed back to the AnzuNewMessageEventCallback_t callback.
 *
 * @return  none.
 *
 * @remarks This callback does not provide the message and does not remove it from the queue.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_RegisterMessageEventCallback( AnzuNewMessageEventCallback_t callback, void * userdata ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_RegisterTextureUpdateCallback( AnzuTextureUpdatedCallback_t callback, void * userdata )
 *
 * @brief   Register a direct callback for receiving texture update (channel dirty) notifications.
 *
 * @details By registering this callback you will get notification whenever a channel's content is updated, and the texture
 *          buffer need to be updated accordingly.
 *
 * @param   callback The callback for handling a channel dirty notifications.
 * @param   userdata Context value to be passed back to the AnzuNewMessageEventCallback_t callback.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_RegisterTextureUpdateCallback( AnzuTextureUpdatedCallback_t callback, void * userdata ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_RegisterTextureImpressionCallback( AnzuTextureImpressionCallback_t callback, void * userdata )
 *
 * @brief   Register a direct callback for receiving impression notifications.
 *
 * @param   callback The callback for handling a channel impression notifications.
 * @param   userdata Context value to be passed back to the AnzuTextureUpdatedCallback_t callback.
 *
 * @return  none.
 *
 * @remarks Functional SDK integration does not require any handling for impression events.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_RegisterTextureImpressionCallback( AnzuTextureImpressionCallback_t callback, void * userdata ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_RegisterTexturePlacementProgressCallback( AnzuTexturePlacementProgressCallback_t callback, void * userdata )
 *
 * @brief   Register a direct callback for impression progress notifications.
 *
 * @param   callback The callback for handling a channel impression progress notifications.
 * @param   userdata Context value to be passed back to the AnzuTexturePlacementProgressCallback_t callback.
 *
 * @return  none.
 *
 * @remarks Functional SDK integration does not require any handling for impression progress events.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_RegisterTexturePlacementProgressCallback( AnzuTexturePlacementProgressCallback_t callback, void * userdata ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_RegisterTexturePlaybackProgressCallback( AnzuTexturePlaybackProgressCallback_t callback, void * userdata )
 *
 * @brief   Register a direct callback for playback progress notifications.
 *
 * @param   callback The callback for handling a channel playback progress notifications.
 * @param   userdata Context value to be passed back to the AnzuTexturePlaybackProgressCallback_t callback.
 *
 * @return  none.
 *
 * @remarks Functional SDK integration does not require any handling for playback progress events.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_RegisterTexturePlaybackProgressCallback( AnzuTexturePlaybackProgressCallback_t callback, void * userdata ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_RegisterTexturePlaybackCompleteCallback( AnzuTexturePlaybackCompleteCallback_t callback, void * userdata )
 *
 * @brief   Register a direct callback for playback complete notifications.
 *
 * @param   callback The callback for handling a channel playback complete notifications.
 * @param   userdata Context value to be passed back to the AnzuTexturePlaybackCompleteCallback_t callback.
 *
 * @return  none.
 *
 * @remarks Functional SDK integration does not require any handling for playback complete events.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_RegisterTexturePlaybackCompleteCallback( AnzuTexturePlaybackCompleteCallback_t callback, void * userdata ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_RegisterTextureInitCallback( AnzuTextureInitCallback_t callback, void * userdata )
 *
 * @brief   Register a direct callback for handling texture init notifications.
 *
 * @details Texture init notification are sent when a new media is about to be played on the channel, the callback delivers
 *          a token, that identified the validity of all succeeding events that relies to this playback.
 *
 * @param   callback The callback for handling a channel init notifications.
 * @param   userdata Context value to be passed back to the AnzuTextureInitCallback_t callback.
 *
 * @return  none.
*/
EXTN EXPORT void ANZU_CALLCONV Anzu_RegisterTextureInitCallback( AnzuTextureInitCallback_t callback, void * userdata ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_RegisterTexturePlaybackInfoCallback( AnzuTexturePlaybackInfoCallback_t callback, void * userdata )
 *
 * @brief   Register a direct callback for handling playback info notifications.
 *
 * @param   callback The callback for handling a playback info notifications.
 * @param   userdata Context value to be passed back to the AnzuTexturePlaybackInfoCallback_t callback.
 *
 * @return  none.
*/
EXTN EXPORT void ANZU_CALLCONV Anzu_RegisterTexturePlaybackInfoCallback( AnzuTexturePlaybackInfoCallback_t callback, void* userdata ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_RegisterLogCallback( AnzuLogCallback_t callback, void * userdata )
 *
 * @brief   Register a direct callback for handling log messages sent by the native SDK.
 *
 * @param   callback The callback for handling a log messages.
 * @param   userdata Context value to be passed back to the AnzuLogCallback_t callback.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_RegisterLogCallback( AnzuLogCallback_t callback, void * userdata ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_RegisterTextureAuditColorCallback( AnzuTextureAuditColorCallback_t callback, void * userdata )
 *
 * @brief   Register a callback for color auditing integrations.
 *
 * @details Color auditing allows external third-parties to register for pixel-color auditing, in this case, for selected apps,
 *          medias, sessions, users, at specific time a specific pixel is requested to be captured from the display by the
 *          third party auditing implementation, and a statistical data is collected by auditor regarding the color accuracy
 *          of the placement.
 *
 * @param   callback The callback for handling a color auditing requests.
 * @param   userdata Context value to be passed back to the AnzuTextureAuditColorCallback_t callback.
 *
 * @return  none.
 *
 * @remarks Functional SDK integration does not require any handling for color auditing requests.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_RegisterTextureAuditColorCallback( AnzuTextureAuditColorCallback_t callback, void * userdata ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_RegisterUriSchemaHook( const char * schema, UriSchemaHookCallback_t callback, void * userdata )
 *
 * @brief   Register a callback hooking a uri schema, fired by a callurl action.
 *
 * @param   schema The uri schema to hook to, the "://" postfix should not be included.
 * @param   callback The callback for handling a uri schema hook.
 * @param   userdata Context value to be passed back to the AnzuTextureAuditColorCallback_t callback.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_RegisterUriSchemaHook( const char * schema, UriSchemaHookCallback_t callback, void * userdata ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_MessageUriSchemaListener( const char * schema, const char * message )
 *
 * @brief   Send message to the schema listener.
 *
 * @param   schema The uri schema listener to send messahe to.
 * @param   message The message to send to the schema listener, the format of the message depends on the protocol agreed by the schema user/service.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_MessageUriSchemaListener( const char * schema, const char * message ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_RegisterNetworkCallback( AnzuNetworkCallback_t callback, void * userdata )
 *
 * @brief   Register a callback for SDK network activity.
 *
 * @param   callback The callback for handling network activity changes.
 * @param   userdata Context value to be passed back to the AnzuNetworkCallback_t callback.
 *
 * @return  none.
 *
 * @remarks Functional SDK integration does not require any handling network activity.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_RegisterNetworkCallback( AnzuNetworkCallback_t callback, void * userdata ) ANZU_DUMMY_IMP();

//most important messages to handle are texture:init (you wont receive it through get messages if you registered the init callback)
//and texture:info in which among other things you get the texture *width* and *height*. once you resized your texture to these dimensions
//you should call ResumePlayback, and you will start getting texture updates

/**
 * @fn      const char * Anzu_GetMessages( void )
 *
 * @brief   Dequeue and get all pending messages.
 *
 * @details Instead of constantly pooling for new messages, a better approach would be to register for message callback using
 *          Anzu_RegisterMessageCallback in which you receive and handle the message, or Anzu_RegisterMessageEventCallback, in
 *          which you will get notified about the availability of new messages.
 *
 * @return  A String representing a JSON array containing messages, or NULL if no messages are waiting.
 *
 * @remarks The string pointer returned by this call is valid only until the next call, if you intent to manage the messages
 *          later on, you should create your own copy of the messages.
 */
EXTN EXPORT const char * ANZU_CALLCONV Anzu_GetMessages( void ) ANZU_DUMMY_IMP(NULL);

/**
 * @fn      const char * Anzu_GetMessagesMalloced( void )
 *
 * @brief   Dequeue and get all pending messages, gaining ownership on returned string memory.
 *
 * @details This function is the same as Anzu_GetMessages, however, the returned string is managed by the caller, and he should
 *          Anzu_FreeString() it once he is done handling it.
 *
 * @return  A String representing a JSON array containing messages, which caller must Anzu_FreeString() , or NULL if no messages are
 *          waiting.
 */
EXTN EXPORT const char * ANZU_CALLCONV Anzu_GetMessagesMalloced( void ) ANZU_DUMMY_IMP(NULL);

/**
 * @fn      bool Anzu_HandlesEntrypoint( const char * name )
 *
 * @brief   Check if a full screen interstitial is available for display.
 *
 * @param   name A name for the entrypoint describing the game event, for example "new game", or "level complete".
 *
 * @return  Boolean specifying the availability if interstitial ready for display, true means available, false means not.
 *
 * @remarks We do not encourage full screen interstitial (or any kind of interstitial) but we do support it.
 */
EXTN EXPORT bool ANZU_CALLCONV Anzu_HandlesEntrypoint( const char * name ) ANZU_DUMMY_IMP(false);

/**
 * @fn      void Anzu_Entrypoint( const char * name )
 *
 * @brief   Show a full screen interstitial if available for display.
 *
 * @param   name A name for the entrypoint describing the game event, for example "new game", or "level complete".
 *
 * @return  none.
 *
 * @remarks We do not encourage full screen interstitial (or any kind of interstitial) but we do support it.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_Entrypoint( const char * name ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_CampaignSync( void )
 *
 * @brief   Causes the logic to re-calculate campaign assignments.
 *
 * @return  none.
 *
 * @remarks There is no need to call this function, and on most cases would have no logical effect. The SDK will re-calculate
 *          campaign assignment automatically whenever needed.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_CampaignSync( void ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu__Texture_SetMaximized( int channelId )
 *
 * @brief   notifies the SDK that the camera has zoomed in on a placement.
 *
 * @details This method provides the SDK with the ability to know when a zoom-in operation has completed.
 *
 * @param   channelId The ID of the channel.
 *
 * @return  none.
 *
 * @remarks Functional SDK integration does not require any handling maximize requests.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu__Texture_SetMaximized( int channelId ) ANZU_DUMMY_IMP();

//metrics allows advanced filter and actions, creating campaign funnels, reward systems etc.
//never set system metrics if you don't know what you are doing

/**
 * @fn      bool Anzu_MetricKeyExists( const char * name )
 *
 * @brief   Check if a user domain metric key-value entry exists.
 *
 * @param   name The user-metric key name.
 *
 * @return  Boolean specifying whether the metric key exists, true means yes, false means no.
 */
EXTN EXPORT bool ANZU_CALLCONV Anzu_MetricKeyExists( const char * name ) ANZU_DUMMY_IMP(false);

/**
 * @fn      float Anzu_MetricSet( const char * name, float value, Anzu_MetricOperation_t operation )
 *
 * @brief   Set or update a numeric user domain metric key-value entry.
 *
 * @param   name The user-metric key name.
 * @param   value The numeric value to set or update.
 * @param   operation The numeric operation to perform on the entry.
 *
 * @return  The new value of the metric after operation is performed.
 *
 * @remarks Metrics are stored on client side and persist between sessions.
 */
EXTN EXPORT float ANZU_CALLCONV Anzu_MetricSet( const char * name, float value, Anzu_MetricOperation_t operation ) ANZU_DUMMY_IMP(0);

/**
 * @fn      float Anzu_SystemMetricSet( const char * name, float value, Anzu_MetricOperation_t operation )
 *
 * @brief   Set or update a numeric system domain metric key-value entry.
 *
 * @param   name The system-metric key name.
 * @param   value The numeric value to set or update.
 * @param   operation The numeric operation to perform on the entry.
 *
 * @return  The new value of the metric after operation is performed.
 *
 * @remarks Metrics are stored on client side and persist between sessions. You should not update system metrics without
 *          consulting the anzu team.
 */
EXTN EXPORT float ANZU_CALLCONV Anzu_SystemMetricSet( const char * name, float value, Anzu_MetricOperation_t operation ) ANZU_DUMMY_IMP(0);

/**
 * @fn      float Anzu_MetricGet( const char * name )
 *
 * @brief   Query numeric user domain metric key-value entry.
 *
 * @param   name The user-metric key name.
 *
 * @return  The value of the metric, if no metric exists, 0 is returned.
 *
 * @remarks Metrics are stored on client side and persist between sessions.
 */
EXTN EXPORT float ANZU_CALLCONV Anzu_MetricGet( const char * name ) ANZU_DUMMY_IMP(0);

/**
 * @fn      float Anzu_CampaignMetricGet( const char * campaignId, const char * name )
 *
 * @brief   Query numeric campaign domain metric key-value entry.
 *
 * @param   name The campaign-metric key name.
 * @param   campaignId The campaign ID metrica value is queried for.
 *
 * @return  The value of the metric, if no metric exists, 0 is returned.
 *
 * @remarks Metrics are stored on client, campaign metrics do not persist between sessions.
 */
EXTN EXPORT float ANZU_CALLCONV Anzu_CampaignMetricGet( const char * campaignId, const char * name ) ANZU_DUMMY_IMP(0);

/**
 * @fn      float Anzu_SystemMetricGet( const char * name )
 *
 * @brief   Query numeric system domain metric key-value entry.
 *
 * @param   name The system-metric key name.
 *
 * @return  The value of the metric, if no metric exists, 0 is returned.
 *
 * @remarks Metrics are stored on client side and persist between sessions. You should not update system metrics without
 *          consulting the anzu team.
 */
EXTN EXPORT float ANZU_CALLCONV Anzu_SystemMetricGet( const char * name ) ANZU_DUMMY_IMP(0);

/**
 * @fn      void Anzu_MetricSetS( const char * name, const char * value, Anzu_MetricOperation_t operation )
 *
 * @brief   Set or update a string user domain metric key-value entry.
 *
 * @param   name The user-metric key name.
 * @param   value The string value to set or update.
 * @param   operation The numeric operation to perform on the entry.
 *
 * @return  none.
 *
 * @remarks Metrics are stored on client side and persist between sessions.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_MetricSetS( const char * name, const char * value, Anzu_MetricOperation_t operation ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_HashedMetricSetS( const char * name, const char * value, Anzu_MetricHashAlgorithm_t hashAlgorithm )
 *
 * @brief   Set or update a string user domain metric key-value entry, the string will be stored hash.
 *
 * @param   name The user-metric key name.
 * @param   value The string value to set or update.
 * @param   hashAlgorithm The hash algorithm to use when storing the value.
 *
 * @return  none.
 *
 * @remarks Metrics are stored on client side and persist between sessions.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_HashedMetricSetS( const char * name, const char * value, Anzu_MetricHashAlgorithm_t hashAlgorithm ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_SystemMetricSetS( const char * name, const char * value, Anzu_MetricOperation_t operation )
 *
 * @brief   Set or update a string system domain metric key-value entry.
 *
 * @param   name The system-metric key name.
 * @param   value The string value to set or update.
 * @param   operation The numeric operation to perform on the entry.
 *
 * @return  none.
 *
 * @remarks Metrics are stored on client side and persist between sessions. You should not update system metrics without
 *          consulting the anzu team.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_SystemMetricSetS( const char * name, const char * value, Anzu_MetricOperation_t operation ) ANZU_DUMMY_IMP();

/**
 * @fn      const char * Anzu_MetricGetS( const char * name )
 *
 * @brief   Query string user domain metric key-value entry.
 *
 * @param   name The user-metric key name.
 *
 * @return  Allocated zero terminated string containing the value of the metric. It is the caller's responsibility to
 *          Anzu_FreeString() the returned pointer. If no entry with such key exists, a default empty string is 
 *          returned. If name parameter is invalid, NULL is returned.
 *
 * @remarks Metrics are stored on client side and persist between sessions.
 */
EXTN EXPORT const char * ANZU_CALLCONV Anzu_MetricGetS( const char * name ) ANZU_DUMMY_IMP(NULL);

/**
 * @fn      const char * Anzu_CampaignMetricGetS( const char * campaignId, const char * name )
 *
 * @brief   Query string campaign domain metric key-value entry.
 *
 * @param   name The campaign-metric key name.
 * @param   campaignId The campaign ID metrica value is queried for.
 *
 * @return  Allocated zero terminated string containing the value of the metric. It is the caller's responsibility to
 *          Anzu_FreeString() the returned pointer. If no entry with such key exists, a default empty string is 
 *          returned. If name parameter is invalid, NULL is returned.
 *
 * @remarks Metrics are stored on client, campaign metrics do not persist between sessions.
 */
EXTN EXPORT const char * ANZU_CALLCONV Anzu_CampaignMetricGetS( const char * campaignId, const char * name ) ANZU_DUMMY_IMP(NULL);


/**
 * @fn      const char * Anzu_SystemMetricGetS( const char * name )
 *
 * @brief   Query string system domain metric key-value entry.
 *
 * @param   name The system-metric key name.
 *
 * @return  Allocated zero terminated string containing the value of the metric. It is the caller's responsibility to
 *          Anzu_FreeString() the returned pointer. If no entry with such key exists, a default empty string is 
 *          returned. If name parameter is invalid, NULL is returned.
 *
 * @remarks Metrics are stored on client side and persist between sessions. You should not update system metrics without
 *          consulting the anzu team.
 */
EXTN EXPORT const char * ANZU_CALLCONV Anzu_SystemMetricGetS( const char * name ) ANZU_DUMMY_IMP(NULL);


/***
 * @fn      void Anzu_FreeString( void * ptr )
 *
 * @brief   Free the memory that was allocated by various Anzu_***MetricGetS functions.
 *
 * @param   ptr The pointer to const char * that was returned from MetricGetS.
 *
 * @return  none.
 * 
 * @remarks In C++, the free function does not throw exceptions. Instead, if free encounters an issue, its behavior is undefined.
 *  This means you could experience anything from silent corruption of data to crashes, but no exception will be thrown.
 *  Here are a few things to keep in mind:
 * Set to NULL After Free: It's a good practice to set pointers to NULL after freeing them. This way, 
 *  you reduce the risk of accidentally double freeing them, and attempts to use them can be more predictably caught.
 * Double Free: Calling free on the same pointer more than once can lead to undefined behavior. Ensure you don't do this.
 * Invalid Pointers: Passing pointers that were not returned by memory allocation functions or pointers that have been 
 *  corrupted can lead to undefined behavior.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_FreeString( void * ptr ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_SetLogLevel( Anzu_LogLevel_t level )
 *
 * @brief   Set up the filtering level for Anzu SDK log messages.
 *
 * @param   level The filtering level.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_SetLogLevel( Anzu_LogLevel_t level ) ANZU_DUMMY_IMP();

//sending a log message to the game engine:
//be aware that calling from your game engine, would result with it being fired through the log callback
//however you might want to use these log APIs after all because internal debugging allows you to log to file or socket
//or filter their visibility by setting up log level

/**
 * @fn      void Anzu_Debug( const char * fmt, ... )
 *
 * @brief   Send a debug-level log message through the Anzu SDK logging system.
 *
 * @param   fmt The printf style format of the message.
 * @param   ... The printf style arguments to the format specified.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_Debug( const char * fmt, ... ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_Log( const char * fmt, ... )
 *
 * @brief   Send a verbose-level log message through the Anzu SDK logging system.
 *
 * @param   fmt The printf style format of the message.
 * @param   ... The printf style arguments to the format specified.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_Log( const char * fmt, ... ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_Warning( const char * fmt, ... )
 *
 * @brief   Send a warning-level log message through the Anzu SDK logging system.
 *
 * @param   fmt The printf style format of the message.
 * @param   ... The printf style arguments to the format specified.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_Warning( const char * fmt, ... ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_Error( const char * fmt, ... )
 *
 * @brief   Send a error-level log message through the Anzu SDK logging system.
 *
 * @param   fmt The printf style format of the message.
 * @param   ... The printf style arguments to the format specified.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_Error( const char * fmt, ... ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_SetGDPRConsent( bool isAllowed, const char * consentString )
 *
 * @brief   Inform SDK whether GDPR Consent was accepted or declined by the player.
 *
 * @details This function is used to Informs the Anzu SDK about the player consent status, consent could be provided in 2 formats:
 *          Boolean consent in which TRUE means that the user-provided consent and FALSE means that the user did not provide consent.
 *          String consent is provided in an IAB Europe Transparency & Consent Framework 2 (TCF2) format provided by a compliant consent management platform that is registered in the IAB CMP list.
 *          This function should be called right after the consent collection and the SDK should not be initialized until consent status is known.
 *          By default, the SDK will treat every user as not providing consent (consent = FALSE) unless specified otherwise.
 *
 * @param   isAllowed Boolean represent the boolean consent status value, TRUE means that the user-provided consent, and FALSE means that the user did not provide consent.
 * @param   consentString When consent is givent, represent the TCF2 consent string, value can also be null (dont update the current value) or empty string if resetting the current
 *          set string (it myight be set alreaddy by the CMP registry fetchers on android and iOS) to empty.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_SetGDPRConsent( bool isAllowed, const char * consentString ) ANZU_DUMMY_IMP();

/**
 * @fn      void Anzu_SetCoppaRegulated()
 *
 * @brief   Enforce Coppa restrictions for this session. Calling this function overrides the Coppa setting for that
 *          app as was set in the app-settings on anzu management console and forces Coppa regulations.
 *
 * @return  none.
 */
EXTN EXPORT void ANZU_CALLCONV Anzu_SetCoppaRegulated( void ) ANZU_DUMMY_IMP();

//used internally by anzu developers, intentionally uses ambiguous types for parameters and return value
//see AnzuEx.h for value meaning. function allows custom behavior of SDK for different game engines
//including setting up cache behavior, data folder, # of concurrent web tasks, memory allocators, etc.
//coordinate the use of this function with the Anzu team.

/**
 * @fn      void * Anzu_InternalDebugging( void * parmKey, void * parmData )
 *
 * @brief   This function is used internally by Anzu developers to enable special features.
 *
 * @param   parmKey Private parameter.
 * @param   parmData Private parameter.
 *
 * @return  A private value, depending on feature.
 *
 * @remarks Use of this API requires coordination with the Anzu team. Helper functions are available using AnzuEx.h
 */
EXTN EXPORT void * ANZU_CALLCONV Anzu_InternalDebugging( void * parmKey, void * parmData ) ANZU_DUMMY_IMP(NULL);
