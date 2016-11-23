/*
 * PolygonF.cpp
 *
 *  Created on: November 22nd, 2016
 *  Author: Daniel Gerendasy
 */

#include <Mini2D/Units.hpp>					// Class definition

int orientation(Vector2 *p, Vector2 *q, Vector2 *r);
bool polyContainPoint(Vector2 * p, Vector2 *poly[], int polyCount, Vector2 * slope);

bool PolygonF::IntersectConvex(Vector2 * polyStatic[], int polyStaticCount, Vector2 * polyMoving[], int polyMovingCount, Vector2 * normal, int * points) {
	int x,c=0;
	Vector2 slope;

	if (!polyStatic || !polyMoving || polyStaticCount <= 0 || polyMovingCount <= 0)
		return 0;

	for (x=0;x<polyMovingCount;x++) {
		if (polyContainPoint(polyMoving[x], polyStatic, polyStaticCount, &slope)) {
			if (normal) {
				normal->X -= slope.Y;
				normal->Y += slope.X;
			}
			c++;
		}
	}

	if (normal && c > 0) {
		// If the normal is invalid, return false
		if (!normal->X && !normal->Y)
			return 0;

		normal->Normalize();
	}

	if (points)
		*points = c;
	return c>0 && c<polyMovingCount;
}

bool PolygonF::IntersectCircle(Vector2 * polyStatic[], int polyStaticCount, CircleF * circleMoving, Vector2 * normal) {
	int x,y,c=0;
	float dist;
	Vector2 temp;
	Vector2 * center;

	if (!polyStatic || !circleMoving || polyStaticCount <= 0)
		return 0;

	center = circleMoving->GetRotatedCenter();

	for (x=0;x<polyStaticCount;x++) {
		y = (x==(polyStaticCount-1)) ? 0 : (x+1);
		dist = abs(Vector2::CrossProduct(polyStatic[x],polyStatic[y],center))/Vector2::DistanceFrom(polyStatic[y],polyStatic[x]);
		if (dist <= circleMoving->Radius) {
			if (normal) {
				temp.Set((*polyStatic[y])-(*polyStatic[x]));
				normal->X -= temp.Y;
				normal->Y += temp.X;
			}
			c++;
		}
	}

	if (normal && c > 0) {
		// If the normal is invalid, return false
		if (!normal->X && !normal->Y)
			return 0;

		normal->Normalize();
	}

	return c!=0;
}

/*
 * From: http://blog.theliuy.com/determine-if-two-rotated-rectangles-overlap-each-other/
 */
int orientation(Vector2 *p, Vector2 *q, Vector2 *r)
{
    int val = Vector2::CrossProduct(p,q,r);
	if (0 == val)
		return 0;
	return val > 0 ? 1: 2;
}

bool polyContainPoint(Vector2 * p, Vector2 *poly[], int polyCount, Vector2 * slope) {
	int x,o;
	float distances[4], distance;
	Vector2 * temp;
	if (polyCount < 2)
		return 0;

    // orientation
	o = orientation(poly[0], poly[1], p);

	if (o == 0)
		return 1;

    for (x=1;x<polyCount;x++) {
    	if (o != orientation(poly[x], poly[((x==polyCount-1)?0:x+1)], p)) {
    		return 0;
    	}
    	else if (slope) {
    		temp = poly[((x==polyCount-1)?0:x+1)];
    		distances[x] = abs(Vector2::CrossProduct(poly[x],temp,p))/Vector2::DistanceFrom(temp,poly[x]);
    	}
    }
    
    if (slope) {
    	distances[0] = abs(Vector2::CrossProduct(poly[0],poly[1],p))/Vector2::DistanceFrom(poly[1],poly[0]);
	    distance = distances[0];
	    o = 0;

	    for (x=1;x<4;x++) {
	    	if (distances[x]<distance) {
	    		distance = distances[x];
	    		o = x;
	    	}
	    }

	    slope->Set(*poly[((o==polyCount-1)?0:o+1)]-*poly[o]);
	}

    return 1;
}
