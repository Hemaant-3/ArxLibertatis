/*
 * Copyright 2013-2019 Arx Libertatis Team (see the AUTHORS file)
 *
 * This file is part of Arx Libertatis.
 *
 * Arx Libertatis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Arx Libertatis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Arx Libertatis.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ARX_GRAPHICS_OPENGL_GLDEBUG_H
#define ARX_GRAPHICS_OPENGL_GLDEBUG_H

namespace gldebug {

enum Mode {
	Enabled, //!< Check and log errors
	Ignored, //!< Check errors but don't log (OpenGL default)
	NoError, //!< Treat errors as undefined behavior (falls back to Ignored if not supported)
	Default  //!< Select mode according to build type and enabled features
};

//! Initialize OpenGL debug output.
void initialize();

//! Check if debug output should be enabled.
Mode mode();

//! Must be called at the end of each frame
void endFrame();

} // namespace gldebug

#endif // ARX_GRAPHICS_OPENGL_GLDEBUG_H
