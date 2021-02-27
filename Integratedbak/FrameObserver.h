#ifndef __FrameObserver__
#define __FrameObserver__

#include <queue>
#include <VimbaCPP/Include/VimbaCPP.h>

using namespace AVT::VmbAPI;

#define WM_FRAME_READY WM_USER + 1

class FrameObserver : virtual public IFrameObserver
{
public:
	//
	// We pass the camera that will deliver the frames to the constructor
	//
	// Parameters:
	//  [in]    pCamera             The camera the frame was queued at
	//
	FrameObserver(CameraPtr pCamera) : IFrameObserver(pCamera) { ; }

	//
	// This is our callback routine that will be executed on every received frame.
	// Triggered by the API.
	//
	// Parameters:
	//  [in]    pFrame          The frame returned from the API
	//
	virtual void FrameReceived(const FramePtr pFrame);

	//
	// After the view has been notified about a new frame it can pick it up.
	// It is then removed from the internal queue
	//
	// Returns:
	//  A shared pointer to the latest frame
	//
	FramePtr GetFrame();

	//
	// Clears the internal (double buffering) frame queue
	//
	void ClearFrameQueue();
private:
	// Since a MFC message cannot contain a whole frame
	// the frame observer stores all FramePtr
	std::queue<FramePtr> m_Frames;
	AVT::VmbAPI::Mutex m_FramesMutex;
};

#endif // !__FrameObserver__