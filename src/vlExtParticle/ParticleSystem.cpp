#include "vlExtParticle/ParticleSystem.hpp"
#include "vlExtParticle/ParticleGenerator.hpp"
#include "vlExtParticle/ParticleEmitter.hpp"
#include "vlExtParticle/ParticleUpdater.hpp"
#include "vlExtParticle/ParticleData.hpp"
#include <vlGraphics/BufferObject.hpp>
#include <vlGraphics/Geometry.hpp>
#include "vlExtUtility/ActorUtility.hpp"

namespace vlExt
{
	namespace {
		std::string vertexShaderCode = "\n"
			"#version 330\n"
			"uniform mat4x4 matModelview;\n"
			"uniform mat4x4 matProjection;\n"
			"layout(location = 0) in vec4 vVertex;\n"
			"layout(location = 2) in vec4 vColor;\n"
			"out vec4 outColor;\n"
			"void main()\n"
			"{\n"
			"	vec4 eyePos = matModelview * gl_Vertex;\n"
			"	gl_Position = matProjection * eyePos;\n"
			"\n"
			"	outColor = vColor;\n"
			"\n"
			"	float dist = length(eyePos.xyz);\n"
			"	float att = inversesqrt(0.1f*dist);\n"
			"	gl_PointSize = 2.0f * att;\n"
			"}";
		std::string fragShaderCode = "\n"
			"#version 330\n"
			"uniform sampler2D tex;\n"
			"in vec4 outColor;\n"
			"out vec4 vFragColor;\n"
			"void main()\n"
			"{\n"
			"	vFragColor = texture(tex, gl_PointCoord) * outColor;\n"
			"}\n";
		vl::ref<vl::GLSLProgram> makeDefaultShader()
		{
			auto vert = new vl::GLSLVertexShader(vertexShaderCode.c_str());
			auto frag = new vl::GLSLFragmentShader(fragShaderCode.c_str());
			auto prog = new vl::GLSLProgram();
			prog->attachShader(vert);
			prog->attachShader(frag);
			prog->linkProgram();
			return prog;
		}

	}

	ParticleSystem::ParticleSystem(size_t maxCount)
		: m_particles(maxCount), m_prevClock(-1)
	{
		m_count = maxCount;
		for (size_t i = 0; i < maxCount; ++i)
			m_particles.m_alive[i] = false;
	}

	void ParticleSystem::prepare(vl::Texture* tex, vl::Transform* trans)
	{
		m_posBuffer = new vl::ArrayFloat4();
		m_colBuffer = new vl::ArrayFloat4();
		m_drawArrays = new vl::DrawArrays();
		m_posBuffer->resize(m_count);
		m_colBuffer->resize(m_count);
		m_posBuffer->updateBufferObject();
		m_colBuffer->updateBufferObject();
		m_posBuffer->bufferObject()->setBufferData(vl::EBufferObjectUsage::BU_STREAM_DRAW);
		m_colBuffer->bufferObject()->setBufferData(vl::EBufferObjectUsage::BU_STREAM_DRAW);
		auto geom = new vl::Geometry();
		geom->setVertexArray(m_posBuffer.get());
		geom->setColorArray(m_colBuffer.get());
		m_drawArrays = new vl::DrawArrays();
		m_drawArrays->setPrimitiveType(vl::EPrimitiveType::PT_POINTS);
		auto effect = new vl::Effect();
		effect->shader()->enable(vl::EEnable::EN_BLEND);
		effect->shader()->enable(vl::EEnable::EN_PROGRAM_POINT_SIZE);
		effect->shader()->gocBlendFunc()->set(vl::EBlendFactor::BF_SRC_ALPHA, vl::EBlendFactor::BF_ONE);
		effect->shader()->gocTextureSampler(0)->setTexture(tex);
		effect->shader()->setRenderState(makeDefaultShader().get());
		//effect->shader()->gocTexEnv(0)->setMode(vl::ETexEnvMode::TEM_REPLACE);
		//effect->shader()->gocTexEnv(0)->setPointSpriteCoordReplace(true);

		m_actor = new vl::Actor(geom, effect, trans);
		m_actor->setOccludee(false);
		m_actor->actorEventCallbacks()->push_back(
			new vlExt::ActorEventCallbackFunc(
				[this](vl::Actor* actor, vl::real frame_clock, const vl::Camera* cam, vl::Renderable* renderable, const vl::Shader* shader, int pass)
				{
					update(frame_clock);
					vl::mat4 mv = cam->viewMatrix() * actor->transform()->getComputedWorldMatrix();
					actor->gocUniform("matModelview")->setUniformMatrix4f(1, mv.ptr());
					actor->gocUniform("matProjection")->setUniformMatrix4f(1, cam->projectionMatrix().ptr());
					actor->gocUniform("tex")->setUniformI(0);
				}
			));
	}
 
	void ParticleSystem::update(double t)
	{
		if (m_prevClock < 0)
		{
			m_prevClock = t;
		}
		vl::real dt = t - m_prevClock;
		for (size_t i = 0; i < m_emitters.size(); ++i)
		{
			m_emitters[i]->emit(dt, &m_particles);
		}
 
		//for (size_t i = 0; i < m_count; ++i)
		//{
		//	m_particles.m_acc[i] = vl::vec4(0, 0, 0, 0);
		//}
		std::memset(m_particles.m_acc.get(), 0, sizeof(vl::fvec4)*m_count);
 
		for (size_t i = 0; i < m_updaters.size(); ++i)
		{
			m_updaters[i]->update(dt, &m_particles);
		}
		m_posBuffer->bufferObject()->setBufferSubData(0, sizeof(vl::fvec4)*m_particles.countAlive(), m_particles.m_pos.get());
		m_colBuffer->bufferObject()->setBufferSubData(0, sizeof(vl::fvec4)*m_particles.countAlive(), m_particles.m_col.get());
		m_drawArrays->setCount(m_particles.countAlive());
		m_prevClock = t;
	}
 
	void ParticleSystem::reset()
	{
		m_particles.setCountAlive(0);
	}
}
