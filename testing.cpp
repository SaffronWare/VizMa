#include "VizMa/VizMa.hpp"

struct TestProject : public vzm::Project<TestProject>
{
	TestProject() {}

	~TestProject() {}

	inline Uint32 render_pixel(unsigned int x, unsigned int y)
	{
		return 0xFFFFFFFF;

	}
};

int main()
{
	TestProject project = TestProject();
	project.start();

	return 0;
}