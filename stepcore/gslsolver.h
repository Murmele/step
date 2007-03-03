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

/** \file gslsolver.h
 *  \brief GslSolver class
 */

#ifndef STEPCORE_GSLSOLVER_H
#define STEPCORE_GSLSOLVER_H

// HACK: CMake does not passes definitions to moc
#if defined(STEPCORE_WITH_GSL) || defined(Q_MOC_RUN)

#include "solver.h"
#include "object.h"

#include <gsl/gsl_odeiv.h>

namespace StepCore
{

/** \ingroup solvers
 *  \brief Adaprive and non-adaptive solvers from GSL library
 *  
 *  See http://www.gnu.org/software/gsl/manual/html_node/Ordinary-Differential-Equations.html#Ordinary-Differential-Equations
 *  and http://en.wikipedia.org/wiki/Numerical_ordinary_differential_equations
 *
 */
class GslSolver: public Solver
{
    STEPCORE_OBJECT(GslSolver)

public:
    /** Constructs GslSolver */
    GslSolver(double stepSize, bool adaptive, const gsl_odeiv_step_type* gslStepType);
    /** Constructs GslSolver */
    GslSolver(int dimension, Function function, void* params, double stepSize,
                bool adaptive, const gsl_odeiv_step_type* gslStepType);
    /** Copy constructor */
    GslSolver(const GslSolver& gslSolver);

    ~GslSolver();

    void setDimension(int dimension) { fini(); _dimension = dimension; init(); }
    void setToleranceAbs(double toleranceAbs) { fini(); _toleranceAbs = toleranceAbs; init(); }
    void setToleranceRel(double toleranceRel) { fini(); _toleranceRel = toleranceRel; init(); }

    void setFunction(Function function) { _gslSystem.function = _function = function; }
    void setParams(void* params) { _gslSystem.params = _params = params; }

    /** Get step size */
    double stepSize() const { return _stepSize; }
    /** Set step size */
    void setStepSize(double stepSize) { _stepSize = stepSize; }

    void doCalcFn(double* t, double y[], double f[] = 0);
    bool doEvolve(double* t, double t1, double y[], double yerr[]);

protected:
    void init();
    void fini();

    double _stepSize;
    bool   _adaptive;

    //gsl_odeiv_control*  _gslControl;
    //gsl_odeiv_evolve*   _gslEvolve;
    double* _ytemp;
    double* _ydiff;

    const gsl_odeiv_step_type* _gslStepType;
    gsl_odeiv_system   _gslSystem;
    gsl_odeiv_step*    _gslStep;
    gsl_odeiv_control* _gslControl;
    gsl_odeiv_evolve*  _gslEvolve;
};

/** \ingroup solvers
 *  \brief Non-adaptive solvers from GSL library
 */
class GslNonAdaptiveSolver: public GslSolver
{
    STEPCORE_OBJECT(GslNonAdaptiveSolver)
public:
    GslNonAdaptiveSolver(double stepSize, const gsl_odeiv_step_type* gslStepType):
                            GslSolver(stepSize, false, gslStepType) {}
    GslNonAdaptiveSolver(int dimension, Function function, void* params, double stepSize,
                            const gsl_odeiv_step_type* gslStepType)
                    : GslSolver(dimension, function, params, stepSize, false, gslStepType) {}
    GslNonAdaptiveSolver(const GslNonAdaptiveSolver& gslSolver): GslSolver(gslSolver) {}
    double stepSize() const { return _stepSize; }
    void setStepSize(double stepSize) { _stepSize = stepSize; }
};

/** \ingroup solvers
 *  \brief Adaptive solvers from GSL library
 */
class GslAdaptiveSolver: public GslSolver
{
    STEPCORE_OBJECT(GslAdaptiveSolver)
public:
    GslAdaptiveSolver(const gsl_odeiv_step_type* gslStepType):
                            GslSolver(1, true, gslStepType) {}
    GslAdaptiveSolver(int dimension, Function function, void* params,
                            const gsl_odeiv_step_type* gslStepType)
                    : GslSolver(dimension, function, params, 1, true, gslStepType) {}
    GslAdaptiveSolver(const GslAdaptiveSolver& gslSolver): GslSolver(gslSolver) {}
    double stepSize() const { return _stepSize; }
};

#define STEPCORE_DECLARE_GSLSOLVER(Class, type) \
class Gsl##Class##Solver: public GslNonAdaptiveSolver { \
    STEPCORE_OBJECT(Gsl##Class##Solver) \
public: \
    Gsl##Class##Solver(double stepSize = 0.01): GslNonAdaptiveSolver(stepSize, gsl_odeiv_step_##type) {} \
    Gsl##Class##Solver(int dimension, Function function, void* params, double stepSize) \
                 : GslNonAdaptiveSolver(dimension, function, params, stepSize, gsl_odeiv_step_##type) {} \
    Gsl##Class##Solver(const Gsl##Class##Solver& gslSolver): GslNonAdaptiveSolver(gslSolver) {} \
};

#define STEPCORE_DECLARE_GSLASOLVER(Class, type) \
class GslAdaptive##Class##Solver: public GslAdaptiveSolver { \
    STEPCORE_OBJECT(GslAdaptive##Class##Solver) \
public: \
    GslAdaptive##Class##Solver(): GslAdaptiveSolver(gsl_odeiv_step_##type) {} \
    GslAdaptive##Class##Solver(int dimension, Function function, void* params) \
                    : GslAdaptiveSolver(dimension, function, params, gsl_odeiv_step_##type) {} \
    GslAdaptive##Class##Solver(const GslAdaptive##Class##Solver& gslSolver): GslAdaptiveSolver(gslSolver) {} \
};

/** \ingroup solvers \brief Runge-Kutta second-order solver from GSL library */
STEPCORE_DECLARE_GSLSOLVER(RK2, rk2);
/** \ingroup solvers \brief Runge-Kutta second-order solver from GSL library */
STEPCORE_DECLARE_GSLASOLVER(RK2, rk2);

/** \ingroup solvers \brief Runge-Kutta classical fourth-order solver from GSL library */
STEPCORE_DECLARE_GSLSOLVER(RK4, rk4);
/** \ingroup solvers \brief Runge-Kutta classical fourth-order solver from GSL library */
STEPCORE_DECLARE_GSLASOLVER(RK4, rk4);

/** \ingroup solvers \brief Runge-Kutta-Fehlberg (4,5) solver from GSL library */
STEPCORE_DECLARE_GSLSOLVER(RKF45, rkf45);
/** \ingroup solvers \brief Runge-Kutta-Fehlberg (4,5) solver from GSL library */
STEPCORE_DECLARE_GSLASOLVER(RKF45, rkf45);

/** \ingroup solvers \brief Runge-Kutta Cash-Karp (4,5) solver from GSL library */
STEPCORE_DECLARE_GSLSOLVER(RKCK, rkck);
/** \ingroup solvers \brief Runge-Kutta Cash-Karp (4,5) solver from GSL library */
STEPCORE_DECLARE_GSLASOLVER(RKCK, rkck);

/** \ingroup solvers \brief Runge-Kutta Prince-Dormand (8,9) solver from GSL library */
STEPCORE_DECLARE_GSLSOLVER(RK8PD, rk8pd);
/** \ingroup solvers \brief Runge-Kutta Prince-Dormand (8,9) solver from GSL library */
STEPCORE_DECLARE_GSLASOLVER(RK8PD, rk8pd);

/** \ingroup solvers \brief Runge-Kutta implicit second-order solver from GSL library */
STEPCORE_DECLARE_GSLSOLVER(RK2IMP, rk2imp);
/** \ingroup solvers \brief Runge-Kutta Prince-Dormand (8,9) solver from GSL library */
STEPCORE_DECLARE_GSLASOLVER(RK2IMP, rk2imp);

/** \ingroup solvers \brief Runge-Kutta implicit fourth-order solver from GSL library */
STEPCORE_DECLARE_GSLSOLVER(RK4IMP, rk4imp);
/** \ingroup solvers \brief Runge-Kutta implicit fourth-order solver from GSL library */
STEPCORE_DECLARE_GSLASOLVER(RK4IMP, rk4imp);

} // namespace StepCore

#endif // defined(STEPCORE_WITH_GSL) || defined(Q_MOC_RUN)

#endif // STEPCORE_GSLSOLVER_H

