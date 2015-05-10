#pragma once

#include <vlGraphics/Actor.hpp>
#include <vlGraphics/Rendering.hpp>
#include <functional>

namespace vlExt
{
	class RenderEventCallbackFunc : public vl::RenderEventCallback
	{
	public:
		/** Reimplement to react to this event.
		* \return \p true if the callback reacted to the given event. */
		virtual bool onRenderingStarted(const vl::RenderingAbstract* rendering)
		{
			if (mRenderingStarted)
			{
				mRenderingStarted(rendering);
				return true;
			}
			else
			{
				return false;
			}
		}

		/** Reimplement to react to this event.
		* \return \p true if the callback reacted to the given event. */
		virtual bool onRenderingFinished(const vl::RenderingAbstract* rendering)
		{

			if (mRenderingFinished)
			{
				mRenderingFinished(rendering);
				return true;
			}
			else
			{
				return false;
			}
		}

		/** Reimplement to react to this event.
		* \return \p true if the callback reacted to the given event. */
		virtual bool onRendererStarted(const vl::RendererAbstract* renderer)
		{

			if (mRendererStarted)
			{
				mRendererStarted(renderer);
				return true;
			}
			else
			{
				return false;
			}
		}

		/** Reimplement to react to this event.
		* \return \p true if the callback reacted to the given event. */
		virtual bool onRendererFinished(const vl::RendererAbstract* renderer)
		{
			if (mRendererFinished)
			{
				mRendererFinished(renderer);
				return true;
			}
			else
			{
				return false;
			}

		}

		void setRenderingStarted(std::function<void(const vl::RenderingAbstract*)> f)
		{
			mRenderingStarted = f;
		}
		void setRenderingFinished(std::function<void(const vl::RenderingAbstract*)> f)
		{
			mRenderingFinished = f;
		}
		void setRendererStarted(std::function<void(const vl::RendererAbstract*)> f)
		{
			mRendererStarted = f;
		}
		void setRendererFinished(std::function<void(const vl::RendererAbstract*)> f)
		{
			mRendererFinished = f;
		}
	private:
		std::function<void(const vl::RenderingAbstract*)> mRenderingStarted;
		std::function<void(const vl::RenderingAbstract*)> mRenderingFinished;
		std::function<void(const vl::RendererAbstract*)> mRendererStarted;
		std::function<void(const vl::RendererAbstract*)> mRendererFinished;
	};
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