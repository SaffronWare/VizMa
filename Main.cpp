#include "VizMa/VizMa.hpp"

struct TestProject : public vzm::Project<TestProject>
{
	inline vzm::SDFValue scene(Vec3 point)
	{
		vzm::SDFValue out;

		out.color = Vec4(0.0f);
		out.dist = (point).flength() - 0.5f;
		if (out.dist <= hit_zero)
		{
			out.color = Vec4(1.0f);
		}

		return out;
	}
};

int main()
{
	std::cout << "Starting TestProject..." << std::endl;
	TestProject project = TestProject();
	project.start();

	return 0;
}