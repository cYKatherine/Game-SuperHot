#include "Collisions.h"

#include <DirectXMath.h>

Vector3 ClosestPointInBoundingBox(const CBoundingBox& bb, const Vector3& point)
{
	Vector3 result = point;

	if (point.x < bb.GetMin().x)
		result.x = bb.GetMin().x;
	else if (point.x > bb.GetMax().x)
		result.x = bb.GetMax().x;

	if (point.y < bb.GetMin().y)
		result.y = bb.GetMin().y;
	else if (point.y > bb.GetMax().y)
		result.y = bb.GetMax().y;

	if (point.z < bb.GetMin().z)
		result.z = bb.GetMin().z;
	else if (point.z > bb.GetMax().z)
		result.z = bb.GetMax().z;

	return result;
}

bool CheckCollision(const CBoundingSphere& sphere, const Vector3& point)
{
	return Vector3::Distance(sphere.GetCenter(), point) <= sphere.GetRadius();
}
bool CheckCollision(const CBoundingSphere& sphere1, const CBoundingSphere& sphere2)
{
	return Vector3::Distance(sphere1.GetCenter(), sphere2.GetCenter()) <= (sphere1.GetRadius() + sphere2.GetRadius());
}
bool CheckCollision(const CBoundingSphere& sphere, const CBoundingBox& bb)
{
	return Vector3::Distance(ClosestPointInBoundingBox(bb, sphere.GetCenter()), sphere.GetCenter()) <= sphere.GetRadius();
}
bool CheckCollision(const CBoundingBox& bb, const Vector3& point)
{
	return (point.x > bb.GetMin().x && point.x < bb.GetMax().x) && 
		   (point.y > bb.GetMin().y && point.y < bb.GetMax().y) && 
		   (point.z > bb.GetMin().z && point.z < bb.GetMax().z);
}
bool CheckCollision(const CBoundingBox& bb1, const CBoundingBox& bb2)
{
	return (bb1.GetMax().x > bb2.GetMin().x &&
			bb1.GetMax().y > bb2.GetMin().y &&
			bb1.GetMax().z > bb2.GetMin().z &&
			bb1.GetMin().x < bb2.GetMax().x &&
			bb1.GetMin().y < bb2.GetMax().y &&
			bb1.GetMin().z < bb2.GetMax().z);
}
PLANE_RESULT CheckPlane(const CPlane& plane, const Vector3& point)
{
	float distance = plane.GetNormal().Dot(point) + plane.GetDistance();
	
	if (distance > ON_PLANE_AMOUNT)
		return PLANE_IN_FRONT;
	else if (distance < -ON_PLANE_AMOUNT)
		return PLANE_BEHIND;
	else
		return PLANE_ON;
}
PLANE_RESULT CheckPlane(const CPlane& plane, const CBoundingSphere& sphere)
{
	float distance = plane.GetNormal().Dot(sphere.GetCenter()) + plane.GetDistance();
	
	if (distance > 0 && distance > sphere.GetRadius())
		return PLANE_IN_FRONT;
	else if (distance < 0 && -distance > sphere.GetRadius())
		return PLANE_BEHIND;
	else
		return PLANE_ON;

}
PLANE_RESULT CheckPlane(const CPlane& plane, const CBoundingBox& bb)
{
	Vector3 diagMin, diagMax;

	if (plane.GetNormal().x >= 0)
	{
		diagMin.x = bb.GetMin().x;
		diagMax.x = bb.GetMax().x;
	}
	else
	{
		diagMin.x = bb.GetMax().x;
		diagMax.x = bb.GetMin().x;
	}

	if (plane.GetNormal().y >= 0)
	{
		diagMin.y = bb.GetMin().y;
		diagMax.y = bb.GetMax().y;
	}
	else
	{
		diagMin.y = bb.GetMax().y;
		diagMax.y = bb.GetMin().y;
	}

	if (plane.GetNormal().z >= 0)
	{
		diagMin.z = bb.GetMin().z;
		diagMax.z = bb.GetMax().z;
	}
	else
	{
		diagMin.z = bb.GetMax().z;
		diagMax.z = bb.GetMin().z;
	}

	float distanceFromMin = plane.GetNormal().x * diagMin.x +
		plane.GetNormal().y * diagMin.y +
		plane.GetNormal().z * diagMin.z +
		plane.GetDistance();
	if (distanceFromMin > 0.0f)
		return PLANE_IN_FRONT;

	float distanceFromMax = plane.GetNormal().x * diagMax.x +
		plane.GetNormal().y * diagMax.y +
		plane.GetNormal().z * diagMax.z +
		plane.GetDistance();
	if (distanceFromMax < 0.0f)
		return PLANE_BEHIND;

	return PLANE_ON;
}

//http://www.cosinekitty.com/raytrace/chapter06_sphere.html
bool CheckRay(const CRay& ray, const CBoundingSphere& sphere, Vector3* hitPoint)
{
	Vector3 displacement = ray.GetOrigin() - sphere.GetCenter();
	float a = ray.GetDirection().LengthSquared();
	float b = 2.0f * displacement.Dot(ray.GetDirection());
	float c = displacement.LengthSquared() - sphere.GetRadius() * sphere.GetRadius();

	float randicand = b*b - 4.0f * a * c;
	if (randicand < 0.0f)
	{
		return false;
	}
	

	float root = sqrt(randicand);
	float denom = 2.0 * a;

	float hit1 = (-b + root) / denom;
	float hit2 = (-b - root) / denom;

	if (hit1 < 0 && hit2 < 0)
	{
		return false;
	}

	
	if (hitPoint)
	{
		Vector3 pointOnSphere = (ray.GetDirection() * min(hit1, hit2)) + ray.GetOrigin();
		hitPoint->x = pointOnSphere.x;
		hitPoint->y = pointOnSphere.y;
		hitPoint->z = pointOnSphere.z;
	}
	return true;
}

//http://tavianator.com/fast-branchless-raybounding-box-intersections/
bool CheckRay(const CRay& ray, const CBoundingBox& bb, Vector3* hitPoint)
{
	float tmin = -INFINITY;
	float tmax = INFINITY;

	float tx1 = (bb.GetMin().x - ray.GetOrigin().x) / ray.GetDirection().x;
	float tx2 = (bb.GetMax().x - ray.GetOrigin().x) / ray.GetDirection().x;

	tmin = max(tmin, min(tx1, tx2));
	tmax = min(tmax, max(tx1, tx2));
	
	float ty1 = (bb.GetMin().y - ray.GetOrigin().y) / ray.GetDirection().y;
	float ty2 = (bb.GetMax().y - ray.GetOrigin().y) / ray.GetDirection().y;

	tmin = max(tmin, min(ty1, ty2));
	tmax = min(tmax, max(ty1, ty2));
	
	float tz1 = (bb.GetMin().z - ray.GetOrigin().z) / ray.GetDirection().z;
	float tz2 = (bb.GetMax().z - ray.GetOrigin().z) / ray.GetDirection().z;

	tmin = max(tmin, min(tz1, tz2));
	tmax = min(tmax, max(tz1, tz2));
	

	if (tmax < 0)
		return false;

	if (tmax >= tmin)
	{
		if (hitPoint)
		{
			float firstHitDistance = tmin < 0 ? tmax : tmin;
			Vector3 pointOnBox = (ray.GetDirection() * firstHitDistance) + ray.GetOrigin();
		
			hitPoint->x = pointOnBox.x;
			hitPoint->y = pointOnBox.y;
			hitPoint->z = pointOnBox.z;
		}
		return true;
	}
	return false;
}
bool CheckRay(const CRay& ray, const CPlane& plane, Vector3* hitPoint)
{
	float planeRayDirection = plane.GetNormal().Dot(ray.GetDirection());

	if (planeRayDirection == 0)
	{
		return false;
	}

	float t = -(plane.GetNormal().Dot(ray.GetOrigin()) + plane.GetDistance()) / planeRayDirection;
	if (t < 0)
	{
		return false;
	}
	if (hitPoint)
	{
		Vector3 pointOnPlane = (ray.GetDirection() * t) + ray.GetOrigin();
		hitPoint->x = pointOnPlane.x;
		hitPoint->y = pointOnPlane.y;
		hitPoint->z = pointOnPlane.z;
	}
	return true;
}