#pragma once

#include "Lumos.h"

struct LUMOS_API WRTK_Math
{
	//given a circle (derived from its center, a point on its circumference, and a normal) and a point A, return the point on the circle's circumference closest to A
	static FVector ProjectPointOntoCircle(FVector circleCenter, FVector pointOnCircumference, FVector circleNormal, FVector pointToProject)
	{
		/*
		find closest point on circle from hand
		loosely based on this stackexchange: https://math.stackexchange.com/questions/31049/distance-from-a-point-to-circles-closest-point
		Project the point onto the plane in which the circle lies.
		Then take the distance to the circle's centre, subtract the radius and take the absolute value to get the distance within the plane.
		Then you get the total distance from the distance to the plane and the distance within the plane using Pythagoras.
		*/

		//project onto plane
		FVector pointToHand = pointToProject - circleCenter;
		float distance = FVector::DotProduct(pointToHand, circleNormal);
		FVector pointOnPlane = pointToProject - circleNormal * distance;

		//convert point on plane to point on circle by clamping distance of vector from center to point
		FVector radiusVector = pointOnCircumference - circleCenter;
		float circleRadius = radiusVector.Size();
		FVector centerToPointOnPlane = pointOnPlane - circleCenter;
		FVector centerToPointOnCircle = centerToPointOnPlane.GetClampedToSize(circleRadius, circleRadius);
		FVector pointOnCircle = centerToPointOnCircle + circleCenter;

		return pointOnCircle;
	}

	static float GetAngleBetweenTwoVectors(FVector v1, FVector v2)
	{
		float VECTOR_EQUALS_TOLERANCE = 0.01f;

		v1.Normalize();
		v2.Normalize();

		//check special cases first
		if (v1.Equals(v2, VECTOR_EQUALS_TOLERANCE))
		{
			return 0.0f;
		}
		else if (v1.Equals(-v2, VECTOR_EQUALS_TOLERANCE))
		{
			return 180.0f;
		}

		float angleBetween = ((acosf(FVector::DotProduct(v1, v2))) * (180.0f / PI));
		check(FMath::IsNaN(angleBetween) == false);

		return angleBetween;
	}

	static float GetSignedAngleBetweenTwoVectors(FVector v1, FVector v2, FVector normal)
	{
		float angleBetween = GetAngleBetweenTwoVectors(v1, v2);

		//mark the angle as positive or negative for clockwise/counterclockwise
		FVector cross = FVector::CrossProduct(v1, v2);
		float dot = FVector::DotProduct(cross, normal);
		if (dot > 0)
		{
			angleBetween *= -1;
		}

		return angleBetween;
	}

	//returns the angle between to points on a circle (in 3D space, so we need a normal)
	static float GetAngleBetweenTwoPointsOnACircle(FVector circleCenter, FVector circleNormal, FVector pointA, FVector pointB)
	{
		FVector centerToCircumference = pointA - circleCenter; centerToCircumference.Normalize();
		FVector centerToProjectedPoint = pointB - circleCenter; centerToProjectedPoint.Normalize();

		return GetSignedAngleBetweenTwoVectors(centerToCircumference, centerToProjectedPoint, circleNormal);
	}

	//returns the angle between pointOnCircumference and the result of projecting pointToProject on the circle
	static float ProjectPointOntoCircleAndGetAngle(FVector circleCenter, FVector pointOnCircumference, FVector circleNormal, FVector pointToProject)
	{
		FVector projectedPoint = ProjectPointOntoCircle(circleCenter, pointOnCircumference, circleNormal, pointToProject);
		float angleBetween = GetAngleBetweenTwoPointsOnACircle(circleCenter, circleNormal, pointOnCircumference, pointToProject);
		return angleBetween;
	}

	//returns the vector projection of vec projected onto a plane with normal planeNormal
	static FVector ProjectVectorOntoPlane(FVector vec, FVector planeNormal)
	{
		//from http://www.euclideanspace.com/maths/geometry/elements/plane/lineOnPlane/
		//if A is the vector to be projected onto a plane with normal N:
		//A || N = N × (A×N / |N|) / |N|

		//N should already be normalized so we can assume its magnitude is 1 to get:
		//A || N = N x (AxN)
		
		return FVector::CrossProduct(planeNormal, FVector::CrossProduct(vec, planeNormal));
	}
	
	//given 3 coplanar vectors A, B, and C, returns whether B is between A and C
	static bool IsVectorBetweenTwoOthers(FVector outerVector1, FVector innerVector, FVector outerVector2)
	{
		//determine whether the constrained vector B is within the two threshold vectors A and C
		FVector AxB = FVector::CrossProduct(outerVector1, innerVector);
		FVector AxC = FVector::CrossProduct(outerVector1, outerVector2);
		FVector CxB = FVector::CrossProduct(outerVector2, innerVector);
		FVector CxA = FVector::CrossProduct(outerVector2, outerVector1);
		float directionTest1 = FVector::DotProduct(AxB, AxC);
		float directionTest2 = FVector::DotProduct(CxB, CxA);
		if (directionTest1 > 0 && directionTest2 > 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};