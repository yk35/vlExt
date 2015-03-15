#include "vlExtParticle/ParticleData.hpp"
#include <vlCore/glsl_math.hpp>
#include <algorithm>

namespace vlExt
{
	void ParticleData::generate(size_t maxSize)
	{
		m_count = maxSize;
		m_countAlive = 0;

		m_pos.reset(new vl::vec4[maxSize]);
		std::fill(m_pos.get(), m_pos.get() + maxSize, vl::vec4(0, 0, 0, 1));
		m_col.reset(new vl::vec4[maxSize]);
		std::fill(m_col.get(), m_col.get() + maxSize, vl::vec4(1, 1, 1, 1));
		m_startCol.reset(new vl::vec4[maxSize]);
		std::fill(m_startCol.get(), m_startCol.get() + maxSize, vl::vec4(1, 1, 1, 1));
		m_endCol.reset(new vl::vec4[maxSize]);
		std::fill(m_endCol.get(), m_endCol.get() + maxSize, vl::vec4(1, 1, 1, 1));
		m_vel.reset(new vl::vec4[maxSize]);
		std::fill(m_vel.get(), m_vel.get() + maxSize, vl::vec4(0, 0, 0, 0));
		m_acc.reset(new vl::vec4[maxSize]);
		std::fill(m_acc.get(), m_acc.get() + maxSize, vl::vec4(0, 0, 0, 0));
		m_time.reset(new vl::vec4[maxSize]);
		std::fill(m_time.get(), m_time.get() + maxSize, vl::vec4(1, 1, 1, 1));
		m_alive.reset(new bool[maxSize]);
		std::fill(m_alive.get(), m_alive.get() + maxSize, false);
	}

	void ParticleData::kill(size_t id)
	{
		if (m_countAlive > 0) // maybe this if can be removed?
		{
			m_alive[id] = false;
			swapData(id, m_countAlive - 1);
			m_countAlive--;
		}
	}

	void ParticleData::wake(size_t id)
	{
		if (m_countAlive < m_count) // maybe this if can be removed?
		{
			m_alive[id] = true;
			swapData(id, m_countAlive);
			m_countAlive++;
		}
	}

	void ParticleData::swapData(size_t a, size_t b)
	{
		std::swap(m_pos[a], m_pos[b]);
		std::swap(m_col[a], m_col[b]);
		std::swap(m_startCol[a], m_startCol[b]);
		std::swap(m_endCol[a], m_endCol[b]);
		std::swap(m_vel[a], m_vel[b]);
		std::swap(m_acc[a], m_acc[b]);
		std::swap(m_time[a], m_time[b]);
		std::swap(m_alive[a], m_alive[b]);
	}
}