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
		std::string vertexShaderCode = ""
			"#version 330\n"
			"uniform mat4 vl_ModelViewMatrix;\n"
			"uniform mat4 vl_ProjectionMatrix;\n"
			"layout(location = 0) in vec4 vl_Vertex;\n"
			"layout(location = 2) in vec4 vl_Color;\n"
			"out vec4 outColor;\n"
			"void main()\n"
			"{\n"
			"	vec4 eyePos = vl_ModelViewMatrix * vl_Vertex;\n"
			"	gl_Position = vl_ProjectionMatrix * eyePos;\n"
			"	// gl_Position = vl_Vertex;\n"
			"\n"
			"	outColor = vl_Color;\n"
			"\n"
			"	float dist = length(eyePos.xyz);\n"
			"	float att = inversesqrt(0.1f*dist);\n"
			"	gl_PointSize = 2.0f * att;\n"
			"}";
		std::string fragShaderCode = ""
			"#version 330\n"
			"uniform sampler2D tex;\n"
			"in vec4 outColor;\n"
			"out vec4 vFragColor;\n"
			"void main()\n"
			"{\n"
			"	vFragColor = texture(tex, gl_PointCoord) * outColor;\n"
			"	// vFragColor = outColor;\n"
			"	//vFragColor = vec4(1, 1, 1, 1);\n"
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
	}

	void ParticleSystem::prepare(vl::Texture* tex, vl::Transform* trans)
	{
		m_posBuffer = new vl::ArrayFloat4();
		m_colBuffer = new vl::ArrayFloat4();
		m_drawArrays = new vl::DrawArrays();
		m_posBuffer->resize(m_count);
		m_colBuffer->resize(m_count);
		m_posBuffer->bufferObject()->setBufferData(vl::EBufferObjectUsage::BU_STREAM_DRAW);
		m_colBuffer->bufferObject()->setBufferData(vl::EBufferObjectUsage::BU_STREAM_DRAW);
		m_drawArrays->setPrimitiveType(vl::EPrimitiveType::PT_POINTS);
		auto geom = new vl::Geometry();
		geom->setVertexArray(m_posBuffer.get());
		geom->setColorArray(m_colBuffer.get());
		geom->drawCalls()->push_back(m_drawArrays.get());
		geom->convertToVertexAttribs();
		auto effect = new vl::Effect();
		effect->shader()->enable(vl::EEnable::EN_BLEND);
		effect->shader()->enable(vl::EEnable::EN_POINT_SPRITE);
		effect->shader()->enable(vl::EEnable::EN_PROGRAM_POINT_SIZE);
		effect->shader()->gocBlendFunc()->set(vl::EBlendFactor::BF_SRC_ALPHA, vl::EBlendFactor::BF_ONE);
		effect->shader()->gocTextureSampler(0)->setTexture(tex);
		effect->shader()->setRenderState(makeDefaultShader().get());

		m_actor = new vl::Actor(geom, effect, trans);
		m_actor->gocUniform("tex")->setUniformI(0);

		m_actor->actorEventCallbacks()->push_back(
			new vlExt::ActorEventCallbackFunc(
				[this](vl::Actor* actor, vl::real frame_clock, const vl::Camera* cam, vl::Renderable* renderable, const vl::Shader* shader, int pass)
				{
					update(frame_clock);
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
