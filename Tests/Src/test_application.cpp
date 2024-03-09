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
		TEST_METHOD(OnReset) //Acceptance test
		{
			Camera camera;
			camera.resetPosition();

			Assert::IsTrue(camera.m_eye[0] == 0.0f);
			Assert::IsTrue(camera.m_eye[1] == 0.0f);
			Assert::IsTrue(camera.m_eye[2] == 0.0f);
		}

		TEST_METHOD(OnFly) //Acceptance test
		{
			Camera camera;
			camera.flyMode();

			Assert::IsTrue(camera.m_cameraMode == CameraMode::FLY);
		}
		/*This teste provides path coverage as it tests every single conditional
		statement checks if each relevant variable is appropriately updated or
		left untouched.*/
		TEST_METHOD(OnKeyPress) //Whitebox test
		{
			/*void Camera::onKeyPress(KeyPressEvent event) <-- function being tested
			{
				if (event.key == GLFW_KEY_W) m_wKey = true;
				else if (event.key == GLFW_KEY_A) m_aKey = true;
				else if (event.key == GLFW_KEY_S) m_sKey = true;
				else if (event.key == GLFW_KEY_D) m_dKey = true;
			} */
			Camera camera;
			camera.init(m_cameraInfo);

			auto& queue = EventDispatcher::GetEventQueue();
			queue.push_back(std::make_unique<KeyPressEvent>(GLFW_KEY_Z, 0));
			KeyPressEvent* eventZ = dynamic_cast<KeyPressEvent*>(EventDispatcher::GetEventQueue()[0].get());
			camera.onKeyPress(*eventZ);
			Assert::IsTrue(camera.m_wKey == false);
			Assert::IsTrue(camera.m_aKey == false);
			Assert::IsTrue(camera.m_sKey == false);
			Assert::IsTrue(camera.m_dKey == false);

			queue.push_back(std::make_unique<KeyPressEvent>(GLFW_KEY_W, 0));
			KeyPressEvent* eventW = dynamic_cast<KeyPressEvent*>(EventDispatcher::GetEventQueue()[1].get());
			camera.onKeyPress(*eventW);
			Assert::IsTrue(camera.m_wKey == true);
			Assert::IsTrue(camera.m_aKey == false);
			Assert::IsTrue(camera.m_sKey == false);
			Assert::IsTrue(camera.m_dKey == false);

			queue.push_back(std::make_unique<KeyPressEvent>(GLFW_KEY_A, 0));
			KeyPressEvent* eventA = dynamic_cast<KeyPressEvent*>(EventDispatcher::GetEventQueue()[2].get());
			camera.onKeyPress(*eventA);
			Assert::IsTrue(camera.m_aKey == true);
			Assert::IsTrue(camera.m_wKey == true);
			Assert::IsTrue(camera.m_sKey == false);
			Assert::IsTrue(camera.m_dKey == false);

			queue.push_back(std::make_unique<KeyPressEvent>(GLFW_KEY_S, 0));
			KeyPressEvent* eventS = dynamic_cast<KeyPressEvent*>(EventDispatcher::GetEventQueue()[3].get());
			camera.onKeyPress(*eventS);
			Assert::IsTrue(camera.m_sKey == true);
			Assert::IsTrue(camera.m_aKey == true);
			Assert::IsTrue(camera.m_wKey == true);
			Assert::IsTrue(camera.m_dKey == false);

			queue.push_back(std::make_unique<KeyPressEvent>(GLFW_KEY_D, 0));
			KeyPressEvent* eventD = dynamic_cast<KeyPressEvent*>(EventDispatcher::GetEventQueue()[4].get());
			camera.onKeyPress(*eventD);
			Assert::IsTrue(camera.m_dKey == true);
			Assert::IsTrue(camera.m_sKey == true);
			Assert::IsTrue(camera.m_aKey == true);
			Assert::IsTrue(camera.m_wKey == true);
		}
		/*This tests that the correct event types were pushed onto the
		event queue before they are passed to the camera class. It then checks
		that the camera class correctly responds to the key release. It
		uses bottom-up testing. */
		TEST_METHOD(OnKeyRelease) //Integration test
		{
			Camera camera;
			camera.init(m_cameraInfo);
			camera.m_aKey = true;
			camera.m_sKey = true;
			camera.m_dKey = true;
			auto& queue = EventDispatcher::GetEventQueue();

			queue.push_back(std::make_unique<KeyReleaseEvent>(GLFW_KEY_A, 0));
			queue.push_back(std::make_unique<KeyReleaseEvent>(GLFW_KEY_S, 0));

			Assert::IsTrue(queue[0]->getType() == EventType::KEY_RELEASE);
			Assert::IsTrue(queue[1]->getType() == EventType::KEY_RELEASE);

			KeyReleaseEvent* event1 = dynamic_cast<KeyReleaseEvent*>(EventDispatcher::GetEventQueue()[0].get());
			KeyReleaseEvent* event2 = dynamic_cast<KeyReleaseEvent*>(EventDispatcher::GetEventQueue()[1].get());

			camera.onKeyRelease(*event1);
			camera.onKeyRelease(*event2);

			Assert::IsTrue(camera.m_aKey == false);
			Assert::IsTrue(camera.m_sKey == false);
			Assert::IsTrue(camera.m_dKey == true);
			Assert::IsTrue(camera.m_wKey == false);
		}
		TEST_METHOD(OnKeyPressAndRelease) //Integration/acceptance test
		{
			Camera camera;
			camera.init(m_cameraInfo);
			auto& queue = EventDispatcher::GetEventQueue();
			queue.push_back(std::make_unique<KeyPressEvent>(GLFW_KEY_A, 0));
			queue.push_back(std::make_unique<KeyReleaseEvent>(GLFW_KEY_A, 0));
			KeyPressEvent* event1 = dynamic_cast<KeyPressEvent*>(EventDispatcher::GetEventQueue()[0].get());
			KeyReleaseEvent* event2 = dynamic_cast<KeyReleaseEvent*>(EventDispatcher::GetEventQueue()[1].get());
			Assert::IsTrue(camera.m_aKey == false);
			camera.onKeyPress(*event1);
			Assert::IsTrue(camera.m_aKey == true);
			camera.onKeyRelease(*event2);
			Assert::IsTrue(camera.m_aKey == false);
		}
		TEST_METHOD(KeyPressEventString_WKey) //Acceptance test
		{
			KeyPressEvent event(GLFW_KEY_W, 0);
			std::string expected = "Key Press - W";
			Assert::AreEqual(expected, event.eventString());
		}
		TEST_METHOD(KeyPressEventString_SKey) //Acceptance test
		{
			KeyPressEvent event(GLFW_KEY_S, 0);
			std::string expected = "Key Press - S";
			Assert::AreEqual(expected, event.eventString());
		}
		TEST_METHOD(KeyReleaseEventString_AKey) //Acceptance test
		{
			KeyReleaseEvent event(GLFW_KEY_A, 0);
			std::string expected = "Key Release - A";
			Assert::AreEqual(expected, event.eventString());
		}
		TEST_METHOD(KeyReleaseEventString_WKey) //Acceptance test
		{
			KeyReleaseEvent event(GLFW_KEY_W, 0);
			std::string expected = "Key Release - W";
			Assert::AreEqual(expected, event.eventString());
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

	TEST_CLASS(WindowTest)
	{
	public:
		TEST_METHOD(NonWASD) //Acceptance test
		{

			Window::keyCallback(nullptr, GLFW_KEY_V, 0, GLFW_PRESS, 0);
			Window::keyCallback(nullptr, GLFW_KEY_B, 0, GLFW_PRESS, 0);
			Assert::IsTrue(EventDispatcher::GetEventQueue().size() == 0);
		}

	private:
		Window m_window;
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