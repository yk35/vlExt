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

		virtual void update(double dt, ParticleData *p) = 0;
	};

	class VLEXT_EXPORT EulerUpdater : public ParticleUpdater
	{
		VL_INSTRUMENT_CLASS(vlExt::EulerUpdater, vlExt::ParticleUpdater)
	public:
		vl::vec4 m_globalAcceleration;
	public:
		EulerUpdater()
			: m_globalAcceleration(0, 0, 0, 0) {}
		virtual void update(double dt, ParticleData *p) override;
	};

	// collision with the floor :) todo: implement a collision model
	class VLEXT_EXPORT FloorUpdater : public ParticleUpdater
	{
		VL_INSTRUMENT_CLASS(vlExt::FloorUpdater, vlExt::ParticleUpdater)
	public:
		float m_floorY;
		float m_bounceFactor;
	public:
		FloorUpdater()
			: m_floorY(0), m_bounceFactor(0.5f) {}
		virtual void update(double dt, ParticleData *p) override;
	};

	class VLEXT_EXPORT AttractorUpdater : public ParticleUpdater
	{
		VL_INSTRUMENT_CLASS(vlExt::AttractorUpdater, vlExt::ParticleUpdater)
	protected:
		std::vector<vl::vec4> m_attractors; // .w is force
	public:
		virtual void update(double dt, ParticleData *p) override;

		size_t collectionSize() const { return m_attractors.size(); }
		void add(const vl::vec4 &attr) { m_attractors.push_back(attr); }
		vl::vec4 &get(size_t id) { return m_attractors[id]; }
	};

	class VLEXT_EXPORT BasicColorUpdater : public ParticleUpdater
	{
		VL_INSTRUMENT_CLASS(vlExt::BasicColorUpdater, vlExt::ParticleUpdater)
	public:
		virtual void update(double dt, ParticleData *p) override;
	};

	class VLEXT_EXPORT PosColorUpdater : public ParticleUpdater
	{
		VL_INSTRUMENT_CLASS(vlExt::PosColorUpdater, vlExt::ParticleUpdater)
	public:
		vl::vec4 m_minPos;
		vl::vec4 m_maxPos;
	public:
		PosColorUpdater()
			: m_minPos(0, 0, 0, 0), m_maxPos(1, 1, 1, 1) {}
		virtual void update(double dt, ParticleData *p) override;
	};

	class VLEXT_EXPORT VelColorUpdater : public ParticleUpdater
	{
		VL_INSTRUMENT_CLASS(vlExt::VelColorUpdater, vlExt::ParticleUpdater)
	public:
		vl::vec4 m_minVel;
		vl::vec4 m_maxVel;
	public:
		VelColorUpdater()
			: m_minVel(0, 0, 0, 0), m_maxVel(1, 1, 1, 1) {}
		virtual void update(double dt, ParticleData *p) override;
	};

	class VLEXT_EXPORT BasicTimeUpdater : public ParticleUpdater
	{
		VL_INSTRUMENT_CLASS(vlExt::BasicTimeUpdater, vlExt::ParticleUpdater)
	public:
		virtual void update(double dt, ParticleData *p) override;
	};
}
