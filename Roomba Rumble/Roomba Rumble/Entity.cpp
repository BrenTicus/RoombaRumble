#include "Entity.h"

Entity::Entity() {}

Entity::Entity(PhysicsManager* physicsManager, vec3 position)
{
	this->position = position;
	rotation = quat();
	material = physicsManager->physics->createMaterial(0.1f, 0.05f, 0.1f);
	model = (obj*)malloc(sizeof(obj));

	hitbox = physicsManager->addDynamicObject(physicsManager->physics->createShape(PxCapsuleGeometry(0.5f, 1.0f), *material), PxVec3(position.x, position.y, position.z), 1.0f);
}

Entity::~Entity()
{
}

quat Entity::getRotation(){
	return this->rotation;
}

vec3 Entity::getPosition(){
	return this->position;
}

obj* Entity::getModel(){
	return this->model;
}

/*
Runs every frame after physics and rendering are updated.
*/
void Entity::Update()
{
	// Get transform.
	PxTransform pxtransform = hitbox->getGlobalPose();
	// GLM does more for us, so convert the PhysX vectors to GLM ones.
	position = vec3(pxtransform.p.x+0.5, pxtransform.p.y, pxtransform.p.z-0.5);
	rotation = quat(pxtransform.q.w, pxtransform.q.x, pxtransform.q.y, pxtransform.q.z);

	//cout << position.x << " " << position.y << " " << position.z << endl;
}

/*
Takes an OBJ and returns a vector of its vertices.
*/
vector<PxVec3> Entity::objToVectors(obj* model)
{
	vector<PxVec3> vectors;
	vector<GLfloat> vertices = *(model->vertices);

	for (uint i = 0; i < vertices.size() / 4; i++)
	{
		vectors.push_back(PxVec3(vertices[i * 4], vertices[i * 4 + 1], vertices[i * 4 + 2]));
	}

	return vectors;
}

/*
Like the overload, but also puts a list of the face indices in faceOut.
*/
vector<PxVec3> Entity::objToVectors(obj* model, vector<PxU32>* faceOut)
{
	vector<PxVec3> vectors;
	vector<GLfloat> vertices = *(model->vertices);
	vector<GLuint> faces = *(model->faceIndices);

	for (uint i = 0; i < vertices.size() / 4; i++)
	{
		vectors.push_back(PxVec3(vertices[i * 4], vertices[i * 4 + 1], vertices[i * 4 + 2]));
	}
	for (uint i = 0; i < faces.size(); i++)
	{
		faceOut->push_back(faces[i]);
	}

	return vectors;
}

/*
Reads an OBJ file into a struct.
*/
int Entity::readObj(obj* target, char *filename) {
	char ch;
	string str;
	GLfloat f;
	GLuint i;
	ifstream reader(filename);

	// Reset everything in the target.
	target->faceIndices = new vector<GLuint>(0);
	target->normIndices = new vector<GLuint>(0);
	target->texIndices = new vector<GLuint>(0);
	target->normals = new vector<GLfloat>(0);
	target->texVertices = new vector<GLfloat>(0);
	target->vertices = new vector<GLfloat>(0);

	if (reader.is_open()) {
		while (!reader.eof()) {
			reader >> str;

			if (str == "v") {
				reader >> f;
				target->vertices->push_back(f);

				reader >> f;
				target->vertices->push_back(f);

				reader >> f;
				target->vertices->push_back(f);

				target->vertices->push_back(1.0f);
			}
			if (str == "vn"){
				reader >> f;
				target->normals->push_back(f);

				reader >> f;
				target->normals->push_back(f);

				reader >> f;
				target->normals->push_back(f);
			}
			if (str == "vt"){
				reader >> f;
				target->texVertices->push_back(f);

				reader >> f;
				target->texVertices->push_back(f);
			}
			if (str == "f") {
				reader >> i;
				target->faceIndices->push_back(i - 1);
				reader >> ch;
				reader >> i;
				target->texIndices->push_back(i - 1);
				reader >> ch;
				reader >> i;
				target->normIndices->push_back(i - 1);

				reader >> i;
				target->faceIndices->push_back(i - 1);
				reader >> ch;
				reader >> i;
				target->texIndices->push_back(i - 1);
				reader >> ch;
				reader >> i;
				target->normIndices->push_back(i - 1);

				reader >> i;
				target->faceIndices->push_back(i - 1);
				reader >> ch;
				reader >> i;
				target->texIndices->push_back(i - 1);
				reader >> ch;
				reader >> i;
				target->normIndices->push_back(i - 1);
			}
			str = "";
		}
		reader.close();
	}
	else{
		cout << "Couldn't open file.\n";
		return 1;
	}

	return 0;
};