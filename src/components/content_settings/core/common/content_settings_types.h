// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_CONTENT_SETTINGS_CORE_COMMON_CONTENT_SETTINGS_TYPES_H_
#define COMPONENTS_CONTENT_SETTINGS_CORE_COMMON_CONTENT_SETTINGS_TYPES_H_

#include "build/build_config.h"

// A particular type of content to care about.  We give the user various types
// of controls over each of these.
// A Java counterpart will be generated for this enum.
// GENERATED_JAVA_ENUM_PACKAGE: org.chromium.chrome.browser
enum ContentSettingsType {
  // "DEFAULT" is only used as an argument to the Content Settings Window
  // opener; there it means "whatever was last shown".
  CONTENT_SETTINGS_TYPE_DEFAULT = -1,
  CONTENT_SETTINGS_TYPE_COOKIES = 0,
  CONTENT_SETTINGS_TYPE_IMAGES,
  CONTENT_SETTINGS_TYPE_JAVASCRIPT,
  CONTENT_SETTINGS_TYPE_PLUGINS,
  CONTENT_SETTINGS_TYPE_POPUPS,
  CONTENT_SETTINGS_TYPE_GEOLOCATION,
  CONTENT_SETTINGS_TYPE_NOTIFICATIONS,
  CONTENT_SETTINGS_TYPE_AUTO_SELECT_CERTIFICATE,
  CONTENT_SETTINGS_TYPE_FULLSCREEN,
  CONTENT_SETTINGS_TYPE_MOUSELOCK,
  CONTENT_SETTINGS_TYPE_MIXEDSCRIPT,
  CONTENT_SETTINGS_TYPE_MEDIASTREAM,  // Default setting for MIC and CAMERA.
  CONTENT_SETTINGS_TYPE_MEDIASTREAM_MIC,
  CONTENT_SETTINGS_TYPE_MEDIASTREAM_CAMERA,
  CONTENT_SETTINGS_TYPE_PROTOCOL_HANDLERS,
  CONTENT_SETTINGS_TYPE_PPAPI_BROKER,
  CONTENT_SETTINGS_TYPE_AUTOMATIC_DOWNLOADS,
  CONTENT_SETTINGS_TYPE_MIDI_SYSEX,
  CONTENT_SETTINGS_TYPE_PUSH_MESSAGING,
  CONTENT_SETTINGS_TYPE_SSL_CERT_DECISIONS,
#if defined(OS_WIN)
  CONTENT_SETTINGS_TYPE_METRO_SWITCH_TO_DESKTOP,
#elif defined(OS_ANDROID) || defined(OS_CHROMEOS)
  CONTENT_SETTINGS_TYPE_PROTECTED_MEDIA_IDENTIFIER,
#endif
#if defined(OS_ANDROID)
  CONTENT_SETTINGS_TYPE_APP_BANNER,
#endif
  CONTENT_SETTINGS_NUM_TYPES,
};

#endif  // COMPONENTS_CONTENT_SETTINGS_CORE_COMMON_CONTENT_SETTINGS_TYPES_H_
