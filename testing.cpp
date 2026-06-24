#include "VizMa/VizMa.hpp"

struct TestProject : public vzm::Project<TestProject>
{
	TestProject() {}

	~TestProject() {}

	inline float* render_pixel(unsigned int x, unsigned int y)
	{
		return nullptr;

	}
};

int main()
{
	TestProject project = TestProject();
	project.start();

	return 0;
}