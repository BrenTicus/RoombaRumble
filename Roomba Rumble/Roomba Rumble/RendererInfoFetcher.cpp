#include "RendererInfoFetcher.h"

RendererInfoFetcher::RendererInfoFetcher()
{
}

RendererInfoFetcher::RendererInfoFetcher(std::string iniFile)
{
	utilityInfoFile = iniFile;
}

RendererInfoFetcher::~RendererInfoFetcher()
{
}

void RendererInfoFetcher::getEMInfo()
{
	std::string str, buff;
	float f;
	glm::vec3 v;
	std::ifstream reader(utilityInfoFile);

	numObjs = 0;

	if(reader.is_open())
	{
		while(!reader.eof())
		{
			reader >> str;

			if(str == "obj")
			{
				reader >> buff;
				objFileNames.push_back(buff);

				reader >> buff;
				types.push_back(buff);

				if(buff == "powerup")
				{
					reader >> buff;
					powerupTypes.push_back(buff);
				}
				numObjs++;
			}
			if(str == "v")
			{
				reader >> f;
				v.x = f;

				reader >> f;
				v.y = f;

				reader >> f;
				v.z = f;

				startPositions.push_back(v);
			}
			str == "";
		}
		reader.close();
	}
	else
	{
		std::cout << "Couldn't open file\n";
	}
}

void RendererInfoFetcher::getRendererInfo()
{
	std::string str, buff;
	float f;
	glm::vec3 v;
	Material m;
	std::ifstream reader(utilityInfoFile);

	numObjs = 0;

	if(reader.is_open())
	{
		while(!reader.eof())
		{
			reader >> str;

			if(str == "tex")
			{
				reader >> buff;
				textureFileNames.push_back(buff);
				numObjs++;
			}
			if(str == "mat")
			{
				//ambient lighting
				reader >> buff;

				reader >> v.x;
				reader >> v.y;
				reader >> v.z;
				m.ambient = v;

				//diffuse albedo
				reader >> buff;

				reader >> v.x;
				reader >> v.y;
				reader >> v.z;
				m.diffuseAlbedo = v;

				//specular albedo
				reader >> buff;

				reader >> v.x;
				reader >> v.y;
				reader >> v.z;
				m.specularAlbedo= v;

				//specular power
				reader >> buff;

				reader >> f;
				m.specularPower = f;

				materials.push_back(m);
			}
			str == "";
		}
		reader.close();
	}
	else
	{
		std::cout << "Couldn't open file\n";
	}
}

void RendererInfoFetcher::clear()
{
	objFileNames.resize(0);
	textureFileNames.resize(0);
	startPositions.resize(0);
	types.resize(0);
	powerupTypes.resize(0);
	materials.resize(0);
}