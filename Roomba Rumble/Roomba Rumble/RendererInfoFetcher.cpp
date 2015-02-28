#include "RendererInfoFetcher.h"

RendererInfoFetcher::RendererInfoFetcher(std::string iniFile)
{
	char id;
	std::string str, buff;
	float f;
	glm::vec3 v;
	std::ifstream reader(iniFile);

	if(reader.is_open())
	{
		while(!reader.eof())
		{
			reader >> str;

			if(str == "obj")
			{
				reader >> buff;
				objFileNames.push_back(buff);
			}
			if(str == "tex")
			{
				reader >> buff;
				textureFileNames.push_back(buff);
			}
			if(str == "type")
			{
				reader >> buff;
				types.push_back(buff);
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

RendererInfoFetcher::~RendererInfoFetcher()
{
}