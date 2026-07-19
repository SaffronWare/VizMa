#include "VizMa/VizMa.hpp"
#include <cmath>




struct TestProject : public vzm::Project<TestProject>
{
	inline unsigned int ij_seeder(float i, float j, unsigned int seed = 0)
	{
		return i * 12044.0f + j * 6417.9f + 7.0f + 9741458.0f * seed;
	}
	
	inline float S3(float x)
	{
		return 3.0f * x * x - 2.0f * x * x * x;
	}

	inline float TerrainPolyBase(Vec3 point, unsigned int seed = 0)
	{
		
		

		float i = std::floor(point.x);
		float j = std::floor(point.z);
		float xt = point.x - i;
		float zt = point.z - j;

		float aij = ark::uniform(-1.0f, 1.0f, ij_seeder(i,j, seed));
		float bij = ark::uniform(-1.0f, 1.0f, ij_seeder(i + 1, j, seed));
		float cij = ark::uniform(-1.0f, 1.0f, ij_seeder(i, j + 1, seed));
		float dij = ark::uniform(-1.0f, 1.0f, ij_seeder(i + 1, j + 1, seed));

		float d = aij + (bij - aij) * S3(xt) + (cij - aij) * S3(zt) + (aij - bij - cij + dij) * S3(xt) * S3(zt);
		return d;

	}

	inline float terrain(Vec3 point)
	{
		
		Vec3 val_at_point = point;	
		//val_at_point.y = 0.0f;
		
		val_at_point.y = 0.0f;
		for (int i = 0; i < 8; i++)
		{
			val_at_point.y += 20.0f * std::exp2f(-1.0f*i) * TerrainPolyBase(rotateY(point, i) * std::exp2f(i) / 100.0f, i);
		}

		static Vec3 max_gradient_normal = Vec3(1.0f, 1.0f, 5.0f).normalized();

		return 0.2f * ark::SDFFunctionBounder(point, max_gradient_normal, val_at_point);
	}

	

	
	inline vzm::SDFValue scene(Vec3 point)
	{
		vzm::SDFValue out;

		point.y -= 0.0f;
		out.dist = terrain(point);
		out.color = Vec4(0.6f, 1.0f, 0.3f, 1.0f);

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