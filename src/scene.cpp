#include "scene.hpp"
#include "data.hpp"

Scene::Scene(): spawn_point(Vector3F()), spawn_orientation(Vector3F()), filename({}), resources_path({}), objects({}), entities({}), entity_objects({}), base_lights({}){}

Scene::Scene(std::string filename, std::string resources_path, const std::vector<std::unique_ptr<Mesh>>& all_meshes, const std::vector<std::unique_ptr<Texture>>& all_textures, const std::vector<std::unique_ptr<NormalMap>>& all_normal_maps, const std::vector<std::unique_ptr<ParallaxMap>>& all_parallax_maps, const std::vector<std::unique_ptr<DisplacementMap>>& all_displacement_maps): filename(std::move(filename)), resources_path(std::move(resources_path))
{
	MDLF input(RawFile(this->get_file_name()));
	std::string spawn_point_string = input.get_tag(tz::scene::spawnpoint_tag_name), spawn_orientation_string = input.get_tag(tz::scene::spawnorientation_tag_name);
	// Initialise spawn_point, spawn_orientation and gravity to the values specified in filename MDL file. If no such tags could be found & validated, zero them.
	if(spawn_point_string != mdl::default_string && spawn_orientation_string != mdl::default_string)
	{
		this->spawn_point = tz::util::string::vectorise_list_3<float>(tz::util::string::deformat(spawn_point_string));
		this->spawn_orientation = tz::util::string::vectorise_list_3<float>(tz::util::string::deformat(spawn_orientation_string));
	}
	else
	{
		this->spawn_point = Vector3F();
		this->spawn_orientation = Vector3F();
	}
	// Parse all objects and entity_objects, and add them to the data vectors.
	std::vector<std::string> object_list = input.get_sequence(tz::scene::objects_sequence_name);
	std::vector<std::string> entity_object_list = input.get_sequence(tz::scene::entityobjects_sequence_name);
	for(std::string object_name : object_list)
	{
		this->add_object(Scene::retrieve_object_data(object_name, this->resources_path.value(), input, all_meshes, all_textures, all_normal_maps, all_parallax_maps, all_displacement_maps));
	}
	for(std::string entity_object_name : entity_object_list)
		this->add_entity_object(Scene::retrieve_entity_object_data(entity_object_name, this->resources_path.value(), input, all_meshes, all_textures, all_normal_maps, all_parallax_maps, all_displacement_maps));
}

bool Scene::has_file_name() const
{
	return this-filename.has_value();
}

const std::string& Scene::get_file_name() const
{
	return this->filename.value();
}

void Scene::add_object(Object3D obj)
{
	this->objects.push_back(std::move(obj));
}

// Add a copy of an entity into this scene. It will have this scene's gravity instantly applied to it.
void Scene::add_entity(Entity ent)
{
	// O(1) amortised Ω(1) ϴ(1) amortised
	this->entities.push_back(std::move(ent));
}

// See documentation for Scene::add_entity(Entity).
void Scene::add_entity_object(EntityObject3D eo)
{
	this->entity_objects.push_back(std::move(eo));
}

// Adds a light to this scene, with the shader handle of the corresponding shader that should handle such lighting.
void Scene::add_light(Light light, GLuint shader_program_handle)
{
	// std::map::operator[] is O(log n) where n is size of map
	while(this->base_lights.size() >= tz::graphics::maximum_lights)
		this->base_lights.erase(this->base_lights.begin());
	this->base_lights[light.get_uniforms(shader_program_handle, this->base_lights.size())] = light;
}

void Scene::remove_object(const Object3D& obj)
{
	this->objects.erase(std::remove(this->objects.begin(), this->objects.end(), obj), this->objects.end());
}

void Scene::remove_entity(const Entity& ent)
{
	this->entities.erase(std::remove(this->entities.begin(), this->entities.end(), ent), this->entities.end());
}

void Scene::remove_entity_object(const EntityObject3D& eo)
{
	this->entity_objects.erase(std::remove(this->entity_objects.begin(), this->entity_objects.end(), eo), this->entity_objects.end());
}

void Scene::remove_light(const Light& light)
{
	for(auto it : this->base_lights)
		if(it.second == light)
			this->base_lights.erase(it.first);
}

const std::vector<Object3D>& Scene::get_objects() const
{
	return this->objects;
}

const std::vector<Entity>& Scene::get_entities() const
{
	return this->entities;
}

const std::vector<EntityObject3D>& Scene::get_entity_objects() const
{
	return this->entity_objects;
}

std::size_t Scene::get_size() const
{
	return this->objects.size() + this->entities.size() + this->entity_objects.size();
}

const std::map<std::array<GLint, tz::graphics::light_number_of_uniforms>, Light>& Scene::get_lights() const
{
	return this->base_lights;
}


void Scene::kill_lights()
{
	for(auto& iter : this->base_lights)
	{
		// Kill all the lights in the shader before losing all the data by zeroing their corresponding uniforms
		std::vector<float> pos({0.0f, 0.0f, 0.0f}), colour({0.0f, 0.0f, 0.0f});
		glUniform3fv(iter.first.front(), 1, &(pos[0]));
		glUniform3fv(iter.first[1], 1, &(colour[0]));
		glUniform1f(iter.first[2], 0);
		glUniform1f(iter.first[3], 0);
		glUniform1f(iter.first[4], 0);
	}
}

void Scene::export_scene(const std::string& scene_link) const
{
	const tz::data::Manager data_manager(this->resources_path.value());
	MDLF output = MDLF(RawFile(scene_link));
	output.get_raw_file().write("# Topaz Auto-Generated Scene File", true);
	std::vector<std::string> object_list;
	std::vector<std::string> entity_object_list;
	output.delete_sequence(tz::scene::objects_sequence_name);
	output.delete_sequence(tz::scene::entityobjects_sequence_name);
	
	output.edit_tag(tz::scene::spawnpoint_tag_name, tz::util::string::format(tz::util::string::devectorise_list_3<float>(this->spawn_point)));
	output.edit_tag(tz::scene::spawnorientation_tag_name, tz::util::string::format(tz::util::string::devectorise_list_3<float>(this->spawn_orientation)));
	for(std::size_t i = 0; i < this->objects.size(); i++)
	{
		const std::string object_name = "object" + tz::util::cast::to_string<int>(i);
		object_list.push_back(object_name);
		const Object3D current_object = this->objects[i];
		
		output.edit_tag(object_name + ".mesh", data_manager.resource_name(current_object.get_mesh().get_file_name()));
		for(auto& texture : current_object.get_textures())
		{
			output.edit_tag(object_name + ".texture" + tz::util::cast::to_string<unsigned int>(static_cast<unsigned int>(texture.first)), data_manager.resource_name(texture.second->get_file_name()));
		}
		output.edit_tag(object_name + ".pos", tz::util::string::format(tz::util::string::devectorise_list_3<float>(current_object.position)));
		output.edit_tag(object_name + ".rot", tz::util::string::format(tz::util::string::devectorise_list_3<float>(current_object.rotation)));
		output.edit_tag(object_name + ".scale", tz::util::string::format(tz::util::string::devectorise_list_3<float>(current_object.scale)));
		output.edit_tag(object_name + ".shininess", tz::util::cast::to_string(current_object.shininess));
		output.edit_tag(object_name + ".parallax_map_scale", tz::util::cast::to_string(current_object.parallax_map_scale));
		output.edit_tag(object_name + ".parallax_map_offset", tz::util::cast::to_string(current_object.parallax_map_offset));
		output.edit_tag(object_name + ".displacement_factor", tz::util::cast::to_string(current_object.displacement_factor));
	}
	for(std::size_t i = 0; i < this->entity_objects.size(); i++)
	{
		const std::string entity_object_name = "eo" + tz::util::cast::to_string<float>(i);
		entity_object_list.push_back(entity_object_name);
		const EntityObject3D current_entity_object = this->entity_objects[i];

		output.edit_tag(entity_object_name + ".mesh", data_manager.resource_name(current_entity_object.get_mesh().get_file_name()));
		for(auto& texture : current_entity_object.get_textures())
		{
			output.edit_tag(entity_object_name + ".texture" + tz::util::cast::to_string<unsigned int>(static_cast<unsigned int>(texture.first)), data_manager.resource_name(texture.second->get_file_name()));
		}
		output.edit_tag(entity_object_name + ".mass", tz::util::cast::to_string(current_entity_object.mass));
		output.edit_tag(entity_object_name + ".pos", tz::util::string::format(tz::util::string::devectorise_list_3<float>(current_entity_object.position)));
		output.edit_tag(entity_object_name + ".rot", tz::util::string::format(tz::util::string::devectorise_list_3<float>(current_entity_object.rotation)));
		output.edit_tag(entity_object_name + ".scale", tz::util::string::format(tz::util::string::devectorise_list_3<float>(current_entity_object.scale)));
		output.edit_tag(entity_object_name + ".shininess", tz::util::cast::to_string(current_entity_object.shininess));
		output.edit_tag(entity_object_name + ".parallax_map_scale", tz::util::cast::to_string(current_entity_object.parallax_map_scale));
		output.edit_tag(entity_object_name + ".parallax_map_offset", tz::util::cast::to_string(current_entity_object.parallax_map_offset));
		output.edit_tag(entity_object_name + ".displacement_factor", tz::util::cast::to_string(current_entity_object.displacement_factor));
	}
	output.add_sequence(tz::scene::objects_sequence_name, object_list);
	output.add_sequence(tz::scene::entityobjects_sequence_name, entity_object_list);
}

void Scene::save() const
{
	this->export_scene(this->get_file_name());
}

void Scene::render(const Camera& cam, Shader* shader, unsigned int width, unsigned int height)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	for(auto& object : this->objects)
		object.render(cam, shader, width, height);
	for(auto& entity_object : this->entity_objects)
		entity_object.render(cam, shader, width, height);
	for(auto& iter : this->base_lights)
	{
		Light light = iter.second;
		std::vector<float> pos, colour;
		pos.push_back(light.position.x);
		pos.push_back(light.position.y);
		pos.push_back(light.position.z);
		colour.push_back(light.colour.x);
		colour.push_back(light.colour.y);
		colour.push_back(light.colour.z);
		glUniform3fv(iter.first.front(), 1, &(pos[0]));
		glUniform3fv(iter.first[1], 1, &(colour[0]));
		glUniform1f(iter.first[2], light.power);
		glUniform1f(iter.first[3], light.diffuse_component);
		glUniform1f(iter.first[4], light.specular_component);
	}
}

void Scene::update(unsigned int tps)
{
	for(auto& eo : this->entity_objects)
		eo.update_motion(tps);
	for(auto& ent : this->entities)
		ent.update_motion(tps);
}

Object3D Scene::retrieve_object_data(const std::string& object_name, std::string resources_path, MDLF& mdlf, const std::vector<std::unique_ptr<Mesh>>& all_meshes, const std::vector<std::unique_ptr<Texture>>& all_textures, const std::vector<std::unique_ptr<NormalMap>>& all_normal_maps, const std::vector<std::unique_ptr<ParallaxMap>>& all_parallax_maps, const std::vector<std::unique_ptr<DisplacementMap>>& all_displacement_maps)
{
	std::string mesh_name = mdlf.get_tag(object_name + ".mesh");
	std::string position_string = mdlf.get_tag(object_name + ".pos");
	std::string rotation_string = mdlf.get_tag(object_name + ".rot");
	std::string scale_string = mdlf.get_tag(object_name + ".scale");
	unsigned int shininess = tz::util::cast::from_string<unsigned int>(mdlf.get_tag(object_name + ".shininess"));
	float parallax_map_scale = tz::util::cast::from_string<float>(mdlf.get_tag(object_name + ".parallax_map_scale"));
	float parallax_map_offset = tz::util::cast::from_string<float>(mdlf.get_tag(object_name + ".parallax_map_offset"));
	float displacement_factor = tz::util::cast::from_string<float>(mdlf.get_tag(object_name + ".displacement_factor"));
	if(!mdlf.exists_tag(object_name + ".shininess"))
		shininess = tz::graphics::default_shininess;
	if(!mdlf.exists_tag(object_name + ".parallax_map_scale"))
		parallax_map_scale = tz::graphics::default_parallax_map_scale;
	if(!mdlf.exists_tag(object_name + ".parallax_map_offset"))
		parallax_map_offset = tz::graphics::default_parallax_map_offset;
	if(!mdlf.exists_tag(object_name + ".displacement_factor"))
		displacement_factor = tz::graphics::default_displacement_factor;
	tz::data::Manager data_manager(resources_path);
	
	std::string mesh_link = data_manager.resource_link(mesh_name);
	std::map<tz::graphics::TextureType, Texture*> textures;
	for(unsigned int i = 0; i < static_cast<unsigned int>(tz::graphics::TextureType::TEXTURE_TYPES); i++)
	{
		std::string texture_name = mdlf.get_tag(object_name + ".texture" + tz::util::cast::to_string(i));
		std::string texture_link = data_manager.resource_link(texture_name);
		Texture* tex = nullptr;
		switch(static_cast<tz::graphics::TextureType>(i))
		{
			case tz::graphics::TextureType::TEXTURE:
			default:
				tex = Texture::get_from_link<Texture>(texture_link, all_textures);
				break;
			case tz::graphics::TextureType::NORMAL_MAP:
				tex = Texture::get_from_link<NormalMap>(texture_link, all_normal_maps);
				break;
			case tz::graphics::TextureType::PARALLAX_MAP:
				tex = Texture::get_from_link<ParallaxMap>(texture_link, all_parallax_maps);
				break;
			case tz::graphics::TextureType::DISPLACEMENT_MAP:
				tex = Texture::get_from_link<DisplacementMap>(texture_link, all_displacement_maps);
				break;
		}
		textures.emplace(static_cast<tz::graphics::TextureType>(i), tex);
	}
	return {tz::graphics::find_mesh(mesh_link, all_meshes), textures, tz::util::string::vectorise_list_3<float>(tz::util::string::deformat(position_string)), tz::util::string::vectorise_list_3<float>(tz::util::string::deformat(rotation_string)), tz::util::string::vectorise_list_3<float>(tz::util::string::deformat(scale_string)), shininess, parallax_map_scale, parallax_map_offset, displacement_factor};
}

EntityObject3D Scene::retrieve_entity_object_data(const std::string& entity_object_name, std::string resources_path, MDLF& mdlf, const std::vector<std::unique_ptr<Mesh>>& all_meshes, const std::vector<std::unique_ptr<Texture>>& all_textures, const std::vector<std::unique_ptr<NormalMap>>& all_normal_maps, const std::vector<std::unique_ptr<ParallaxMap>>& all_parallax_maps, const std::vector<std::unique_ptr<DisplacementMap>>& all_displacement_maps)
{
	// No point repeating code from Scene::retrieve_object_data, so call it to receive a valid Object3D. Then parse the mass from the data-file, and cobble all the data together to create the final EntityObject3D. This doesn't really waste memory as Objects are now smaller than before, as pointers << strings.
	Object3D object = Scene::retrieve_object_data(entity_object_name, resources_path, mdlf, all_meshes, all_textures, all_normal_maps, all_parallax_maps, all_displacement_maps);
	std::string mass_string = mdlf.get_tag(entity_object_name + ".mass");
	float mass = tz::util::cast::from_string<float>(mass_string);
	if(!mdlf.exists_tag(entity_object_name + ".mass"))
		mass = tz::physics::default_mass;
	return{&(object.get_mesh()), object.get_textures(), mass, object.position, object.rotation, object.scale, object.shininess, object.parallax_map_scale, object.parallax_map_offset, object.displacement_factor};
}