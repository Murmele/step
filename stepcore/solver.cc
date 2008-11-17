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

#include "solver.h"
#include <QtGlobal>

namespace StepCore {

// XXX: units for toleranceAbs, localError
STEPCORE_META_OBJECT(Solver, QT_TRANSLATE_NOOP("ObjectClass", "Solver"), QT_TR_NOOP("Solver"), MetaObject::ABSTRACT, STEPCORE_SUPER_CLASS(Object),
    STEPCORE_PROPERTY_R(QString, solverType, QT_TR_NOOP("solverType"), STEPCORE_UNITS_NULL, QT_TR_NOOP("Solver type"), solverType)
    STEPCORE_PROPERTY_RW_D(double, stepSize, QT_TR_NOOP("stepSize"), QT_TR_NOOP("s"), QT_TR_NOOP("Step size"), stepSize, setStepSize)
    STEPCORE_PROPERTY_RW(double, toleranceAbs, QT_TR_NOOP("toleranceAbs"), STEPCORE_UNITS_1, QT_TR_NOOP("Allowed absolute tolerance"), toleranceAbs, setToleranceAbs)
    STEPCORE_PROPERTY_RW(double, toleranceRel, QT_TR_NOOP("toleranceRel"), STEPCORE_UNITS_1, QT_TR_NOOP("Allowed relative tolerance"), toleranceRel, setToleranceRel)
    STEPCORE_PROPERTY_R(int, dimension, QT_TR_NOOP("dimension"), STEPCORE_UNITS_1, QT_TR_NOOP("Count of dynamic variables"), dimension)
    STEPCORE_PROPERTY_R_D(double, localError, QT_TR_NOOP("localError"), STEPCORE_UNITS_1, QT_TR_NOOP("Maximal local error during last step"), localError)
    STEPCORE_PROPERTY_R_D(double, localErrorRatio, QT_TR_NOOP("localErrorRatio"), STEPCORE_UNITS_1, QT_TR_NOOP("Maximal local error ratio during last step"), localErrorRatio))

} // namespace StepCore
