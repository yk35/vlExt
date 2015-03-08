#include "vlExtParticle/ParticleUpdater.hpp"
#include "vlExtParticle/ParticleData.hpp"
#include <vlCore/Vector4.hpp>
#include <vlCore/glsl_math.hpp>
#include <algorithm>

namespace vlExt
{
	void EulerUpdater::update(double dt, ParticleData *p)
	{
		const vl::vec4 globalA( dt * m_globalAcceleration.x(), dt * m_globalAcceleration.y(), dt * m_globalAcceleration.z(), 0.0 );
		const float localDT = (float)dt;

		const unsigned int endId = p->countAlive();
		for (size_t i = 0; i < endId; ++i)
			p->m_acc[i] += globalA;

		for (size_t i = 0; i < endId; ++i)
			p->m_vel[i] += localDT * p->m_acc[i];

		for (size_t i = 0; i < endId; ++i)
			p->m_pos[i] += localDT * p->m_vel[i];
	}

	void FloorUpdater::update(double dt, ParticleData *p)
	{
		const float localDT = (float)dt;

		const size_t endId = p->countAlive();
		for (size_t i = 0; i < endId; ++i)
		{
			if (p->m_pos[i].y() < m_floorY)
			{
				vl::vec4 force = p->m_acc[i];
				float normalFactor = vl::dot(force, vl::vec4(0.0f, 1.0f, 0.0f, 0.0f));
				if (normalFactor < 0.0f)
					force -= vl::vec4(0.0f, 1.0f, 0.0f, 0.0f) * normalFactor;

				float velFactor = vl::dot(p->m_vel[i], vl::vec4(0.0f, 1.0f, 0.0f, 0.0f));
				//if (velFactor < 0.0)
				p->m_vel[i] -= vl::vec4(0.0f, 1.0f, 0.0f, 0.0f) * (1.0f + m_bounceFactor) * velFactor;

				p->m_acc[i] = force;
			}
		}

	}

	void AttractorUpdater::update(double dt, ParticleData *p)
	{
		const float localDT = (float)dt;

		const size_t endId = p->countAlive();
		const size_t countAttractors = m_attractors.size();
		vl::vec4 off;
		float dist;
		size_t a;
		for (size_t i = 0; i < endId; ++i)
		{
			for (a = 0; a < countAttractors; ++a)
			{
				off.x() = m_attractors[a].x() - p->m_pos[i].x();
				off.y() = m_attractors[a].y() - p->m_pos[i].y();
				off.z() = m_attractors[a].z() - p->m_pos[i].z();
				dist = vl::dot(off, off);

				//if (fabs(dist) > 0.00001)
				dist = m_attractors[a].w() / dist;

				p->m_acc[i] += off * dist;
			}
		}
	}

	void BasicColorUpdater::update(double dt, ParticleData *p)
	{
		const size_t endId = p->countAlive();
		for (size_t i = 0; i < endId; ++i)
			p->m_col[i] = vl::mix(p->m_startCol[i], p->m_endCol[i], p->m_time[i].z());
	}

	void PosColorUpdater::update(double dt, ParticleData *p)
	{
		const size_t endId = p->countAlive();
		float scaler, scaleg, scaleb;
		float diffr = m_maxPos.x() - m_minPos.x();
		float diffg = m_maxPos.y() - m_minPos.y();
		float diffb = m_maxPos.z() - m_minPos.z();
		for (size_t i = 0; i < endId; ++i)
		{
			scaler = (p->m_pos[i].x() - m_minPos.x()) / diffr;
			scaleg = (p->m_pos[i].y() - m_minPos.y()) / diffg;
			scaleb = (p->m_pos[i].z() - m_minPos.z()) / diffb;
			p->m_col[i].r() = scaler;// glm::mix(p->m_startCol[i].r, p->m_endCol[i].r, scaler);
			p->m_col[i].g() = scaleg;// glm::mix(p->m_startCol[i].g, p->m_endCol[i].g, scaleg);
			p->m_col[i].b() = scaleb;// glm::mix(p->m_startCol[i].b, p->m_endCol[i].b, scaleb);
			p->m_col[i].a() = vl::mix(p->m_startCol[i].a(), p->m_endCol[i].a(), p->m_time[i].z());
		}
	}

	void VelColorUpdater::update(double dt, ParticleData *p)
	{
		const size_t endId = p->countAlive();
		float scaler, scaleg, scaleb;
		float diffr = m_maxVel.x() - m_minVel.x();
		float diffg = m_maxVel.y() - m_minVel.y();
		float diffb = m_maxVel.z() - m_minVel.z();
		for (size_t i = 0; i < endId; ++i)
		{
			scaler = (p->m_vel[i].x() - m_minVel.x()) / diffr;
			scaleg = (p->m_vel[i].y() - m_minVel.y()) / diffg;
			scaleb = (p->m_vel[i].z() - m_minVel.z()) / diffb;
			p->m_col[i].r() = scaler;// glm::mix(p->m_startCol[i].r, p->m_endCol[i].r, scaler);
			p->m_col[i].g() = scaleg;// glm::mix(p->m_startCol[i].g, p->m_endCol[i].g, scaleg);
			p->m_col[i].b() = scaleb;// glm::mix(p->m_startCol[i].b, p->m_endCol[i].b, scaleb);
			p->m_col[i].a() = vl::mix(p->m_startCol[i].a(), p->m_endCol[i].a(), p->m_time[i].z());
		}
	}

	void BasicTimeUpdater::update(double dt, ParticleData *p)
	{
		unsigned int endId = p->countAlive();
		const float localDT = (float)dt;

		if (endId == 0) return;

		for (size_t i = 0; i < endId; ++i)
		{
			p->m_time[i].x() -= localDT;
			// interpolation: from 0 (start of life) till 1 (end of life)
			p->m_time[i].z() = (float)1.0 - (p->m_time[i].x()*p->m_time[i].w()); // .w is 1.0/max life time		

			if (p->m_time[i].x() < (float)0.0)
			{
				p->kill(i);
				endId = (std::min)(p->countAlive(), p->count());
			}
		}
	}
}