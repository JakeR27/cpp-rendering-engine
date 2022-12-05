#include "cylinder.hpp"

SimpleMeshData make_cylinder( bool aCapped, std::size_t aSubdivs, Vec3f aColor, Mat44f aPreTransform )
{
	// DONE: Implement cylinder shell generation
	std::vector<Vec3f> pos;

	float prevY = std::cos(0.f);
	float prevZ = std::sin(0.f);
	
	// Generate vertices for the triangles that make the sides of the cylinder
	for(std::size_t i = 0; i < aSubdivs; ++i)
	{
		float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;
			
		float y = std::cos(angle);
		float z = std::sin(angle);
		// Two triangles (= 3*2 positions) create one segment of the cylinder’s shell.
		pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
		pos.emplace_back(Vec3f{ 0.f, y, z });
		pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
			
		pos.emplace_back(Vec3f{ 0.f, y, z });
		pos.emplace_back(Vec3f{ 1.f, y, z });
		pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
			
		prevY = y;
		prevZ = z;
	}

	// Generate vertices for the top and bottom caps of the cylinder
	if (aCapped) {
		// begin again at the first vertex we drew
		float prevY = std::cos(0.f);
		float prevZ = std::sin(0.f);

		// define the center vertex of the top cap
		// x is 1, y is half of height, z is half of width
		Vec3f topCenter = { 1.f, 1.f / float(aSubdivs), 1 / float(aSubdivs) };

		// draw top cap, should be a triangle fan of aSubdiv no. triangles
		for (std::size_t i = 0; i < aSubdivs; ++i)
		{
			float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;

			float y = std::cos(angle);
			float z = std::sin(angle);
			// triangles that start from the middle of the cap and have their base make up the circle
			// specifically this is CLOCKWISE
			pos.emplace_back(topCenter);
			pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
			pos.emplace_back(Vec3f{ 1.f, y, z });

			prevY = y;
			prevZ = z;
		}

		// define the center vertex of the bottom cap
		Vec3f bottomCenter = { 0.f, 1.f / float(aSubdivs), 1 / float(aSubdivs) };

		// draw bottom cap, should be a triangle fan of aSubdiv no. triangles
		for (std::size_t i = 0; i < aSubdivs; ++i)
		{
			float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;

			float y = std::cos(angle);
			float z = std::sin(angle);
			// triangles that start from the middle of the cap and have their base make up the circle
			// specifically this is ANTI-CLOCKWISE
			pos.emplace_back(bottomCenter);
			pos.emplace_back(Vec3f{ 0.f, y, z });
			pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });

			prevY = y;
			prevZ = z;
		}
	}

	for (auto& p : pos)
	{
		Vec4f p4{ p.x, p.y, p.z, 1.f };
		Vec4f t = aPreTransform * p4;
		t /= t.w;
		
		p = Vec3f{ t.x, t.y, t.z };
	}

	std::vector col(pos.size(), aColor);

	return SimpleMeshData{ std::move(pos), std::move(col) };
}
