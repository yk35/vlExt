#pragma once

#include <vlExtParticle/link_config.hpp>
#include <vlCore/Object.hpp>
#include <vlCore/TypeInfo.hpp>
#include <vlCore/glsl_math.hpp>
#include <vector>

namespace vlExt
{
	class ParticleData;
	/*!
	 * updator for particle status
	 */
	class VLEXT_EXPORT ParticleUpdater : public vl::Object
	{
		VL_INSTRUMENT_ABSTRACT_CLASS(vlExt::ParticleUpdater, vl::Object)
	public:
		ParticleUpdater() { }
		virtual ~ParticleUpdater() { }

		//! do update particle
		/*!
		 * @param[in] dt deltaT
		 * @param[in] p update particle data
		 */
		virtual void update(double dt, ParticleData *p) = 0;
	};

	// update position by euler eq.
	class VLEXT_EXPORT EulerUpdater : public ParticleUpdater
	{
		VL_INSTRUMENT_CLASS(vlExt::EulerUpdater, vlExt::ParticleUpdater)
	public:
		//! get global acceleration
		vl::vec4 const& globalAcceleration() const { return m_globalAcceleration; }
		//! set global acceleration
		void setGlobalAcceleration(vl::vec4 const& acc) {
			m_globalAcceleration = acc;
		}
	public:
		EulerUpdater()
			: m_globalAcceleration(0, 0, 0, 0) {}
		virtual void update(double dt, ParticleData *p) override;
	protected:
		vl::vec4 m_globalAcceleration;
	};

	// collision with the floor :) todo: implement a collision model
	class VLEXT_EXPORT FloorUpdater : public ParticleUpdater
	{
		VL_INSTRUMENT_CLASS(vlExt::FloorUpdater, vlExt::ParticleUpdater)
	public:
		//! 
		void setFloor(vl::vec4 const& planeEq)
		{
			m_floor = planeEq;
		}
		//! 
		void setBounceFactor(float factor)
		{
			m_bounceFactor = factor;
		}
	public:
		FloorUpdater()
			: m_bounceFactor(0.5f)
		{
			m_floor = vl::vec4(0, 1, 0, 0);
		}
		virtual void update(double dt, ParticleData *p) override;
	protected:
		vl::vec4 m_floor;	//!< floor eq
		float m_bounceFactor;	//!< bounce factor
	};

	//! update position by attractor point
	class VLEXT_EXPORT AttractorUpdater : public ParticleUpdater
	{
		VL_INSTRUMENT_CLASS(vlExt::AttractorUpdater, vlExt::ParticleUpdater)
	protected:
		std::vector<vl::vec4> m_attractors; // .w is force
	public:
		virtual void update(double dt, ParticleData *p) override;

	public:
		std::vector<vl::vec4> const& attractors() const { return m_attractors; }
		std::vector<vl::vec4>& attractors() { return m_attractors; }
		void addAttractor(const vl::vec4 &attr) { m_attractors.push_back(attr); }
		vl::vec4 const& getAddtractor(size_t id) const { return m_attractors[id]; }
	};

	//! update color by life time
	class VLEXT_EXPORT BasicColorUpdater : public ParticleUpdater
	{
		VL_INSTRUMENT_CLASS(vlExt::BasicColorUpdater, vlExt::ParticleUpdater)
	public:
		virtual void update(double dt, ParticleData *p) override;
	};

	//! update color by position
	class VLEXT_EXPORT PosColorUpdater : public ParticleUpdater
	{
		VL_INSTRUMENT_CLASS(vlExt::PosColorUpdater, vlExt::ParticleUpdater)
	public:
		void setPositionRange(vl::vec3 const& minPos, vl::vec3 const& maxPos)
		{
			m_minPos = minPos;
			m_maxPos = maxPos;
		}
	public:
		PosColorUpdater()
			: m_minPos(0, 0, 0), m_maxPos(1, 1, 1) {}
		virtual void update(double dt, ParticleData *p) override;
	protected:
		vl::vec3 m_minPos;
		vl::vec3 m_maxPos;
	};

	//! update color by current velocity
	class VLEXT_EXPORT VelColorUpdater : public ParticleUpdater
	{
		VL_INSTRUMENT_CLASS(vlExt::VelColorUpdater, vlExt::ParticleUpdater)
	public:
		void setVelocity(vl::vec3 const& minVel, vl::vec3 const& maxVel)
		{
			m_minVel = minVel;
			m_maxVel = maxVel;
		}

	public:
		VelColorUpdater()
			: m_minVel(0, 0, 0), m_maxVel(1, 1, 1) {}
		virtual void update(double dt, ParticleData *p) override;
	protected:
		vl::vec3 m_minVel;
		vl::vec3 m_maxVel;
	};

	//! update life time
	class VLEXT_EXPORT BasicTimeUpdater : public ParticleUpdater
	{
		VL_INSTRUMENT_CLASS(vlExt::BasicTimeUpdater, vlExt::ParticleUpdater)
	public:
		virtual void update(double dt, ParticleData *p) override;
	};
}
