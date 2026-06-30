#include "VizMa/VizMa.hpp"

struct TestProject : public vzm::Project<TestProject>
{
	TestProject() {}

	~TestProject() {}

	inline Uint32 render_pixel(unsigned int x, unsigned int y)
	{
		return 0xFFFF0000;

	}
};

int main()
{
	std::cout << "Starting TestProject..." << std::endl;

	TestProject project = TestProject();
	project.start();

	return 0;
}