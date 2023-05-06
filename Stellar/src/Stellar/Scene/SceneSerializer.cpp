#include "stlrpch.h"

#include "Entity.h"
#include "Components.h"
#include "SceneSerializer.h"

#define YAML_CPP_API

#include <yaml-cpp/yaml.h>

namespace YAML {
	template<>
	struct convert<glm::vec2> {
		static Node encode(const glm::vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs) {
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4> {
		static Node encode(const glm::vec4& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs) {
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Stellar {
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	SceneSerializer::SceneSerializer(const STLR_Ptr<Scene>& scene) : m_Scene(scene) {

	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity) {
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << "1234";

		if (entity.hasComponent<TagComponent>()) {
			auto& tag = entity.getComponent<TagComponent>().tag;

			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Tag" << YAML::Value << tag;
			out << YAML::EndMap;
		}

		if (entity.hasComponent<TransformComponent>()) {
			auto& transformComponent = entity.getComponent<TransformComponent>();

			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Translation" << YAML::Value << transformComponent.translation;
			out << YAML::Key << "Rotation" << YAML::Value << transformComponent.rotation;
			out << YAML::Key << "Scale" << YAML::Value << transformComponent.scale;
			out << YAML::EndMap;
		}

		if (entity.hasComponent<CameraComponent>()) {
			auto& cameraComponent = entity.getComponent<CameraComponent>();
			auto& cam = cameraComponent.camera;

			out << YAML::Key << "CameraComponnet";
			out << YAML::BeginMap;
			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)cam.getProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << cam.getFov();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << cam.getPerspectiveNear();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << cam.getPerspectiveFar();
			out << YAML::Key << "OrthographicSize" << YAML::Value << cam.getOrthoGraphicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << cam.getOrthographicNear();
			out << YAML::Key << "OrthographicFar" << YAML::Value << cam.getOrthographicFar();
			out << YAML::EndMap;

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.fixedAspectRatio;
			out << YAML::EndMap;
		}

		if (entity.hasComponent<SpriteRendererComponent>()) {
			auto& spriteComponent = entity.getComponent<SpriteRendererComponent>();

			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Color" << YAML::Value << spriteComponent.color;
			out << YAML::Key << "Texture" << YAML::Value << spriteComponent.texture->getPath();
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::serialize(const std::string& path) {
		YAML::Emitter out;

		out << YAML::BeginMap;
		out << YAML::Key << "Scene";
		out << YAML::Value << "Untitled";
		out << YAML::Key << "Entities";
		out << YAML::Value << YAML::BeginSeq;

		m_Scene->m_Registry.each([&](auto entityID) {
			Entity entity = { entityID, m_Scene.raw() };
			if (!entity) return;

			SerializeEntity(out, entity);
		});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
	}

	void SceneSerializer::serializeRT(const std::string& path) {

	}

	bool SceneSerializer::deserialize(const std::string& path) {
		std::ifstream stream(path);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"]) return false;

		std::string sceneName = data["Scene"].as<std::string>();
		STLR_CONSOLE_LOG_DEBUG("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (entities) {
			for (auto entity : entities) {
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();
				STLR_CONSOLE_LOG_DEBUG("Deserialized entity with ID: {0}, name: {1}", uuid, name);

				Entity deserialized = m_Scene->createEntity(name);

				auto transform = entity["TransformComponent"];
				if (transform) {
					auto& tc = deserialized.getComponent<TransformComponent>();
					tc.translation = transform["Translation"].as<glm::vec3>();
					tc.rotation = transform["Rotation"].as<glm::vec3>();
					tc.scale = transform["Scale"].as<glm::vec3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent) {
					auto& cc = deserialized.addComponent<CameraComponent>();

					auto cameraProps = cameraComponent["Camera"];
					cc.camera.setProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.camera.setFov(cameraProps["PerspectiveFOV"].as<float>());
					cc.camera.setPerspectiveNear(cameraProps["PerspectiveNear"].as<float>());
					cc.camera.setPerspectiveFar(cameraProps["PerspectiveFar"].as<float>());

					cc.camera.setOrthoGraphicSize(cameraProps["OrthographicSize"].as<float>());
					cc.camera.setOrthographicNear(cameraProps["OrthographicNear"].as<float>());
					cc.camera.setOrthographicFar(cameraProps["OrthographicFar"].as<float>());

					cc.primary = cameraComponent["Primary"].as<bool>();
					cc.fixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent) {
					auto& src = deserialized.addComponent<SpriteRendererComponent>();
					src.color = spriteRendererComponent["Color"].as<glm::vec4>();

					std::string texturePath = spriteRendererComponent["Texture"].as<std::string>();
					src.texture = texturePath == "" ? Texture2D::Create(ImageFormat::RGBA) : Texture2D::Create(texturePath);
				}
			}
		}

		return true;
	}

	void SceneSerializer::deserializeRT(const std::string& path) {

	}
}