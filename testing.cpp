#include "VizMa/VizMa.hpp"

struct TestProject : public vzm::Project<TestProject>
{
	inline float scene(Vec4 point)
	{
		return (point).flength() - 0.5f;
	}
};

int main()
{
	std::cout << "Starting TestProject..." << std::endl;
	TestProject project = TestProject();
	project.start();

	return 0;
}