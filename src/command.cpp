#include "command.hpp"

void Commands::loadWorld(std::vector<std::string> args, World*& world)
{
	if(args.size() != 2)
	{
		std::cout << "Nonfatal Command Error: Unexpected quantity of args, got " << args.size() << ", expected 2.\n";
		return;
	}
	std::string worldname = args.at(1);
	delete world;
	std::string link = ("./res/data/worlds/" + worldname);
	world = new World(link);
	std::cout << "Now rendering the world '" << worldname << "'.\n";
}

void Commands::exportWorld(std::vector<std::string> args, World*& world)
{
	if(args.size() != 2)
	{
		std::cout << "Nonfatal Command Error: Unexpected quantity of args, got " << args.size() << ", expected 2.\n";
		return;
	}
	std::string worldname = args.at(1);
	world->exportWorld(worldname);
}

void Commands::addObject(std::vector<std::string> args, World*& world, Camera& cam, bool printResults)
{
	if(args.size() != 6)
	{
		std::cout << "Nonfatal Command Error: Unexpected quantity of args, got " << args.size() << ", expected 6.\n";
		return;
	}
	DataTranslation dt("./res/resources.data");
	std::string meshName = args.at(1);
	std::string textureName = args.at(2);
	std::string posStr = args.at(3);
	std::string rotStr = args.at(4);
	std::string scaleStr = args.at(5);

	Vector3F pos, rot, scale;
		
	std::string meshLink = dt.getMeshLink(meshName);
	std::string textureLink = dt.getTextureLink(textureName);
	
	if(meshLink == "_")
	{
		std::cout << "Nonfatal Command Error: Unknown Mesh Name '" << meshName << "'.\n";
		return;
	}
	if(textureLink == "_")
	{
		std::cout << "Nonfatal Command Error: Unknown Texture Name '" << textureName << "'.\n";
		return;
	}
	
	/*
	Mesh* thisMesh;
	Texture* thisTexture;
	for(unsigned int i = 0; i < allMeshes.size(); i++)
	{
		if(meshLink == allMeshes.at(i).getFileName())
			thisMesh = &(allMeshes.at(i));
	}
	for(unsigned int i = 0; i < allTextures.size(); i++)
	{
		if(textureLink == allTextures.at(i).getFileName())
			thisTexture = &(allTextures.at(i));
	}
	*/
	
	if(posStr == "me")
	{
		pos = cam.getPos();
	}
	else
	{
		std::vector<std::string> posSplit = StringUtility::splitString(StringUtility::replaceAllChar(StringUtility::replaceAllChar(posStr, '[', ""), ']', ""), ',');
		pos = Vector3F(MathsUtility::parseTemplateFloat(posSplit.at(0)), MathsUtility::parseTemplateFloat(posSplit.at(1)), MathsUtility::parseTemplateFloat(posSplit.at(2)));
	}
		
	if(rotStr == "me")
	{
		rot = Vector3F(cam.getRot().getX(), cam.getRot().getY(), cam.getRot().getZ());
	}
	else
	{
		std::vector<std::string> rotSplit = StringUtility::splitString(StringUtility::replaceAllChar(StringUtility::replaceAllChar(rotStr, '[', ""), ']', ""), ',');
		rot = Vector3F(MathsUtility::parseTemplateFloat(rotSplit.at(0)), MathsUtility::parseTemplateFloat(rotSplit.at(1)), MathsUtility::parseTemplateFloat(rotSplit.at(2)));	
	}
		
	std::vector<std::string> scaleSplit = StringUtility::splitString(StringUtility::replaceAllChar(StringUtility::replaceAllChar(scaleStr, '[', ""), ']', ""), ',');
	scale = Vector3F(MathsUtility::parseTemplateFloat(scaleSplit.at(0)), MathsUtility::parseTemplateFloat(scaleSplit.at(1)), MathsUtility::parseTemplateFloat(scaleSplit.at(2)));
		
	world->addObject(Object(meshLink, textureLink, pos, rot, scale));
	if(printResults)
	{
		std::cout << "Added the following to this world:\n";
		std::cout << "Mesh name = " << meshName << ", link = " << meshLink << ".\n";
		std::cout << "Texture name = " << textureName << ", link = " << textureLink << ".\n";
		std::cout << "Pos = [" << pos.getX() << ", " << pos.getY() << ", " << pos.getZ() << "].\n";
		std::cout << "Rot = [" << rot.getX() << ", " << rot.getY() << ", " << rot.getZ() << "].\n";
		std::cout << "Scale = [" << scale.getX() << ", " << scale.getY() << ", " << scale.getZ() << "].\n";
	}
}

void Commands::reloadWorld(World*& world, bool printResults)
{
	world->exportWorld("temp.world");
	delete world;
	world = new World("./res/data/worlds/temp.world");
	if(printResults)
		std::cout << "World successfully reloaded via 'temp.world'.\n";
}