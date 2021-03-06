// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "sync/internal_api/public/events/configure_get_updates_request_event.h"

#include "base/strings/stringprintf.h"
#include "sync/protocol/proto_enum_conversions.h"
#include "sync/protocol/proto_value_conversions.h"

namespace syncer {

ConfigureGetUpdatesRequestEvent::ConfigureGetUpdatesRequestEvent(
      base::Time timestamp,
      sync_pb::SyncEnums::GetUpdatesOrigin origin,
      const sync_pb::ClientToServerMessage& request)
  : timestamp_(timestamp),
    origin_(origin),
    request_(request) { }

ConfigureGetUpdatesRequestEvent::~ConfigureGetUpdatesRequestEvent() {}

base::Time ConfigureGetUpdatesRequestEvent::GetTimestamp() const {
  return timestamp_;
}

std::string ConfigureGetUpdatesRequestEvent::GetType() const {
  return "Initial GetUpdates";
}

std::string ConfigureGetUpdatesRequestEvent::GetDetails() const {
  return base::StringPrintf("Reason: %s", GetUpdatesOriginString(origin_));
}

std::unique_ptr<base::DictionaryValue>
ConfigureGetUpdatesRequestEvent::GetProtoMessage() const {
  return std::unique_ptr<base::DictionaryValue>(
      ClientToServerMessageToValue(request_, false));
}

std::unique_ptr<ProtocolEvent> ConfigureGetUpdatesRequestEvent::Clone() const {
  return std::unique_ptr<ProtocolEvent>(
      new ConfigureGetUpdatesRequestEvent(timestamp_, origin_, request_));
}

}  // namespace syncer
