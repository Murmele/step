/* This file is part of StepCore library.
   Copyright (C) 2007 Vladimir Kuznetsov <ks.vladimir@gmail.com>

   StepCore library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   StepCore library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with StepCore; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/** \file tool.h
 *  \brief Note class
 */

#ifndef STEPCORE_TOOL_H
#define STEPCORE_TOOL_H

#include "world.h"
#include "types.h"

namespace StepCore
{

class Note: public Item, public Tool
{
    STEPCORE_OBJECT(Note)

public:
    const QString& text() const { return _text; }
    void setText(const QString& text) { _text = text; }

    const Vector2d& position() const { return _position; }
    void setPosition(const Vector2d& position) { _position = position; }

private:
    QString  _text;
    Vector2d _position;
};

} // namespace StepCore

#endif