/*
 * Copyright 2015 Arx Libertatis Team (see the AUTHORS file)
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

#ifndef ARX_GUI_WIDGET_TEXTWIDGET_H
#define ARX_GUI_WIDGET_TEXTWIDGET_H

#include "graphics/Color.h"
#include "gui/widget/Widget.h"

class Font;

class TextWidget: public Widget {
	
public:
	std::string lpszText;
	Font*	pFont;
	Color lColor;
	Color lOldColor;
	Color lColorHighlight;
	bool	bSelected;
	
public:
	TextWidget(MenuButton id, Font * font, const std::string & text, Vec2i pos = Vec2i_ZERO);
	virtual ~TextWidget();
	
	void setColor(Color color) { lColor = color; }
	
	Widget * OnShortCut();
	bool OnMouseClick();
	void Update(int time);
	void Render();
	void SetText(const std::string & _pText);
	void RenderMouseOver();
	
	bool OnMouseDoubleClick();
};

#endif // ARX_GUI_WIDGET_TEXTWIDGET_H
