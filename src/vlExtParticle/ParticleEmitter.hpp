#pragma once

#include <vlExtParticle/link_config.hpp>
#include <vlCore/Object.hpp>
#include <vlCore/TypeInfo.hpp>
#include <vlCore/Collection.hpp>


namespace vlExt
{
	class ParticleData;
	class ParticleGenerator;
	/*!
	 * particle emitter
	 */
	class VLEXT_EXPORT ParticleEmitter : public vl::Object
	{
		VL_INSTRUMENT_CLASS(vlExt::ParticleEmitter, vl::Object)
	public:
		/**
		 * constructor
		 */
		ParticleEmitter()
			: m_emitRate(0.0) {}
		/*!
		 * do emit particle
		 */
		void emit(double dt, ParticleData *p);

		/*!
		 * add particle generator
		 */
		void addGenerator(ParticleGenerator* generator);

		/*!
		 * get generators
		 */
		vl::Collection<ParticleGenerator>& generators() { return m_generators;  }
		vl::Collection<ParticleGenerator> const& generators() const { return m_generators; }

		vl::real emitRate() const { return m_emitRate; }
		/**
		 * set to emit particle rate per second
		 */
		void setEmitRate(vl::real rate) { m_emitRate = rate; }
	protected:
		vl::real m_emitRate;
		vl::Collection<ParticleGenerator> m_generators;
	};
}