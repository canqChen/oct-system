#include "stdafx.h"
#include "FrameObserver.h"

extern CWnd* mCWnd;
//
// This is our callback routine that will be executed on every received frame.
// Triggered by the API.
//
// Parameters:
//  [in]    pFrame          The frame returned from the API
//
void FrameObserver::FrameReceived(const FramePtr pFrame)
{
	bool bQueueDirectly = true;
	VmbFrameStatusType eReceiveStatus;

	if (VmbErrorSuccess == pFrame->GetReceiveStatus(eReceiveStatus))
	{
		CWinApp *pApp = AfxGetApp();
		if (NULL != pApp)
		{
			if (NULL != mCWnd)
			{
				// Lock the frame queue
				m_FramesMutex.Lock();
				// We store the FramePtr
				m_Frames.push(pFrame);
				// Unlock frame queue
				m_FramesMutex.Unlock();
				// And notify the view about it
				mCWnd->PostMessage(WM_FRAME_READY, eReceiveStatus);
				bQueueDirectly = false;
			}
		}
	}

	// If any error occurred we queue the frame without notification
	if (true == bQueueDirectly)
	{
		m_pCamera->QueueFrame(pFrame);
	}
}

//
// After the view has been notified about a new frame it can pick it up.
// It is then removed from the internal queue
//
// Returns:
//  A shared pointer to the latest frame
//
FramePtr FrameObserver::GetFrame()
{
	// Lock frame queue
	m_FramesMutex.Lock();
	// Pop the frame from the queue
	FramePtr res;
	if (!m_Frames.empty())
	{
		res = m_Frames.front();
		m_Frames.pop();
	}
	// Unlock the frame queue
	m_FramesMutex.Unlock();
	return res;
}

//
// Clears the internal (double buffering) frame queue
//
void FrameObserver::ClearFrameQueue()
{
	// Lock the frame queue
	m_FramesMutex.Lock();
	// Clear the frame queue and release the memory
	std::queue<FramePtr> empty;
	std::swap(m_Frames, empty);
	// Unlock the frame queue
	m_FramesMutex.Unlock();
}
