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

		// White-box Test: This test provides Coverage by verifying the camera's mouse position update mechanism. 
		// Method being tested:
		/*void Camera::onMouseMove(MouseMoveEvent event)
		{
			if (!m_leftMouse && !m_rightMouse)
			{
				// No action, just update the mouse position
				m_mousePos.x = static_cast<float>(event.xPos);
				m_mousePos.y = static_cast<float>(event.yPos);
				return;
			}
		}
		*/
		TEST_METHOD(mousePositionUpdate)
		{
			Camera camera;
			camera.init(m_cameraInfo);

			MouseMoveEvent moveEvent(50, 50); 
			camera.onMouseMove(moveEvent);

			Assert::AreEqual(50.0f, camera.m_mousePos.x);
			Assert::AreEqual(50.0f, camera.m_mousePos.y);
		}

		// Integration Test: MousePositionUpdatedOutOfBound
		// This test case is designed to verify the integration between the Camera class and the event handling system
		// The goal is to ensure that, when a MouseMoveEvent is out of bound
		// Approach: Bottom-up testing is employed here:
		// We first ensure that the individual units (Camera and event handling) work on their own.
		// We then need to test their integration by simulating a mouse move event with an extreme values (so it out of bound).
		// This allows us to test how well the Camera class handles unexpected input in coordination with the event system.
		TEST_METHOD(MousePositionUpdatedOutOfBound)
		{	
			Camera camera;
			camera.init(m_cameraInfo);

			MouseMoveEvent moveEvent(100000, 100000); 
			camera.onMouseMove(moveEvent);

			Assert::IsTrue(camera.m_mousePos.x && camera.m_mousePos.y <= 100);
		}

		//acceptance test
		TEST_METHOD(CameraInitialization)
		{
			Camera camera;
			Camera::CreateInfo infotest{};
			infotest.position = glm::vec3(0.0f, 0.0f, 0.0f);
			infotest.windowHeight = 100;
			infotest.windowWidth = 100;
			camera.init(infotest);

			Assert::IsTrue(camera.getPosition() == infotest.position);
			Assert::IsTrue(camera.m_width == infotest.windowWidth);
			Assert::IsTrue(camera.m_height == infotest.windowHeight);

		}
		//acceptance test
		TEST_METHOD(CameraOnMouseClick)
		{
			Camera camera;
		
			MouseClickEvent leftClick(GLFW_MOUSE_BUTTON_LEFT, 0);
			camera.onMouseClick(leftClick);

			Assert::IsTrue(camera.m_leftMouse);

			MouseClickEvent rightClick(GLFW_MOUSE_BUTTON_RIGHT, 0);
			camera.onMouseClick(rightClick);

			Assert::IsTrue(camera.m_rightMouse);
		}

		//acceptance test
		TEST_METHOD(CameraOnMouseRelease)
		{
			Camera camera;
			camera.m_leftMouse = true;
			camera.m_rightMouse = true;

			MouseReleaseEvent leftReleaseEvent(GLFW_MOUSE_BUTTON_LEFT, 0);
			camera.onMouseRelease(leftReleaseEvent);

			Assert::IsFalse(camera.m_leftMouse);

			MouseReleaseEvent rightReleaseEvent(GLFW_MOUSE_BUTTON_RIGHT, 0);
			camera.onMouseRelease(rightReleaseEvent);

			Assert::IsFalse(camera.m_rightMouse);
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

		//acceptance test
		TEST_METHOD(MouseReleaseEventString_LeftButtonNoShift)
		{
			MouseReleaseEvent event(GLFW_MOUSE_BUTTON_LEFT, 0);
			std::string expected = "Mouse Release - LEFT, mod: NONE";
			Assert::AreEqual(expected, event.eventString());
		}

		//acceptance test
		TEST_METHOD(MouseReleaseEventString_RightButtonWithShift)
		{
			MouseReleaseEvent event(GLFW_MOUSE_BUTTON_RIGHT, GLFW_MOD_SHIFT);
			std::string expected = "Mouse Release - RIGHT, mod: SHIFT";
			Assert::AreEqual(expected, event.eventString());
		}

		//acceptance test
		TEST_METHOD(MouseClickEvent_LeftClickWithShiftModifier)
		{
			MouseClickEvent event(GLFW_MOUSE_BUTTON_LEFT, GLFW_MOD_SHIFT);
			std::string expectedOutput = "Mouse Click - LEFT, mod: SHIFT";
			Assert::AreEqual(expectedOutput, event.eventString());
		}

		//acceptance test
		TEST_METHOD(MouseClickEvent_RightClickWithoutModifiers)
		{
			MouseClickEvent event(GLFW_MOUSE_BUTTON_RIGHT, 0);
			std::string expectedOutput = "Mouse Click - RIGHT, mod: NONE";
			Assert::AreEqual(expectedOutput, event.eventString());
		}

		//acceptance test
		TEST_METHOD(WindowMinimizedEventString)
		{
			WindowMinimizedEvent event;
			std::string expected = "Window Minimized";
			Assert::AreEqual(expected, event.eventString());
		}

		//acceptance test
		TEST_METHOD(MouseMoveEventString)
		{
			MouseMoveEvent event(300, 400);
			std::string expected = "Mouse Move - x:300, y:400";
			Assert::AreEqual(expected, event.eventString());
		}
	};
	// ---------------------------------------------------------------------------------------------------------
	// model
	//
	TEST_CLASS(ModelTest)
	{
	public:
		//acceptance test
		TEST_METHOD(testingDef)
		{
			Model::Instance test;
			Assert::IsTrue(test.transform == glm::mat4(1.0f));
			Assert::IsTrue(test.objectID == 0);
		}

	};
}