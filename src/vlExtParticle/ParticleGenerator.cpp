#include "vlExtParticle/ParticleGenerator.hpp"
#include "vlExtParticle/ParticleData.hpp"
#include <vlCore/glsl_math.hpp>
#include <vlCore/math_utils.hpp>


namespace vlExt
{
	namespace {
		vl::vec4 rand4(vl::vec4 const& minVec, vl::vec4 const& maxVec)
		{
			vl::vec4 ret;
			ret.x() = vl::random(minVec.x(), maxVec.x());
			ret.y() = vl::random(minVec.y(), maxVec.y());
			ret.z() = vl::random(minVec.z(), maxVec.z());
			ret.w() = vl::random(minVec.w(), maxVec.w());
			return ret;
		}
	}
	void BoxPosGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
	{
		vl::vec4 posMin{ m_pos.x() - m_maxStartPosOffset.x(), m_pos.y() - m_maxStartPosOffset.y(), m_pos.z() - m_maxStartPosOffset.z(), 1.0 };
		vl::vec4 posMax{ m_pos.x() + m_maxStartPosOffset.x(), m_pos.y() + m_maxStartPosOffset.y(), m_pos.z() + m_maxStartPosOffset.z(), 1.0 };

		for (size_t i = startId; i < endId; ++i)
		{
			p->m_pos[i] = rand4(posMin, posMax);
		}
	}

	void RoundPosGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
	{
		for (size_t i = startId; i < endId; ++i)
		{
			double ang = vl::random(0.0, vl::dPi*2.0);
			p->m_pos[i] = m_center + vl::vec4(m_radX*sin(ang), m_radY*cos(ang), 0.0, 1.0);
		}
	}

	void BasicColorGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
	{
		for (size_t i = startId; i < endId; ++i)
		{
			p->m_startCol[i] = rand4(m_minStartCol, m_maxStartCol);
			p->m_endCol[i] = rand4(m_minEndCol, m_maxEndCol);
		}
	}

	void BasicVelGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
	{
		for (size_t i = startId; i < endId; ++i)
		{
			p->m_vel[i] = rand4(m_minStartVel, m_maxStartVel);
		}
	}

	void SphereVelGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
	{
		float phi, theta, v, r;
		for (size_t i = startId; i < endId; ++i)
		{
			phi = vl::random(-vl::dPi, vl::dPi);
			theta = vl::random(-vl::dPi, vl::dPi);
			v = vl::random(m_minVel, m_maxVel);

			r = v*sinf(phi);
			p->m_vel[i].z() = v*cosf(phi);
			p->m_vel[i].x() = r*cosf(theta);
			p->m_vel[i].y() = r*sinf(theta);
		}
	}

	void ConeVelGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
	{
		float phi, theta, v, r;
		for (size_t i = startId; i < endId; ++i)
		{
			phi = vl::random(-m_phi, m_phi) + m_basePhi;
			theta = vl::random(-m_theta, m_theta) + m_baseTheta;
			v = vl::random(m_minVel, m_maxVel);

			r = v*sinf(phi);
			p->m_vel[i].z() = v*cosf(phi);
			p->m_vel[i].x() = r*cosf(theta);
			p->m_vel[i].y() = r*sinf(theta);
		}
	}


	void BasicTimeGen::generate(double dt, ParticleData *p, size_t startId, size_t endId)
	{
		for (size_t i = startId; i < endId; ++i)
		{
			auto r = vl::random(m_minTime, m_maxTime);
			p->m_time[i].x() = r;
			p->m_time[i].y() = r;
			p->m_time[i].z() = (float)0.0;
			p->m_time[i].w() = (float)1.0 / p->m_time[i].x();
		}
	}
}