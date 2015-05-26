/*
 * Copyright 2012-2015 Moritz Hilscher
 *
 * This file is part of Mapcrafter.
 *
 * Mapcrafter is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mapcrafter is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mapcrafter.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "overlay.h"

#include "../blockimages.h"
#include "../image.h"
#include "../../mc/pos.h"

namespace mapcrafter {
namespace renderer {

OverlayRenderer::OverlayRenderer()
	: high_contrast(true) {
}

OverlayRenderer::~OverlayRenderer() {
}

void OverlayRenderer::setHighContrast(bool high_contrast) {
	this->high_contrast = high_contrast;
}

void OverlayRenderer::tintBlock(RGBAImage& image, uint8_t r, uint8_t g, uint8_t b) {
	if (high_contrast) {
		// if high contrast mode is enabled, then do some magic here

		// get luminance of recolor
		int luminance = (10*r + 3*g + b) / 14;

		// try to do luminance-neutral additive/subtractive color
		// instead of alpha blending (for better contrast)
		// so first subtract luminance from each component
		int nr = (r - luminance) / 3; // /3 is similar to alpha=85
		int ng = (g - luminance) / 3;
		int nb = (b - luminance) / 3;

		int size = image.getWidth();
		for (int y = 0; y < size; y++) {
			for (int x = 0; x < size; x++) {
				uint32_t pixel = image.getPixel(x, y);
				if (pixel != 0)
					image.setPixel(x, y, rgba_add_clamp(pixel, nr, ng, nb));
			}
		}
	} else {
		// otherwise just simple alphablending
		uint32_t color = rgba(r, g, b, 128);

		int size = image.getWidth();
		for (int y = 0; y < size; y++) {
			for (int x = 0; x < size; x++) {
				uint32_t pixel = image.getPixel(x, y);
				if (pixel != 0)
					image.setPixel(x, y, color);
			}
		}
	}
}

const RenderModeRendererType OverlayRenderer::TYPE = RenderModeRendererType::OVERLAY;

OverlayRenderMode::~OverlayRenderMode() {
}

void OverlayRenderMode::draw(RGBAImage& image, const mc::BlockPos& pos, uint16_t id,
		uint16_t data) {
	RGBAPixel color = getBlockColor(pos, id, data);
	if (rgba_alpha(color) == 0)
		return;

	uint8_t r = rgba_red(color);
	uint8_t g = rgba_green(color);
	uint8_t b = rgba_blue(color);
	// tint top face of solid blocks and everything of transparent blocks for now
	if (images->isBlockTransparent(id, data)) {
		renderer->tintBlock(image, r, g, b);
	} else {
		renderer->tintTop(image, r, g, b, 0);
	}
}

}
}
