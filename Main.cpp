#include "VizMa/VizMa.hpp"
#include <cmath>




struct TestProject : public vzm::Project<TestProject>
{
	inline unsigned int ij_seeder(float i, float j)
	{
		return i * 12044.0f + j * 6417.9f;
	}
	
	inline float S3(float x)
	{
		return 3.0f * x * x - 2.0f * x * x * x;
	}

	inline float TerrainPolyBase(Vec3 point)
	{
		
		if (std::fabsf(point.y) > 1.1f)
		{
			return std::fabsf(point.y) - 1.0f;
		}

		float i = std::floor(point.x);
		float j = std::floor(point.z);
		float xt = point.x - i;
		float zt = point.z - j;

		float aij = ark::uniform(ij_seeder(i,j));
		float bij = ark::uniform(ij_seeder(i + 1, j));
		float cij = ark::uniform(ij_seeder(i, j + 1));
		float dij = ark::uniform(ij_seeder(i + 1, j + 1));

		float d = aij + (bij - aij) * S3(xt) + (cij - aij) * S3(zt) + (aij - bij - cij + dij) * S3(xt) * S3(zt);
		return d;

	}

	inline float terrain(Vec3 point)
	{
		point.y -= 10.0f;
		Vec3 val_at_point = point;		
		point.y /= 10.0f;
		
		val_at_point.y = TerrainPolyBase(point / 10.0f);

		static Vec3 max_gradient_normal = Vec3(1.0f, 1.0f, 10.0f);

		return 0.5f * ark::SDFFunctionBounder(point, max_gradient_normal, val_at_point);
	}

	

	
	inline vzm::SDFValue scene(Vec3 point)
	{
		vzm::SDFValue out;


		out.dist = terrain(point);
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