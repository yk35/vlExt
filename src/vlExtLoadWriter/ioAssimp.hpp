#pragma once

#include <vlGraphics/Actor.hpp>
#include <vlCore/ResourceLoadWriter.hpp>
#include <vlCore/ResourceDatabase.hpp>
#include <vlCore/String.hpp>
#include <vector>
#include "vlExtLoadWriter/link_config.hpp"

namespace vl
{
	class VirtualFile;
}

namespace vlExt
{
	//-----------------------------------------------------------------------------
	VLEXT_EXPORT vl::ref<vl::ResourceDatabase> loadAssimp(vl::VirtualFile* file);
	VLEXT_EXPORT vl::ref<vl::ResourceDatabase> loadAssimp(const vl::String& path);
}


