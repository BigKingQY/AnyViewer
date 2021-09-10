//
//  CoreEventsAdapter.h
//  AnyViewer
//
//  Created by Aomei on 2021/9/10.
//

#ifndef CoreEventsAdapter_h
#define CoreEventsAdapter_h

#pragma once
#include "FrameBuffer.h"
#include "Rect.h"

#include <string>

// --------------------------------------------------------------------------------
/// <summary>
/// CCoreEventsAdapter interface is used to pass events from CRemoteViewerCore to
/// your application.
/// You should inherit this abstract class and override the methods you are
/// interested in. For example, if all you need is to show the contents of the
/// remote screen, then you should override two functions: OnFrameBufferUpdate()
/// and OnFrameBufferPropChange().
/// </summary>
// --------------------------------------------------------------------------------
class CCoreEventsAdapter
{
public:
  //
  // This makes the class abstract, so that no instance can be created.
  //
  virtual ~CCoreEventsAdapter() = 0;

  //
  // Bell event has been received from the server.
  //
  virtual void OnBell();

  //
// Bell event has been received from the server.
//
  virtual void OnCursorShapeChanged(){}

  //
  // New cut text (clipboard) contents has been received from the server.
  //
  virtual void OnCutText(const std::string& pCutText);

  //
  // Connection has been established.
  //
  virtual void OnEstablished();

  //
  // Protocol has entered the normal interactive phase (in other words,
  // protocol initialization has been completed).
  //
  // FIXME: document it.
  // Output need for capability, e.g. FT.
  virtual void OnConnected();

  //
  // CRemoteViewerCore has been Disconnected by calling Stop()
  // or connection with server is Disconnected.
  //
  // FIXME: now, OnDisconnect not called after OnError().
  // FIXME: change documentation or call OnDisconnect() after OnError().
  virtual void OnDisconnect(const std::string& pMsg);


  //
  // Error has been occured.
  //
//  virtual void OnError(const CException *pException);

  // this event after pUpdateRect of frame buffer "pFrameBuffer" in rectangle "pUpdateRect".
  // guaranteed correct of frame buffer's area in rectangle "pUpdateRect".
  //
  // Frame buffer contents has been changed. During this callback,
  // the frame buffer is locked, and the rectangle is guaranteed to be valid
  // (no guarantees about other areas of the frame buffer).
  //
  virtual void OnFrameBufferUpdate(const CFrameBuffer *pFrameBuffer, const Rect *pUpdateRect);

  // changed properties of frame buffer.
  // In this moment frame buffer area is dirty and may be contained incorrect data
  //
  // Some properties of the frame buffer have been changed. Assume that new
  // frame buffer has been created and the old one has been destroyed. This
  // notification will be called on initial frame buffer allocation as well.
  //
  virtual void OnFrameBufferPropChange(const CFrameBuffer *pFrameBuffer);

  // ª∫≥Â«¯≥ﬂ¥Á∏ƒ±‰ ¬º˛
  virtual void OnFrameBufferSizeChange(const unsigned int /*nWidth*/, const unsigned int /*nHeight*/) {}
};




#endif /* CoreEventsAdapter_h */
