
#include "vlExtParticle/ParticleEmitter.hpp"
#include "vlExtParticle/ParticleGenerator.hpp"
#include "vlExtParticle/ParticleData.hpp"
#include <cmath>

namespace vlExt
{
	void ParticleEmitter::emit(double dt, ParticleData *p)
	{
		const size_t maxNewParticles = static_cast<size_t>(dt*m_emitRate);
		const size_t startId = p->countAlive();
		const size_t endId = (std::min)(startId + maxNewParticles, p->count() - 1);

		for (size_t i = 0; i < m_generators.size(); ++i)
		{
			m_generators[i]->generate(dt, p, startId, endId);
		}

		for (size_t i = startId; i < endId; ++i)  // << wake loop
			p->wake(i);
	}

	void ParticleEmitter::addGenerator(ParticleGenerator* generator)
	{
		m_generators.push_back(generator);
	}

}