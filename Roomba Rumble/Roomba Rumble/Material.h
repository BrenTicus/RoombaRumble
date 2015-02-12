#include <glm/glm.hpp>

class Material{

public:
	glm::vec3 diffuseAlbedo;
	glm::vec3 specularAlbedo;
	glm::vec3 ambient;
	float specularPower;
};