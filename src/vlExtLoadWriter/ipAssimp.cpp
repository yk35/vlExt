
#include <vlGraphics/Actor.hpp>
#include <vlCore/ResourceLoadWriter.hpp>
#include <vlCore/ResourceDatabase.hpp>
#include <vlCore/String.hpp>
#include <vlCore/VirtualFile.hpp>
#include <vlCore/DiskFile.hpp>
#include <vlGraphics/Array.hpp>
#include <vlGraphics/DrawArrays.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlCore/Image.hpp>
#include <vlGraphics/Texture.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <sstream>

#include "vlExtLoadWriter/ioAssimp.hpp"

namespace {
	vl::fvec4 toVL(aiColor4D const& from)
	{
		return vl::fvec4(from.r, from.g, from.b, from.a);
	}
	vl::fvec4 toVL(aiColor3D const& from, float a = 1.0f)
	{
		return vl::fvec4(from.r, from.g, from.b, a);
	}
	vl::fvec4 toVL(aiVector3D const& from, float w = 1.0f)
	{
		return vl::fvec4(from.x, from.y, from.z, w);
	}
	unsigned int loadTextures(vl::ResourceDatabase* db, vl::Effect* effect, aiMaterial const* material, aiTextureType texType, vl::String const& path)
	{
		int texCount = material->GetTextureCount(texType);
		aiString p;  // filename
		unsigned int uvidx = 0;
		// now only supported diffuse texture!
		for (int texIndex = 0; texIndex < texCount; ++texIndex)
		{
			aiTextureMapMode mapMode;
			if (material->GetTexture(texType, texIndex, &p, nullptr, &uvidx, 0, 0, &mapMode) == AI_SUCCESS)
			{
				vl::String sDir = path.extractPath();
				vl::String sTextureName = vl::String::fromUTF8(p.data);
				vl::String sFullPath = sDir + sTextureName;
				vl::ref<vl::Image> texImage = vl::loadImage(sFullPath);
				if (!texImage.get())
				{
					vl::Log::warning(vl::String("can't load image file: ") + sFullPath);
					continue;
				}
				vl::ref<vl::Texture> tex;
				int texFlg;
				if (aiGetMaterialInteger(material, AI_MATKEY_TEXFLAGS(texType, texIndex), &texFlg) == AI_SUCCESS)
				{
					if (texFlg == aiTextureFlags_UseAlpha)
					{
						texImage->setFormat(vl::EImageFormat::IF_RGBA);
						effect->shader()->enable(vl::EN_BLEND);
					}
					else
					{
						texImage->setFormat(vl::EImageFormat::IF_RGB);
					}
				}
				tex = new vl::Texture(texImage.get());
				tex->setObjectName(sTextureName.toStdString().c_str());
				effect->shader()->gocTextureSampler(uvidx)->setTexture(tex.get());
				effect->shader()->gocTexEnv(uvidx)->setMode(vl::ETexEnvMode::TEM_MODULATE);
				auto texParameter = tex->getTexParameter();
				vl::ETexParamWrap wrap = vl::ETexParamWrap::TPW_CLAMP;
				switch (mapMode)
				{
				case aiTextureMapMode_Clamp:
				case aiTextureMapMode_Decal:
					wrap = vl::ETexParamWrap::TPW_CLAMP;
					break;
				case aiTextureMapMode_Mirror:
					wrap = vl::ETexParamWrap::TPW_MIRRORED_REPEAT;
					break;
				case aiTextureMapMode_Wrap:
					wrap = vl::ETexParamWrap::TPW_REPEAT;
					break;
				}
				texParameter->setWrapS(wrap);
				texParameter->setWrapT(wrap);
				texParameter->setWrapR(wrap);
				texParameter->setMinFilter(vl::ETexParamFilter::TPF_LINEAR);
				texParameter->setMagFilter(vl::ETexParamFilter::TPF_LINEAR);
				db->resources().push_back(tex);
			}
		}
		return texCount;
	}
}

const int TangentAttribIndex = 4;
const int BitangentAttribIndex = 5;

vl::ref<vl::ResourceDatabase> vlExt::loadAssimp(vl::VirtualFile* file)
{
	std::vector<char> data;
	file->load(data);
	using namespace vl;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFileFromMemory(
		data.data(),
		data.size(),
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType,
		file->path().extractFileExtension(false).toStdString().c_str());
	if (!scene)
	{
		vl::Log::warning(importer.GetErrorString());
		throw std::runtime_error(std::string(importer.GetErrorString()));
		//return nullptr;
	}

	int iTotalVertices = 0;
	vl::ref<vl::ResourceDatabase> db = new vl::ResourceDatabase();

	for (int i = 0; i < scene->mNumCameras; ++i)
	{
		auto cam = scene->mCameras[i];
		vl::ref<vl::Camera> vlCam = new vl::Camera();
		vlCam->setObjectName(cam->mName.C_Str());
		vlCam->setNearPlane(cam->mClipPlaneNear);
		vlCam->setFarPlane(cam->mClipPlaneFar);
		vlCam->setFOV(cam->mHorizontalFOV);
		vlCam->setViewMatrixLookAt(toVL(cam->mPosition).xyz(), toVL(cam->mLookAt).xyz(), toVL(cam->mUp).xyz());
		db->resources().push_back(vlCam.get());
		
	}

	for (int i = 0; i < scene->mNumLights; ++i)
	{
		aiLight* l = scene->mLights[i];
		if (l->mType == aiLightSource_AMBIENT)
		{
			// not supported!
			continue;
		}
		vl::ref<vl::Light> vlLight = new vl::Light();
		vlLight->setObjectName(l->mName.C_Str());
		vlLight->setAmbient(toVL(l->mColorAmbient));
		vlLight->setDiffuse(toVL(l->mColorDiffuse));
		vlLight->setSpecular(toVL(l->mColorSpecular));
		vlLight->setConstantAttenuation(l->mAttenuationConstant);
		vlLight->setLinearAttenuation(l->mAttenuationLinear);
		vlLight->setQuadraticAttenuation(l->mAttenuationQuadratic);
		switch (l->mType)
		{
		case aiLightSource_DIRECTIONAL:
			vlLight->setPosition(-toVL(l->mDirection, 0));
			break;
		case aiLightSource_POINT:
			vlLight->setPosition(toVL(l->mPosition));
			break;
		case aiLightSource_SPOT:
			vlLight->setPosition(toVL(l->mPosition));
			vlLight->setSpotDirection(vl::fvec3(l->mDirection.x, l->mDirection.y, l->mDirection.z));
			vlLight->setSpotCutoff(vl::degrees(l->mAngleOuterCone));
			break;
		};
		db->resources().push_back(vlLight);
	}

	std::vector<vl::ref<vl::Effect>> effects;
	for (int i = 0; i < scene->mNumMaterials; ++i)
	{
		const aiMaterial* material = scene->mMaterials[i];
		auto effect = new vl::Effect();
		vl::ref<vl::EnableSet> enableSet = new vl::EnableSet();
		enableSet->enable(vl::EN_DEPTH_TEST);
		effect->shader()->setEnableSet(enableSet.get());
		effect->shader()->gocFrontFace()->set(vl::EFrontFace::FF_CCW);
		effects.push_back(effect);
		aiColor4D col;
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &col))
		{
			effect->shader()->gocMaterial()->setDiffuse(toVL(col));
		}
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &col))
		{
			effect->shader()->gocMaterial()->setAmbient(toVL(col));
		}
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &col))
		{
			effect->shader()->gocMaterial()->setSpecular(toVL(col));
		}
		if (AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &col))
		{
			effect->shader()->gocMaterial()->setEmission(toVL(col));
		}
		float shininess = 0.0;
		unsigned int num;
		aiGetMaterialFloatArray(material, AI_MATKEY_SHININESS, &shininess, &num);
		effect->shader()->gocMaterial()->setShininess(shininess);
		int two_sided;
		if ((AI_SUCCESS == aiGetMaterialInteger(material, AI_MATKEY_TWOSIDED, &two_sided)) && two_sided)
		{
			effect->shader()->disable(vl::EN_CULL_FACE);
		}
		else
		{
			effect->shader()->enable(vl::EN_CULL_FACE);
		}
		loadTextures(db.get(), effect, material, aiTextureType_DIFFUSE, file->path());
		loadTextures(db.get(), effect, material, aiTextureType_SPECULAR, file->path());
		loadTextures(db.get(), effect, material, aiTextureType_NORMALS, file->path());
		db->resources().push_back(effect);
	}
	for (int i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[i];

		ref<ArrayFloat3>  verts = new vl::ArrayFloat3;
		verts->resize(mesh->mNumVertices);
		ref<Geometry> geom = new Geometry;
		geom->setObjectName(mesh->mName.C_Str());
		std::memcpy(verts->ptr(), mesh->mVertices, sizeof(aiVector3D)*mesh->mNumVertices);
		geom->setVertexArray(verts.get());
		bool hasNormal = false;
		if (mesh->mNormals)
		{
			ref<ArrayFloat3>  normals = new ArrayFloat3;
			normals->resize(mesh->mNumVertices);
			std::memcpy(normals->ptr(), mesh->mNormals, sizeof(aiVector3D)*mesh->mNumVertices);
			geom->setNormalArray(normals.get());
			hasNormal = true;
		}
		if (mesh->mColors[0])
		{
			ref<ArrayFloat4> colors = new ArrayFloat4;
			colors->resize(mesh->mNumVertices);
			std::memcpy(colors->ptr(), mesh->mColors[0], sizeof(aiColor4D)*mesh->mNumVertices);
			geom->setColorArray(colors.get());
		}
		if (mesh->mTangents && mesh->mBitangents)
		{
			ref<ArrayFloat3>  tangents = new ArrayFloat3;
			ref<ArrayFloat3>  bitangents= new ArrayFloat3;
			tangents->resize(mesh->mNumVertices);
			bitangents->resize(mesh->mNumVertices);
			std::memcpy(tangents->ptr(), mesh->mTangents, sizeof(aiVector3D)*mesh->mNumVertices);
			std::memcpy(bitangents->ptr(), mesh->mBitangents, sizeof(aiVector3D)*mesh->mNumVertices);
			geom->setVertexAttribArray(TangentAttribIndex, tangents.get(), false);
			geom->setVertexAttribArray(BitangentAttribIndex, bitangents.get(), false);

		}

		bool hasTexture = false;
		for (int tidx = 0; tidx < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++tidx)
		{
			if (mesh->mTextureCoords[tidx])
			{
				ref<ArrayFloat3> t = new ArrayFloat3();
				t->resize(mesh->mNumVertices);
				std::memcpy(t->ptr(), mesh->mTextureCoords[tidx], sizeof(aiVector3D)*mesh->mNumVertices);
				geom->setTexCoordArray(tidx, t.get());
				hasTexture = true;
			}
		}

		ref<DrawElementsUInt> dc = new vl::DrawElementsUInt(vl::PT_TRIANGLES);
		dc->setCount(mesh->mNumFaces * 3);
		dc->indexBuffer()->resize(mesh->mNumFaces * 3);
		for (unsigned int j = 0; j < mesh->mNumFaces; ++j)
		{
			std::memcpy(dc->indexBuffer()->ptr() + sizeof(unsigned int) * 3 * j, mesh->mFaces[j].mIndices, sizeof(unsigned int)*mesh->mFaces[j].mNumIndices);
		}
		geom->drawCalls()->push_back(dc.get());
		int iMeshVertices = mesh->mNumVertices;
		iTotalVertices += iMeshVertices;
		db->resources().push_back(geom);
		auto effect = effects[mesh->mMaterialIndex];
		if (hasNormal)
		{
			effect->shader()->enable(vl::EN_LIGHTING);
		}
		db->resources().push_back(new vl::Actor(geom.get(), effects[mesh->mMaterialIndex].get()));
	}
	return db;

}
vl::ref<vl::ResourceDatabase> vlExt::loadAssimp(const vl::String& path)
{
	vl::ref<vl::DiskFile> f(new vl::DiskFile(path));
	if (!f->exists())
	{
		std::ostringstream strm;
		strm << "not found file: " << path.toStdString();
		throw std::runtime_error(strm.str());
	}
	return loadAssimp(f.get());
}
