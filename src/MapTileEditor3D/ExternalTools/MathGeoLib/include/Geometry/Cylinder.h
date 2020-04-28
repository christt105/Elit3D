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

/** @file Cylinder.h
	@author Jukka Jylänki
	@brief The Cylinder geometry object. */
#pragma once

#include "../MathGeoLibFwd.h"
#include "LineSegment.h"

MATH_BEGIN_NAMESPACE

/// A 3D cylinder 
class Cylinder
{
public:
	/// Specifies the two inner points of this capsule.
	LineSegment l;

	/// Specifies the radius of this cylinder. [similarOverload: l]
	float r;

	/// The default constructor does not initialize any members of this class.
	/** This means that the values of the members l and r are both undefined after creating a new capsule using
		this default constructor. Remember to assign to them before use.
		@see l, r. */
	Cylinder() {}

	/// Constructs a new capsule by explicitly specifying the member variables.
	/** @param endPoints Specifies the line segment of the capsule.
		@param radius Specifies the size of this capsule.
		@see l, r. */
	Cylinder(const LineSegment &endPoints, float radius);

	/// Constructs a new capsule by explicitly specifying the member variables.
	/** This constructor is equivalent to calling Cylinder(LineSegment(bottomPoint, topPoint), radius), but provided
		here for conveniency.
		@see l, r. */
	Cylinder(const float3 &bottomPoint, const float3 &topPoint, float radius);

	/// Computes the distance of the two inner points of this capsule.
	/** <img src="CylinderFunctions.png" />
		@see Height(). */
	float LineLength() const;

	/// Computes the total height of this capsule, i.e. LineLength() + Diameter().
	/** <img src="CylinderFunctions.png" />
		@see LineLength(). */
	float Height() const;

	/// Returns the center point of this Cylinder.
	/** <img src="CylinderFunctions.png" />
		@return The point (l.a + l.b) / 2. This point is the center of mass for this capsule.
		@see l, Bottom(), Top(). */
	float3 Center() const;
	float3 Centroid() const { return l.CenterPoint(); } ///< [similarOverload: Center]

};
MATH_END_NAMESPACE
