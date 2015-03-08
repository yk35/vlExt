#pragma once

#include <vlExtParticle/link_config.hpp>
#include <memory>
#include <vlCore/glsl_math.hpp>

namespace vlExt
{
	/*!
	 * particle datas 
	 */
	class VLEXT_EXPORT ParticleData
	{
	public:
		//! current position
		std::unique_ptr<vl::vec4[]> m_pos;
		//! current color
		std::unique_ptr<vl::vec4[]> m_col;
		//! color when emitted
		std::unique_ptr<vl::vec4[]> m_startCol;
		//! color when vanished
		std::unique_ptr<vl::vec4[]> m_endCol;
		//! current velocity
		std::unique_ptr<vl::vec4[]> m_vel;
		//! current accel
		std::unique_ptr<vl::vec4[]> m_acc;
		//! time to live
		std::unique_ptr<vl::vec4[]> m_time;
		//! alived flag
		std::unique_ptr<bool[]>  m_alive;

	public:
		explicit ParticleData(size_t maxCount)
			: m_count(0), m_countAlive(0) { generate(maxCount); }
		~ParticleData() { }

		ParticleData(const ParticleData &) = delete;
		ParticleData &operator=(const ParticleData &) = delete;

		void generate(size_t maxSize);
		void kill(size_t id);
		void wake(size_t id);
		void swapData(size_t a, size_t b);
		void setCount(size_t count) { m_count = count; }
		void setCountAlive(size_t countAlive) { m_countAlive = countAlive; }
		size_t count() const { return m_count; }
		size_t countAlive() const {	return m_countAlive; }
	private:
		size_t m_count;
		size_t m_countAlive;

	};
}