#include "VizMa/VizMa.hpp"
#include <cmath>




struct TestProject : public vzm::Project<TestProject>
{
	inline float PolyTerrain(Vec3 point)
	{
		float py = point.y;
		point /= 4.0f;

		int i = point.x;
		int j = point.z;

		float xt = std::fmodf(point.x, 1.0f);
		float zt = std::fmodf(point.z, 1.0f);

		float h = 2 * std::sinf(3.0f * point.x);
		
		return std::abs(py -  h - 20.0f) ;


	}

	inline vzm::SDFValue scene(Vec3 point)
	{
		vzm::SDFValue out;
		
		out.dist = PolyTerrain(point);
		out.color = Vec4(1.0f);

		return out;
	}

	inline Vec4 sky(Vec3 ray_dir)
	{
		static Vec4 low_sky_color = Vec4(0.3f, 0.6f, 1.0f, 1.0f);
		static Vec4 high_sky_color = Vec4(0.6f, 0.8f, 1.0f, 1.0f);

		float t0 = std::asin(ray_dir.y) / (2.0f * pi);
		return ark::blend(low_sky_color, high_sky_color, -10 * t0);
	}
};

int main()
{

	std::cout << "Starting TestProject..." << std::endl;
	TestProject project = TestProject();
	project.start();

	return 0;
}