#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>

#include "Material.h"

class RendererInfoFetcher{
private:
	std::string utilityInfoFile;

public:
	int numObjs;

	std::vector<std::string> objFileNames;
	std::vector<std::string> textureFileNames;
	std::vector<glm::vec3> startPositions;
	std::vector<std::string> types;
	std::vector<std::string> powerupTypes;
	std::vector<Material> materials;

	RendererInfoFetcher();
	RendererInfoFetcher(std::string iniFile);
	~RendererInfoFetcher();

	void getEMInfo();
	void getRendererInfo();
	void clear();
};