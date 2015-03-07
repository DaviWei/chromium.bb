// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// IPC messages for geolocation.
// Multiply-included message file, hence no include guard.

#include "content/public/common/geoposition.h"
#include "ipc/ipc_message_macros.h"
#include "url/gurl.h"

#define IPC_MESSAGE_START GeolocationMsgStart

IPC_ENUM_TRAITS_MAX_VALUE(content::Geoposition::ErrorCode,
                          content::Geoposition::ERROR_CODE_LAST)

IPC_STRUCT_TRAITS_BEGIN(content::Geoposition)
  IPC_STRUCT_TRAITS_MEMBER(latitude)
  IPC_STRUCT_TRAITS_MEMBER(longitude)
  IPC_STRUCT_TRAITS_MEMBER(altitude)
  IPC_STRUCT_TRAITS_MEMBER(accuracy)
  IPC_STRUCT_TRAITS_MEMBER(altitude_accuracy)
  IPC_STRUCT_TRAITS_MEMBER(heading)
  IPC_STRUCT_TRAITS_MEMBER(speed)
  IPC_STRUCT_TRAITS_MEMBER(timestamp)
  IPC_STRUCT_TRAITS_MEMBER(error_code)
  IPC_STRUCT_TRAITS_MEMBER(error_message)
IPC_STRUCT_TRAITS_END()

// Messages sent from the browser to the renderer.

// Reply in response to GeolocationHostMsg_RequestPermission.
IPC_MESSAGE_ROUTED2(GeolocationMsg_PermissionSet,
                    int /* bridge_id */,
                    bool /* is_allowed */)

// Messages sent from the renderer to the browser.

// The |bridge_id| representing |host| is requesting permission to access
// geolocation position. This will be replied by GeolocationMsg_PermissionSet.
// TODO(mlamouri): |origin| should be a security origin to guarantee that a
// proper origin is passed.
IPC_MESSAGE_ROUTED3(GeolocationHostMsg_RequestPermission,
                     int /* bridge_id */,
                     GURL /* origin in the frame requesting geolocation */,
                     bool /* user_gesture */)
