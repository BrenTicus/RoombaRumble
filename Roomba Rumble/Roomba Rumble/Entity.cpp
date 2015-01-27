#include "Entity.h"


Entity::Entity(PhysicsManager* physicsManager, vec3 position)
{
	this->position = position;
	rotation = quat();
	material = physicsManager->physics->createMaterial(0.1f, 0.05f, 0.1f);

	hitbox = physicsManager->addDynamicObject(physicsManager->physics->createShape(PxCapsuleGeometry(0.5f, 1.0f), *material), PxVec3(position.x, position.y, position.z), 1.0f);
}


Entity::~Entity()
{
}

int Entity::readObj (char *filename) {
	char c [256];
	char ch;
	string str;
	GLfloat f;
	GLuint i;
	ifstream reader (filename);

	if (reader.is_open()) {
		while (!reader.eof()) {
			reader >> str;

			if (str == "v") {
				reader >> f;
				vertices.push_back (f);

				reader >> f;
				vertices.push_back (f);

				reader >> f;
				vertices.push_back (f);

				vertices.push_back (1.0f);
			}
			if (str == "vn"){
				reader >> f;
				normals.push_back (f);

				reader >> f;
				normals.push_back (f);

				reader >> f;
				normals.push_back (f);
			}
			if (str == "vt"){
				reader >> f;
				texVertices.push_back (f);

				reader >> f;
				texVertices.push_back (f);
			}
			if (str == "f") {
				reader >> i;
				faceIndices.push_back (i-1);
				reader >> ch;
				reader >> i;
				texIndices.push_back(i-1);
				reader >> ch;
				reader >> i;
				normIndices.push_back(i-1);

				reader >> i;
				faceIndices.push_back (i-1);
				reader >> ch;
				reader >> i;
				texIndices.push_back(i-1);
				reader >> ch;
				reader >> i;
				normIndices.push_back(i-1);

				reader >> i;
				faceIndices.push_back (i-1);
				reader >> ch;
				reader >> i;
				texIndices.push_back(i-1);
				reader >> ch;
				reader >> i;
				normIndices.push_back(i-1);
			}
			str = "";
		}
		reader.close();
	}
	else
		return 1;

	return 0;
}

void Entity::Update()
{
	PxTransform pxtransform = hitbox->getGlobalPose();
	position = vec3(pxtransform.p.x, pxtransform.p.y, pxtransform.p.z);
	rotation = quat(pxtransform.q.w, pxtransform.q.x, pxtransform.q.y, pxtransform.q.z);

	//std::cout << position.x << ", " << position.y << ", " << position.z << std::endl;
}
