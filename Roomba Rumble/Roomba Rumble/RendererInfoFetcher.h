#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>

class RendererInfoFetcher{
public:
	std::vector<std::string> objFileNames;
	std::vector<std::string> textureFileNames;
	std::vector<glm::vec3> startPositions;
	std::vector<std::string> types;

	RendererInfoFetcher(std::string iniFile);
	~RendererInfoFetcher();
};