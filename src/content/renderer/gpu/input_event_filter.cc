// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/bind.h"
#include "base/command_line.h"
#include "base/debug/trace_event.h"
#include "base/location.h"
#include "base/message_loop/message_loop_proxy.h"
#include "content/common/input_messages.h"
#include "content/common/view_messages.h"
#include "content/public/common/content_switches.h"
#include "content/renderer/gpu/input_event_filter.h"
#include "ui/gfx/vector2d_f.h"

using WebKit::WebInputEvent;

namespace content {

InputEventFilter::InputEventFilter(
    IPC::Listener* main_listener,
    const scoped_refptr<base::MessageLoopProxy>& target_loop)
    : main_loop_(base::MessageLoopProxy::current()),
      main_listener_(main_listener),
      sender_(NULL),
      target_loop_(target_loop),
      overscroll_notifications_enabled_(false) {
  DCHECK(target_loop_.get());
  overscroll_notifications_enabled_ =
      CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableOverscrollNotifications);
}

void InputEventFilter::SetBoundHandler(const Handler& handler) {
  DCHECK(main_loop_->BelongsToCurrentThread());
  handler_ = handler;
}

void InputEventFilter::DidAddInputHandler(int routing_id,
                                          cc::InputHandler* input_handler) {
  base::AutoLock locked(routes_lock_);
  routes_.insert(routing_id);
}

void InputEventFilter::DidRemoveInputHandler(int routing_id) {
  base::AutoLock locked(routes_lock_);
  routes_.erase(routing_id);
}

void InputEventFilter::DidOverscroll(int routing_id,
                                     gfx::Vector2dF accumulated_overscroll,
                                     gfx::Vector2dF current_fling_velocity) {
  DCHECK(target_loop_->BelongsToCurrentThread());

  if (!overscroll_notifications_enabled_)
    return;

  io_loop_->PostTask(
      FROM_HERE,
      base::Bind(&InputEventFilter::SendMessageOnIOThread, this,
                 ViewHostMsg_DidOverscroll(routing_id,
                                           accumulated_overscroll,
                                           current_fling_velocity)));
}

void InputEventFilter::OnFilterAdded(IPC::Channel* channel) {
  io_loop_ = base::MessageLoopProxy::current();
  sender_ = channel;
}

void InputEventFilter::OnFilterRemoved() {
  sender_ = NULL;
}

void InputEventFilter::OnChannelClosing() {
  sender_ = NULL;
}

// This function returns true if the IPC message is one that the compositor
// thread can handle *or* one that needs to preserve relative order with
// messages that the compositor thread can handle. Returning true for a message
// type means that the message will go through an extra copy and thread hop, so
// use with care.
static bool RequiresThreadBounce(const IPC::Message& message) {
  return IPC_MESSAGE_ID_CLASS(message.type()) == InputMsgStart;
}

bool InputEventFilter::OnMessageReceived(const IPC::Message& message) {
  TRACE_EVENT0("input", "InputEventFilter::OnMessageReceived");

  if (!RequiresThreadBounce(message))
    return false;

  {
    base::AutoLock locked(routes_lock_);
    if (routes_.find(message.routing_id()) == routes_.end())
      return false;
  }

  target_loop_->PostTask(
      FROM_HERE,
      base::Bind(&InputEventFilter::ForwardToHandler, this, message));
  return true;
}

// static
const WebInputEvent* InputEventFilter::CrackMessage(
    const IPC::Message& message,
    ui::LatencyInfo* latency_info) {
  DCHECK(message.type() == InputMsg_HandleInputEvent::ID);

  PickleIterator iter(message);
  const WebInputEvent* event = NULL;
  IPC::ParamTraits<IPC::WebInputEventPointer>::Read(&message, &iter, &event);
  if (latency_info)
    IPC::ParamTraits<ui::LatencyInfo>::Read(&message, &iter, latency_info);
  return event;
}

InputEventFilter::~InputEventFilter() {
}

void InputEventFilter::ForwardToMainListener(const IPC::Message& message) {
  main_listener_->OnMessageReceived(message);
}

void InputEventFilter::ForwardToHandler(const IPC::Message& message) {
  DCHECK(!handler_.is_null());
  DCHECK(target_loop_->BelongsToCurrentThread());

  if (message.type() != InputMsg_HandleInputEvent::ID) {
    main_loop_->PostTask(
        FROM_HERE,
        base::Bind(&InputEventFilter::ForwardToMainListener,
                   this, message));
    return;
  }

  ui::LatencyInfo latency_info;
  const WebInputEvent* event = CrackMessage(message, &latency_info);

  InputEventAckState ack =
      handler_.Run(message.routing_id(), event, latency_info);

  if (ack == INPUT_EVENT_ACK_STATE_NOT_CONSUMED) {
    TRACE_EVENT0("input", "InputEventFilter::ForwardToHandler");
    main_loop_->PostTask(
        FROM_HERE,
        base::Bind(&InputEventFilter::ForwardToMainListener,
                   this, message));
    return;
  }

  SendACK(message, ack);
}

void InputEventFilter::SendACK(const IPC::Message& message,
                               InputEventAckState ack_result) {
  DCHECK(target_loop_->BelongsToCurrentThread());

  io_loop_->PostTask(
      FROM_HERE,
      base::Bind(&InputEventFilter::SendMessageOnIOThread, this,
                 InputHostMsg_HandleInputEvent_ACK(
                     message.routing_id(),
                     CrackMessage(message, NULL)->type,
                     ack_result)));
}

void InputEventFilter::SendMessageOnIOThread(const IPC::Message& message) {
  DCHECK(io_loop_->BelongsToCurrentThread());

  if (!sender_)
    return;  // Filter was removed.

  sender_->Send(new IPC::Message(message));
}

}  // namespace content
