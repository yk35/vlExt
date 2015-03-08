#pragma once

#include <vlExtParticle/link_config.hpp>
#include <vlCore/Say.hpp>
#include <vlCore/Object.hpp>
#include <vlCore/TypeInfo.hpp>
#include <vlCore/Collection.hpp>
#include <vlGraphics/Actor.hpp>
#include <vlGraphics/Array.hpp>
#include <vlGraphics/DrawArrays.hpp>
#include <vlGraphics/Texture.hpp>
#include <vlGraphics/GLSL.hpp>
#include <vlCore/Transform.hpp>
#include <vlExtParticle/ParticleData.hpp>
#include <vlExtParticle/ParticleEmitter.hpp>
#include <vlExtParticle/ParticleUpdater.hpp>

namespace vlExt
{
	/**
	 * create a particle actor
	 */
	class VLEXT_EXPORT ParticleSystem : public vl::Object
	{
		VL_INSTRUMENT_CLASS(vlExt::ParticleSystem, vl::Object)
	protected:
		ParticleData m_particles;

		size_t m_count;
		vl::real m_prevClock;

		vl::Collection<ParticleEmitter> m_emitters;
		vl::Collection<ParticleUpdater> m_updaters;
		vl::ref<vl::ArrayFloat4> m_posBuffer;
		vl::ref<vl::ArrayFloat4> m_colBuffer;
		vl::ref<vl::DrawArrays> m_drawArrays;
		vl::ref<vl::GLSLProgram> m_shader;
		vl::ref<vl::Actor> m_actor;

	public:
		explicit ParticleSystem(size_t maxCount);
		virtual ~ParticleSystem() { }

		ParticleSystem(const ParticleSystem &) = delete;
		ParticleSystem &operator=(const ParticleSystem &) = delete;

		void prepare(vl::Texture* tex, vl::Transform* trans = 0);

		virtual void update(double dt);
		virtual void reset();

		virtual size_t numAllParticles() const { return m_particles.count(); }
		virtual size_t numAliveParticles() const { return m_particles.countAlive(); }

		vl::Collection<ParticleEmitter>& emitters() { return m_emitters;  }
		vl::Collection<ParticleUpdater>& updaters() { return m_updaters; }

		vl::Collection<ParticleEmitter> const& emitters() const { return m_emitters; }
		vl::Collection<ParticleUpdater> const& updaters() const { return m_updaters; }

		vl::Actor* actor() { return m_actor.get(); }
		vl::GLSLProgram* defShader() { return m_shader.get(); }
	};
}
