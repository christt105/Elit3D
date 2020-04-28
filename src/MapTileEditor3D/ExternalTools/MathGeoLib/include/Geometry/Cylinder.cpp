/* Copyright Jukka Jylänki

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

/** @file Cylinder.cpp
	@author Jukka Jylänki
	@brief Implementation for the Cylinder geometry object. */
#include "Cylinder.h"
#include "../Math/MathConstants.h"
#include "../Math/MathFunc.h"
#include "../Math/float3x3.h"
#include "../Math/float3x4.h"
#include "../Math/float4x4.h"
#include "../Math/Quat.h"
#include "AABB.h"
#include "OBB.h"
#include "Frustum.h"
#include "Plane.h"
#include "Ray.h"
#include "Line.h"
#include "LineSegment.h"
#include "Polygon.h"
#include "Polyhedron.h"
#include "Sphere.h"
#include "Circle.h"
#include "Triangle.h"
#include "../Algorithm/Random/LCG.h"
#include "../Math/assume.h"

#ifdef MATH_ENABLE_STL_SUPPORT
#include <iostream>
#endif

MATH_BEGIN_NAMESPACE

Cylinder::Cylinder(const LineSegment &endPoints, float radius)
:l(endPoints), r(radius)
{
}

Cylinder::Cylinder(const float3 &bottomPoint, const float3 &topPoint, float radius)
:l(bottomPoint, topPoint), r(radius)
{
}

float Cylinder::LineLength() const
{
	return l.Length();
}

float Cylinder::Height() const
{
	return l.Length();
}

float3 Cylinder::Center() const
{
	return l.CenterPoint();
}

MATH_END_NAMESPACE