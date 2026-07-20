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

	inline float terrainY(Vec3 point, float amp, int l)
	{
		float out = 0.0f;
		amp /= 2.0f;
		for (int i = 0; i < l; i++)
		{
			out += amp * std::exp2f(-1.0f * i) * TerrainPolyBase(rotateY(point, i) * std::exp2f(i) / 100.0f, i);
		}
		return out;
	}

	inline float terrain(Vec3 point, int det = 16)
	{
		point.x += 14.0f;
		point.y += 5.0f;
		
		
		Vec3 val_at_point = point;	
		val_at_point.y = 0.0f;
		val_at_point.y += terrainY(point, 40.0f, std::fmaxf(det/2,det -0.08f * (camera_pos - point).flength()));
		

		static Vec3 max_gradient_normal = Vec3(1.0f, 1.0f, 5.0f).normalized();

		return 0.2f * ark::SDFFunctionBounder(point, max_gradient_normal, val_at_point);
	}

	inline Vec3 terrainNormal(Vec3 point)
	{
		static const float off = 0.001f;
		static const float offm = 1 / off;
		float tp = terrain(point, 3);
		return Vec3(
			terrain(point + Vec3(off, 0.0f, 0.0f), 2) - tp,
			terrain(point + Vec3(0.0f, off, 0.0f), 2) - tp,
			terrain(point + Vec3(0.0f, 0.0f, off), 2) - tp
		).normalized();
	}


	

	
	inline vzm::SDFValue scene(Vec3 point)
	{
		

		vzm::SDFValue out;

		point.y -= 0.0f;
		out.dist = terrain(point);
		// based on normal's y comp, the higher it is, blend into this.
	
		if (out.dist <= 2.0f * hit_zero)
		{
			float blending = std::fabsf(terrainNormal(point).y);
			// let me think. If y high -> green
			// if y sort of low -> brown...
			// Pondering... 
			//
			blending*= blending;
			blending *= blending;
			blending *= blending;
			blending *= blending;

			blending *= blending;
			blending *= blending;
			blending *= blending;
			blending *= blending;

			blending *= blending;
			blending *= blending;
	
	


			static Vec4 muddy_hill = Vec4(0.4f, 0.32f, 0.25f, 1.0f);
			static Vec4 grassy_hill = Vec4(0.2f, 0.5f, 0.1f, 1.0f);
			out.color = ark::blend(muddy_hill, grassy_hill, blending);
		}


		return out;
	}

	inline Vec4 sky(Vec3 ray_dir, Vec3 camera_origin)
	{
		static Vec4 low_sky_color = Vec4(0.3f, 0.6f, 1.0f, 1.0f);
		static Vec4 high_sky_color = Vec4(0.6f, 0.8f, 1.0f, 1.0f);

		static float sky_level = 1000.0f;
		
		// find where ray hits sky in xz
		float t = (sky_level - camera_origin.y)/ray_dir.y;
		Vec3 ahit = camera_origin + ray_dir * t;
		ahit -= Vec3(100.0f, 200.0f, 0.0f);
		float cloud_val = (terrainY(ahit * 0.025f, 1.0f, 5) + 1.0f) / 2.0f;
		cloud_val = std::powf(cloud_val, 5.0f) *2.2f;
		//return Vec4(cloud_val, cloud_val, cloud_val, 1.0f);


		
		float t0 = std::asin(ray_dir.y) / (2.0f * pi);
		Vec4 defsky = ark::blend(low_sky_color, high_sky_color, -10 * t0);
		defsky = low_sky_color;
		return ark::blend(defsky, Vec4(1.0f), cloud_val);
	}

	

	inline Vec4 postproc(Vec4 col, Vec3 origin, Vec3 ray_dir, bool hit, float dist)
	{
		//return col;
		float e = 1.0f;
		e = hit ? std::expf(-dist / 500.0f) : std::powf(std::fabsf(ray_dir.y), 0.35f);
		return (col - Vec4(1)) * e + Vec4(1);
		
	}
};

int main()
{

	std::cout << "Starting TestProject..." << std::endl;
	TestProject project = TestProject();
	float scale = 1.5f;
	project.start(1000 * scale, 800 * scale);

	return 0;
}