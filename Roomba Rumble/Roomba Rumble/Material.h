#pragma once

#include <glm/glm.hpp>
#include <iostream>

class Material{

public:
	glm::vec3 diffuseAlbedo;
	glm::vec3 specularAlbedo;
	glm::vec3 ambient;
	float specularPower;
	
	void printMat()
	{
		std::cout << ambient.x << " " << ambient.y << " " << ambient.z << "\n"; 
		std::cout << diffuseAlbedo.x << " " << diffuseAlbedo.y << " " << diffuseAlbedo.z << "\n"; 
		std::cout << specularAlbedo.x << " " << specularAlbedo.y << " " << specularAlbedo.z << "\n"; 
		std::cout << specularPower << "\n\n"; 
	}
};