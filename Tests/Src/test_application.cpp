#include "test_header.h"

// -------------------------------------------------------------------------------------------------------------
// 
// Unit testing the classes that are present in the Application directory of RayTrace
// 
// -------------------------------------------------------------------------------------------------------------

// Defining the static event queue here because Visual Studio can't find it in the original event.cpp file
std::vector<std::unique_ptr<Event>> EventDispatcher::s_queue;

namespace ApplicationTest
{
	// ---------------------------------------------------------------------------------------------------------
	// Camera
	//
	TEST_CLASS(CameraTest)
	{
	public:
		TEST_METHOD_INITIALIZE(Initialize)
		{
			m_cameraInfo.windowHeight = 100;
			m_cameraInfo.windowWidth  = 100;
		}
		TEST_METHOD(OnWindowResize)
		{
			{
				auto& queue = EventDispatcher::GetEventQueue();
				queue.push_back(std::make_unique<WindowResizeEvent>(120, 110));
			}

			Camera camera;
			camera.init(m_cameraInfo);

			WindowResizeEvent* event = dynamic_cast<WindowResizeEvent*>(EventDispatcher::GetEventQueue()[0].get());

			camera.onWindowResize(*event);

			Assert::IsTrue(camera.m_width == event->width);
			Assert::IsTrue(camera.m_height == event->height);
		} 

	private:
		Camera::CreateInfo m_cameraInfo;
	};

	// ---------------------------------------------------------------------------------------------------------
	// Event
	//
	TEST_CLASS(EventTest)
	{
	public:
		TEST_METHOD(CheckEventTypeFromQueue)
		{
			auto& queue = EventDispatcher::GetEventQueue();

			queue.push_back(std::make_unique<WindowResizeEvent>(100, 100));
			Assert::IsTrue(queue[0]->getType() == EventType::WINDOW_RESIZE);
		}
	};
}