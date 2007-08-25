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

#include "spring.h"
#include "types.h"

#include <algorithm>
#include <cmath>

namespace StepCore {

STEPCORE_META_OBJECT(Spring, "Massless spring", 0,
    STEPCORE_SUPER_CLASS(Item) STEPCORE_SUPER_CLASS(Force),
    STEPCORE_PROPERTY_RW(double, restLength, "m", "Rest length", restLength, setRestLength)
    STEPCORE_PROPERTY_R_D(double, length, "m", "Current length", length)
    STEPCORE_PROPERTY_RW(double, stiffness, "N/m", "Stiffness", stiffness, setStiffness)
    STEPCORE_PROPERTY_RW(double, damping, "N s/m", "Damping", damping, setDamping)
    STEPCORE_PROPERTY_RW(QString, body1, STEPCORE_UNITS_NULL, "Body1", body1, setBody1)
    STEPCORE_PROPERTY_RW(QString, body2, STEPCORE_UNITS_NULL, "Body2", body2, setBody2)
    STEPCORE_PROPERTY_RW(StepCore::Vector2d, localPosition1, "m",
                    "Local position 1", localPosition1, setLocalPosition1)
    STEPCORE_PROPERTY_RW(StepCore::Vector2d, localPosition2, "m",
                    "Local position 2", localPosition2, setLocalPosition2)
    STEPCORE_PROPERTY_R_D(StepCore::Vector2d, position1, "m", "Position1", position1)
    STEPCORE_PROPERTY_R_D(StepCore::Vector2d, position2, "m", "Position2", position2)
    STEPCORE_PROPERTY_R_D(double, force, "N", "Spring tension force", force)
    )

STEPCORE_META_OBJECT(SpringErrors, "Errors class for Spring", 0,
    STEPCORE_SUPER_CLASS(ObjectErrors),
    STEPCORE_PROPERTY_RW(double, restLengthVariance, "m",
                    "Rest length variance", restLengthVariance, setRestLengthVariance)
    STEPCORE_PROPERTY_R_D(double, lengthVariance, "m",
                    "Current length variance", lengthVariance)
    STEPCORE_PROPERTY_RW(double, stiffnessVariance, "N/m",
                    "Stiffness variance", stiffnessVariance, setStiffnessVariance)
    STEPCORE_PROPERTY_RW(double, dampingVariance, "N/m",
                    "Damping variance", dampingVariance, setDampingVariance)
    STEPCORE_PROPERTY_RW(StepCore::Vector2d, localPosition1Variance, "m",
                    "Local position 1 variance", localPosition1Variance, setLocalPosition1Variance)
    STEPCORE_PROPERTY_RW(StepCore::Vector2d, localPosition2Variance, "m",
                    "Local position 2 variance", localPosition2Variance, setLocalPosition2Variance)
    STEPCORE_PROPERTY_R_D(StepCore::Vector2d, position1Variance, "m",
                    "Position1 variance", position1Variance)
    STEPCORE_PROPERTY_R_D(StepCore::Vector2d, position2Variance, "m",
                    "Position2 variance", position2Variance)
    STEPCORE_PROPERTY_R_D(double, forceVariance, "N",
                    "Spring tension force variance", forceVariance)
    )

Spring* SpringErrors::spring() const
{
    return static_cast<Spring*>(owner());
}

Spring::Spring(double restLength, double stiffness, double damping, Body* bodyPtr1, Body* bodyPtr2)
    : _restLength(restLength),
      _stiffness(stiffness), _damping(damping),
      _localPosition1(0), _localPosition2(0) //, _position1(0), _position2(0)
{
    setBodyPtr1(bodyPtr1);
    setBodyPtr2(bodyPtr2);
}

void Spring::calcForce(bool calcVariances)
{
    /*
    // XXX: SLOW checks !
    if(_bodyPtr1 == NULL || _bodyPtr2 == NULL) return;
    if(std::find(world()->bodies().begin(), world()->bodies().end(), _bodyPtr1)
                == world()->bodies().end()) return;
    if(std::find(world()->bodies().begin(), world()->bodies().end(), _bodyPtr2)
                == world()->bodies().end()) return;
    */

    Particle* p1 = dynamic_cast<Particle*>(_bodyPtr1);
    Particle* p2 = dynamic_cast<Particle*>(_bodyPtr2);

    RigidBody* r1 = dynamic_cast<RigidBody*>(_bodyPtr1);
    RigidBody* r2 = dynamic_cast<RigidBody*>(_bodyPtr2);

    Vector2d r = position2() - position1();
    Vector2d v = velocity2() - velocity1();

    double l = r.norm();
    double dl = l - _restLength;
    double vr = v.innerProduct(r);
    Vector2d force = (_stiffness*dl + _damping*vr/l) / l * r;
    
    if(p1) p1->addForce(force);
    else if(r1) r1->applyForce(force, position1());

    force.invert();
    if(p2) p2->addForce(force);
    else if(r2) r2->applyForce(force, position2());

    if(calcVariances) {
        SpringErrors* se = springErrors();

        Vector2d rV = se->position2Variance() + se->position1Variance();
        Vector2d vV = se->velocity2Variance() + se->velocity1Variance();

        Vector2d forceV = (se->_restLengthVariance * square(_stiffness) +
                           se->_stiffnessVariance * square(dl) +
                           se->_dampingVariance * square(vr/l) +
                           vV.innerProduct( (_damping/l*r).cSquare() )
                           )/square(l)*r.cSquare();

        forceV[0] += rV[0] * square(_stiffness*( 1 - _restLength/l*(1 - square(r[0]/l)) ) +
                                    _damping/(l*l)*( v[0]*r[0] + vr - 2*vr*square(r[0]/l) )) +
                     rV[1] * square(_stiffness*_restLength*r[0]*r[1]/(l*l*l) +
                                    _damping/(l*l)*( v[1]*r[0] - 2*vr*r[0]*r[1]/(l*l) ));
        forceV[1] += rV[1] * square(_stiffness*( 1 - _restLength/l*(1 - square(r[1]/l)) ) +
                                    _damping/(l*l)*( v[1]*r[1] + vr - 2*vr*square(r[1]/l) )) +
                     rV[0] * square(_stiffness*_restLength*r[0]*r[1]/(l*l*l) +
                                    _damping/(l*l)*( v[0]*r[1] - 2*vr*r[0]*r[1]/(l*l) ));

        if(p1) p1->particleErrors()->addForceVariance(forceV);
        //else if(r1) r1->applyForce(force, position1());

        if(p2) p2->particleErrors()->addForceVariance(forceV);
        //else if(r2) r2->applyForce(force, position2());
    }
}

void Spring::setBodyPtr1(Body* bodyPtr1)
{
    if(dynamic_cast<Particle*>(bodyPtr1) || dynamic_cast<RigidBody*>(bodyPtr1)) {
        //_localPosition1.setZero();
        _bodyPtr1 = bodyPtr1;
    } else {
        //Particle* p1 = dynamic_cast<Particle*>(_bodyPtr1);
        //RigidBody* r1 = dynamic_cast<RigidBody*>(_bodyPtr1);
        //if(p1) _localPosition1 = p1->position();
        //else if(r1) _localPosition1 = r1->position();
        _bodyPtr1 = NULL;
    }
}

void Spring::setBodyPtr2(Body* bodyPtr2)
{
    if(dynamic_cast<Particle*>(bodyPtr2) || dynamic_cast<RigidBody*>(bodyPtr2)) {
        //_localPosition2.setZero();
        _bodyPtr2 = bodyPtr2;
    } else {
        //Particle* p2 = dynamic_cast<Particle*>(_bodyPtr2);
        //RigidBody* r2 = dynamic_cast<RigidBody*>(_bodyPtr2);
        //if(p2) _localPosition2 = p2->position();
        //else if(r2) _localPosition2 = r2->position();
        _bodyPtr2 = NULL;
    }
}

Vector2d Spring::position1() const
{
    Particle* p1 = dynamic_cast<Particle*>(_bodyPtr1);
    if(p1) return p1->position() + _localPosition1;

    RigidBody* r1 = dynamic_cast<RigidBody*>(_bodyPtr1);
    if(r1) return r1->pointLocalToWorld(_localPosition1);

    return _localPosition1;
}

Vector2d SpringErrors::position1Variance() const
{
    Particle* p1 = dynamic_cast<Particle*>(spring()->bodyPtr1());
    if(p1) return p1->particleErrors()->positionVariance() + _localPosition1Variance;

    // XXX: TODO
    //RigidBody* r1 = dynamic_cast<RigidBody*>(_bodyPtr1);
    //if(r1) return r1->pointLocalToWorld(_localPosition1);

    return _localPosition1Variance;
}

Vector2d Spring::position2() const
{
    Particle* p2 = dynamic_cast<Particle*>(_bodyPtr2);
    if(p2) return p2->position() + _localPosition2;

    RigidBody* r2 = dynamic_cast<RigidBody*>(_bodyPtr2);
    if(r2) return r2->pointLocalToWorld(_localPosition2);

    return _localPosition2;
}

Vector2d SpringErrors::position2Variance() const
{
    Particle* p2 = dynamic_cast<Particle*>(spring()->bodyPtr2());
    if(p2) return p2->particleErrors()->positionVariance() + _localPosition2Variance;

    // XXX: TODO
    //RigidBody* r2 = dynamic_cast<RigidBody*>(_bodyPtr2);
    //if(r2) return r2->pointLocalToWorld(_localPosition2);

    return _localPosition2Variance;
}

double SpringErrors::lengthVariance() const
{
    Vector2d r = spring()->position2() - spring()->position1();
    Vector2d rV = position2Variance() + position1Variance();
    return (r[0]*r[0]*rV[0] + r[1]*r[1]*rV[1])/r.norm2();
}

Vector2d Spring::velocity1() const
{
    Particle* p1 = dynamic_cast<Particle*>(_bodyPtr1);
    if(p1) return p1->velocity();

    RigidBody* r1 = dynamic_cast<RigidBody*>(_bodyPtr1);
    if(r1) return r1->velocityWorld(_localPosition1);

    return Vector2d(0);
}

Vector2d SpringErrors::velocity1Variance() const
{
    Particle* p1 = dynamic_cast<Particle*>(spring()->bodyPtr1());
    if(p1) return p1->particleErrors()->velocityVariance();

    // XXX: TODO
    //RigidBody* r1 = dynamic_cast<RigidBody*>(_bodyPtr1);
    //if(r1) return r1->pointLocalToWorld(_localPosition1);

    return Vector2d(0);
}

Vector2d Spring::velocity2() const
{
    Particle* p2 = dynamic_cast<Particle*>(_bodyPtr2);
    if(p2) return p2->velocity();

    RigidBody* r2 = dynamic_cast<RigidBody*>(_bodyPtr2);
    if(r2) return r2->velocityWorld(_localPosition2);

    return Vector2d(0);
}

Vector2d SpringErrors::velocity2Variance() const
{
    Particle* p2 = dynamic_cast<Particle*>(spring()->bodyPtr2());
    if(p2) return p2->particleErrors()->velocityVariance();

    // XXX: TODO
    //RigidBody* r2 = dynamic_cast<RigidBody*>(_bodyPtr2);
    //if(r2) return r2->pointLocalToWorld(_localPosition2);

    return Vector2d(0);
}

double Spring::force() const
{
    Vector2d r = position2() - position1();
    Vector2d v = velocity2() - velocity1();
    double l = r.norm();
    return _stiffness * (l - _restLength) +
                _damping * v.innerProduct(r)/l;
}

double SpringErrors::forceVariance() const
{
    Spring* s = spring();
    Vector2d r = s->position2() - s->position1();
    Vector2d v = s->velocity2() - s->velocity1();
    Vector2d rV = position2Variance() + position1Variance();
    Vector2d vV = velocity2Variance() + velocity1Variance();
    double l = r.norm();
    double dl = l - s->restLength();
    // XXX: CHECKME
    return square(dl) * _stiffnessVariance +
           square(s->stiffness()) * _restLengthVariance +
           square(v.innerProduct(r)/l) * _dampingVariance +
           (s->damping()/l*r).cSquare().innerProduct(vV) +
           (( s->stiffness() - s->damping()*v.innerProduct(r) / (l*l) ) / l * r +
              s->damping() / l * v).cSquare().innerProduct(rV);
}

void Spring::worldItemRemoved(Item* item)
{
    if(item == NULL) return;
    if(item == dynamic_cast<Item*>(_bodyPtr1)) {
        //Particle* p1 = dynamic_cast<Particle*>(_bodyPtr1);
        //RigidBody* r1 = dynamic_cast<RigidBody*>(_bodyPtr1);
        //if(p1) _localPosition1 = p1->position();
        //else if(r1) _localPosition1 = r1->position();
        _bodyPtr1 = NULL;
    }
    if(item == dynamic_cast<Item*>(_bodyPtr2)) {
        //Particle* p2 = dynamic_cast<Particle*>(_bodyPtr2);
        //RigidBody* r2 = dynamic_cast<RigidBody*>(_bodyPtr2);
        //if(p2) _localPosition2 = p2->position();
        //else if(r2) _localPosition2 = r2->position();
        _bodyPtr2 = NULL;
    }
}

void Spring::setWorld(World* world)
{
    if(world == NULL) {
        _bodyPtr1 = NULL;
        _bodyPtr2 = NULL;
    } else if(this->world() != NULL) { 
        if(_bodyPtr1 != NULL)
            _bodyPtr1 = dynamic_cast<Body*>(world->item(body1()));
            //dynamic_cast<Body*>(
            //    world->items()[ this->world()->itemIndex(dynamic_cast<const Item*>(_bodyPtr1)) ]);
        if(_bodyPtr2 != NULL)
            _bodyPtr2 = dynamic_cast<Body*>(world->item(body2()));
            //_bodyPtr2 = dynamic_cast<Body*>(
            //    world->items()[ this->world()->itemIndex(dynamic_cast<const Item*>(_bodyPtr2)) ]);
    }
    Item::setWorld(world);
}

} // namespace StepCore

