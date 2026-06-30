#include "VizMa/VizMa.hpp"

struct TestProject : public vzm::Project<TestProject>
{
	
};

int main()
{
	std::cout << "Starting TestProject..." << std::endl;

	TestProject project = TestProject();
	project.start();

	return 0;
}