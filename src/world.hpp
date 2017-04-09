#ifndef WORLD_HPP
#define WORLD_HPP
#include "entityobject.hpp"
#include "datatranslation.hpp"

class World
{
public:
	World(std::string filename);
	const std::string getFileName() const;
	void addObject(Object obj);
	void addEntity(Entity* ent);
	void addEntityObject(std::unique_ptr<EntityObject> eo);
	void exportWorld(const std::string& worldName) const;
	void setGravity(Vector3F gravity = Vector3F());
	void setSpawnPoint(Vector3F spawnPoint = Vector3F());
	void setSpawnOrientation(Vector3F spawnOrientation = Vector3F());

	void update(unsigned int fps, Camera& cam, Shader& shader, unsigned int width, unsigned int height, std::vector<std::shared_ptr<Mesh>> allMeshes, std::vector<std::shared_ptr<Texture>> allTextures, std::vector<std::shared_ptr<NormalMap>> allNormalMaps, std::vector<std::shared_ptr<ParallaxMap>> allParallaxMaps) const;
	void updateInstanced(unsigned int fps, Camera& cam, Shader& shader, unsigned int width, unsigned int height, std::vector<std::shared_ptr<Mesh>> allMeshes, std::vector<std::shared_ptr<Texture>> allTextures, std::vector<std::shared_ptr<NormalMap>> allNormalMaps, std::vector<std::shared_ptr<ParallaxMap>> allParallaxMaps) const;
		
	unsigned int getSize() const;
	const std::vector<Object>& getMembers() const;
	const std::vector<Entity*>& getEntities() const;
	const std::vector<std::unique_ptr<EntityObject>>& getEntityObjects() const;
	Vector3F getGravity() const;
	Vector3F getSpawnPoint() const;
	Vector3F getSpawnOrientation() const;
	std::string getWorldLink() const;
private:
	Vector3F gravity, spawnPoint, spawnOrientation;
	const std::string filename;
	static Object retrieveData(const std::string& objectName, MDLF& mdlf);
	static std::unique_ptr<EntityObject> retrieveEOData(const std::string& eoName, MDLF& mdlf);
	std::vector<Object> members;
	std::vector<Entity*> entities;
	std::vector<std::unique_ptr<EntityObject>> entityObjects;
};

#endif