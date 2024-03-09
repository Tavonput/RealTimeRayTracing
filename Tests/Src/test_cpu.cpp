#include "test_header.h"

// -------------------------------------------------------------------------------------------------------------
// 
// Unit testing the classes that are present in the Cpu-Raytracing directory of RayTrace
// 
// -------------------------------------------------------------------------------------------------------------


namespace cpuTest {
	TEST_CLASS(CpuRay)
	{
	public:
	
		TEST_METHOD(correctBufferSize)
		{	//JF acceptance test
			int width = 800;
			int height = 800;
			class CpuRaytracer temp;
			Assert::IsTrue(width * height * 3 == temp.calculateSpace(800, 800));
		}
		TEST_METHOD(checkInit)
		{
			//JF Coverage test for if different heights are applied to cpuraytrace instances, 
			// if debug is enabled and if a file with a specific name is generated 
			// in CpuRaytracer::Init
			int width = 800;
			int height = 800;
			char fileName [] = "cpuRayTraceObject";
			//remove testfile incase it is there from last test
			if (FILE* file = fopen(fileName, "r")) { 
				fclose(file);
				remove(fileName); 
			}
			class CpuRaytracer temp;
			temp.init(800, 800);
			FILE* file = fopen(fileName, "r");
			Assert::IsTrue(file!=NULL);
			Assert::IsTrue(temp.height == height && temp.width == width);
			Assert::IsTrue(temp.debug == 1);
		}
		TEST_METHOD(checkProgressReport)
		{
			//JF acceptance test
			int width = 800;
			int height = 800;
			class CpuRaytracer temp;
			temp.init(width, height);
			Assert::IsTrue(temp.progress==100);
		}
		TEST_METHOD(calcualteHorizontalVectors)
		{   //JF acceptance test
			int width = 800;
			class CpuRaytracer temp;
			Assert::IsTrue(temp.vecHorizontal() != 0);

		}
		TEST_METHOD(testRangedRandom)
		{   //JF acceptance test
			double y = 32;
			double x = 45;
			double result = randomRangedDouble(x, y);
			double result2 = randomRangedDouble(x, y);
			Assert::IsTrue(result!=result2);
			Assert::IsTrue(x<=result&&result<=y);
			Assert::IsTrue(x <= result2 && result2 <= y);
		}
		TEST_METHOD(calcualteVerticalVectors) {
			//JF acceptance test
			int width = 800;
			class CpuRaytracer temp;
			Assert::IsTrue(temp.vecVertical() != 0);
		}

		TEST_METHOD(initVector) {
			//JF acceptance test
			class CpuRaytracer temp;
			CpuRaytracer::vector vec;
			temp.vecotrInit(vec);
			Assert::IsTrue(vec.v1 != 0 && vec.v2 != 0 && vec.v3 != 0);
		}
		TEST_METHOD(randRange0to1) {
			//JF acceptance test
			double temp1  = random_double();
			double temp2 = random_double();
			Assert::IsTrue(temp1 != temp2);
			Assert::IsTrue(temp1 >= 0.0 && temp1 <= 1.0);
			Assert::IsTrue(temp2 >= 0.0 && temp2 <= 1.0);
		}

		TEST_METHOD(testRefletence){
			//JF acceptance test
			double results = reflectance(3.0, 17.0);
			auto temp = (1 - 17.0) / (1 + 17.0);
			temp = temp * temp;
			temp = temp + (1 - temp) * (1 - 3.0) * (1 - 3.0) * (1 - 3.0) * (1 - 3.0) * (1 - 3.0);
			Assert::IsTrue(temp == results);
		}
		TEST_METHOD(GuiSwitchToCPU)
		{ 
			//JF Integration test: tests to see if after calling changeRenderMethod in the GUI 
			//class will the application start with the opposite type of rendering 
			Application::Settings settings;
			class Application app;
			class Gui temp; 
			bool currentMode = settings.cpuRaytracing;
			app.init(settings);
			temp.changeRenderMethod();
			Assert::IsTrue(currentMode != settings.cpuRaytracing);
			temp.cleanup();
			/*app.cleanup();*/ //private can't cleanup
			

		}
		TEST_METHOD(testRadCalculator)
		{
			//JF acceptance test
			double rads = degreeToRad(180);
			//check to make sure it is the same within error
			Assert::IsTrue(((180 * 3.1459) / 180.0) - 0.1 <= rads && rads <= ((180 * 3.1459) / 180.0) + 0.1);
		}
		TEST_METHOD(testviewpoint_height) {
			//JF acceptance test
			double height = 800;
			double focallength = 1.3;
			Assert::IsTrue(viewpoint_height(height, focallength) == height * focallength * 2.0);

		}
	};
}