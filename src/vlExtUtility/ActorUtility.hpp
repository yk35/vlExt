#pragma once

#include <vlGraphics/Actor.hpp>
#include <functional>

namespace vlExt
{
	class ActorEventCallbackFunc : public vl::ActorEventCallback
	{
	public:
		ActorEventCallbackFunc(
			std::function<void(vl::Actor* actor, vl::real frame_clock, const vl::Camera* cam, vl::Renderable* renderable, const vl::Shader* shader, int pass)> started)
			: m_renderStarted(started)
		{

		}
		ActorEventCallbackFunc(
			std::function<void(vl::Actor* actor, vl::real frame_clock, const vl::Camera* cam, vl::Renderable* renderable, const vl::Shader* shader, int pass)> started,
			std::function<void(vl::Actor* actor)> actorDelete)
			: m_renderStarted(started), m_actorDelete(actorDelete)
		{

		}
		virtual void onActorRenderStarted(vl::Actor* actor, vl::real frame_clock, const vl::Camera* cam, vl::Renderable* renderable, const vl::Shader* shader, int pass) override
		{
			if (m_renderStarted)
			{
				m_renderStarted(actor, frame_clock, cam, renderable, shader, pass);
			}
		}

		/** Event notifying that an Actor is being deleted. */
		virtual void onActorDelete(vl::Actor* actor) override
		{
			if (m_actorDelete)
			{
				m_actorDelete(actor);
			}
		}
	private:
		std::function<void(vl::Actor* actor, vl::real frame_clock, const vl::Camera* cam, vl::Renderable* renderable, const vl::Shader* shader, int pass)> m_renderStarted;
		std::function<void(vl::Actor* actor)> m_actorDelete;
	};
}