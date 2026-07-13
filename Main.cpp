#include "VizMa/VizMa.hpp"
#include <cmath>




struct TestProject : public vzm::Project<TestProject>
{
	inline float PolyTerrain(Vec3 point)
	{
		float py = point.y;
		point /= 10.0f;

		int i = point.x;
		int j = point.z;

		float xt = std::fmodf(point.x, 1.0f);
		float zt = std::fmodf(point.z, 1.0f);

		float h = 2 * std::sinf(0.2f * point.x);
		
		return std::abs(py -  h - 20.0f) ;
	}

	inline float SineSDF(Vec3 point)
	{
		Vec3 val = Vec3(point.x, std::sinf(point.x) + 20.0f, point.z);
		return std::fabsf(ark::SDFFunctionBounder(point, Vec3(1.0f, 1.0f, 1.0f), val));
	}

	inline vzm::SDFValue scene(Vec3 point)
	{
		vzm::SDFValue out;


		
		out.dist = SineSDF(point);
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
	project.start(1000, 800);

	return 0;
}