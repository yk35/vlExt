#pragma once

#include <vlExtParticle/link_config.hpp>
#include <vlCore/Object.hpp>
#include <vlCore/TypeInfo.hpp>
#include <vlExtParticle/ParticleData.hpp>

namespace vlExt
{
	/*!
	 * generator for particle objects
	 */
	class VLEXT_EXPORT ParticleGenerator : public vl::Object
	{
		VL_INSTRUMENT_ABSTRACT_CLASS(vlExt::ParticleGenerator, vl::Object)
	public:
		/*!
		 * generate particle datas 
		 */
		virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) =0;

	};
	/*!
	* box position generator
	*/
	class VLEXT_EXPORT BoxPosGen : public ParticleGenerator
	{
		VL_INSTRUMENT_CLASS(vlExt::BoxPosGen, vlExt::ParticleGenerator)
	protected:
		vl::vec3 m_pos;
		vl::vec3 m_maxStartPosOffset;
	public:
		vl::vec3 pos() const { return m_pos; }
		void setPos(vl::vec3 const& pos) { m_pos = pos; }
		vl::vec3 maxStartPosOffset() const { return m_maxStartPosOffset; }
		void setMaxStartPosOffset(vl::vec3 const& offset) { m_maxStartPosOffset = offset; }
	public:
		BoxPosGen() { }

		virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
	};

	/*!
	 * round position generator
	 */
	class VLEXT_EXPORT RoundPosGen : public ParticleGenerator
	{
		VL_INSTRUMENT_CLASS(vlExt::RoundPosGen, vlExt::ParticleGenerator)
	protected:
		vl::vec4 m_center;
		float m_radX{ 0.0 };
		float m_radY{ 0.0 };

	public:
		vl::vec4 center() const { return m_center; }
		void setCenter(vl::vec4 const& c) { m_center = c; }
		float radX() const { return m_radX; }
		void setRadX(float x) { m_radX = x; }
		float radY() const { return m_radY; }
		void setRadY(float y) { m_radY = y; }

	public:
		RoundPosGen() { }
		RoundPosGen(const vl::vec4 &center, double radX, double radY)
			: m_center(center)
			, m_radX((float)radX)
			, m_radY((float)radY)
		{ }

		virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
	};

	//! set initial color when emit perticle
	class VLEXT_EXPORT BasicColorGen : public ParticleGenerator
	{
		VL_INSTRUMENT_CLASS(vlExt::BasicColorGen, vlExt::ParticleGenerator)
	public:
		void setStartCol(vl::vec4 const& startMin, vl::vec4 const startMax)
		{
			m_minStartCol = startMin;
			m_maxStartCol = startMax;
		}
		void setEndCol(vl::vec4 const& endMin, vl::vec4 const endMax)
		{
			m_minEndCol = endMin;
			m_maxEndCol = endMax;
		}
	protected:
		vl::vec4 m_minStartCol;
		vl::vec4 m_maxStartCol;
		vl::vec4 m_minEndCol;
		vl::vec4 m_maxEndCol;
	public:
		BasicColorGen()
		{
			m_minStartCol = vl::vec4(1, 1, 1, 1);
			m_maxStartCol = vl::vec4(1, 1, 1, 1);
			m_minEndCol = vl::vec4(1, 1, 1, 1);
			m_maxEndCol = vl::vec4(1, 1, 1, 1);
		}

		virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
	};

	//! set initial velocity when emit perticle
	class VLEXT_EXPORT BasicVelGen : public ParticleGenerator
	{
		VL_INSTRUMENT_CLASS(vlExt::BasicVelGen, vlExt::ParticleGenerator)
	public:
		vl::vec4 m_minStartVel;
		vl::vec4 m_maxStartVel;
	public:
		BasicVelGen() { }

		virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
	};

	//! set initial velocity to all direction when emit perticle
	class VLEXT_EXPORT SphereVelGen : public ParticleGenerator
	{
		VL_INSTRUMENT_CLASS(vlExt::SphereVelGen, vlExt::ParticleGenerator)
	protected:
		float m_minVel{ 0.0f };
		float m_maxVel{ 0.0f };
	public:
		void setVelocity(float minVel, float maxVel)
		{
			m_minVel = minVel;
			m_maxVel = maxVel;
		}
	public:
		SphereVelGen() { }

		virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
	};

	//! set initial velocity to spec direction when emit perticle
	class VLEXT_EXPORT ConeVelGen : public SphereVelGen
	{
		VL_INSTRUMENT_CLASS(vlExt::ConeVelGen, vlExt::SphereVelGen)
	protected:
		float m_phi;
		float m_theta;
		float m_basePhi;
		float m_baseTheta;
	public:
		void setDirectionBaseAngle(float phi, float theta)
		{
			m_basePhi = phi;
			m_baseTheta = theta;
		}
		void setDirectionRange(float phi, float theta)
		{
			m_phi = phi;
			m_theta = theta;
		}
	public:
		ConeVelGen()
			: m_basePhi(0),
			m_baseTheta(0),
			m_phi(0),
			m_theta(0)
		{

		}

		virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
	};


	//! set initial life time when emit particle
	class VLEXT_EXPORT BasicTimeGen : public ParticleGenerator
	{
		VL_INSTRUMENT_CLASS(vlExt::BasicTimeGen, vlExt::ParticleGenerator)
	protected:
		float m_minTime{ 0.0 };
		float m_maxTime{ 1.0 };
	public:
		//! particle life time
		void setLifeTime(float minTime, float maxTime)
		{
			m_minTime = minTime;
			m_maxTime = maxTime;
		}
	public:
		BasicTimeGen() { }

		virtual void generate(double dt, ParticleData *p, size_t startId, size_t endId) override;
	};
}