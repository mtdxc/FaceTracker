#include "FaceTracker.h"
#define PI	3.14159265358979323846
#define DEG_TO_RAD (PI/180.0)
#define RAD_TO_DEG (180.0/PI)
float ofDegToRad(float degrees) {
	return degrees * DEG_TO_RAD;
}
float ofRadToDeg(float radians) {
	return radians * RAD_TO_DEG;
}

#define SET_ROW(row, v1, v2, v3, v4 )    \
_mat[(row)][0] = (v1); \
_mat[(row)][1] = (v2); \
_mat[(row)][2] = (v3); \
_mat[(row)][3] = (v4);
#define QX  q._v[0]
#define QY  q._v[1]
#define QZ  q._v[2]
#define QW  q._v[3]

void ofMatrix4x4::makeIdentityMatrix()
{
	SET_ROW(0, 1, 0, 0, 0)
	SET_ROW(1, 0, 1, 0, 0)
	SET_ROW(2, 0, 0, 1, 0)
	SET_ROW(3, 0, 0, 0, 1)
}

void ofMatrix4x4::makeRotationMatrix(float angle1, const ofVec3f& axis1, float angle2, const ofVec3f& axis2, float angle3, const ofVec3f& axis3)
{
	makeIdentityMatrix();

	ofQuaternion quat;
	quat.makeRotate(angle1, axis1,
		angle2, axis2,
		angle3, axis3);
	setRotate(quat);
}

void ofMatrix4x4::setRotate(const ofQuaternion& q)
{
	double length2 = q.length2();
	if (fabs(length2) <= std::numeric_limits<double>::min())
	{
		_mat[0][0] = 1.0; _mat[1][0] = 0.0; _mat[2][0] = 0.0;
		_mat[0][1] = 0.0; _mat[1][1] = 1.0; _mat[2][1] = 0.0;
		_mat[0][2] = 0.0; _mat[1][2] = 0.0; _mat[2][2] = 1.0;
	}
	else
	{
		double rlength2;
		// normalize quat if required.
		// We can avoid the expensive sqrt in this case since all 'coefficients' below are products of two q components.
		// That is a square of a square root, so it is possible to avoid that
		if (length2 != 1.0)
		{
			rlength2 = 2.0 / length2;
		}
		else
		{
			rlength2 = 2.0;
		}

		// Source: Gamasutra, Rotating Objects Using Quaternions
		//
		//http://www.gamasutra.com/features/19980703/quaternions_01.htm

		double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

		// calculate coefficients
		x2 = rlength2*QX;
		y2 = rlength2*QY;
		z2 = rlength2*QZ;

		xx = QX * x2;
		xy = QX * y2;
		xz = QX * z2;

		yy = QY * y2;
		yz = QY * z2;
		zz = QZ * z2;

		wx = QW * x2;
		wy = QW * y2;
		wz = QW * z2;

		// Note.  Gamasutra gets the matrix assignments inverted, resulting
		// in left-handed rotations, which is contrary to OpenGL and OSG's
		// methodology.  The matrix assignment has been altered in the next
		// few lines of code to do the right thing.
		// Don Burns - Oct 13, 2001
		_mat[0][0] = 1.0 - (yy + zz);
		_mat[1][0] = xy - wz;
		_mat[2][0] = xz + wy;


		_mat[0][1] = xy + wz;
		_mat[1][1] = 1.0 - (xx + zz);
		_mat[2][1] = yz - wx;

		_mat[0][2] = xz - wy;
		_mat[1][2] = yz + wx;
		_mat[2][2] = 1.0 - (xx + yy);
	}

#if 0
	_mat[0][3] = 0.0;
	_mat[1][3] = 0.0;
	_mat[2][3] = 0.0;

	_mat[3][0] = 0.0;
	_mat[3][1] = 0.0;
	_mat[3][2] = 0.0;
	_mat[3][3] = 1.0;
#endif
}


void ofQuaternion::makeRotate(const ofVec3f& from, const ofVec3f& to)
{
	// This routine takes any vector as argument but normalized
	// vectors are necessary, if only for computing the dot product.
	// Too bad the API is that generic, it leads to performance loss.
	// Even in the case the 2 vectors are not normalized but same length,
	// the sqrt could be shared, but we have no way to know beforehand
	// at this point, while the caller may know.
	// So, we have to test... in the hope of saving at least a sqrt
	ofVec3f sourceVector = from;
	ofVec3f targetVector = to;

	float fromLen2 = from.lengthSquared();
	float fromLen;
	// normalize only when necessary, epsilon test
	if ((fromLen2 < 1.0 - 1e-7) || (fromLen2 > 1.0 + 1e-7)) {
		fromLen = sqrt(fromLen2);
		sourceVector /= fromLen;
	}
	else fromLen = 1.0;

	float toLen2 = to.lengthSquared();
	// normalize only when necessary, epsilon test
	if ((toLen2 < 1.0 - 1e-7) || (toLen2 > 1.0 + 1e-7)) {
		float toLen;
		// re-use fromLen for case of mapping 2 vectors of the same length
		if ((toLen2 > fromLen2 - 1e-7) && (toLen2 < fromLen2 + 1e-7)) {
			toLen = fromLen;
		}
		else toLen = sqrt(toLen2);
		targetVector /= toLen;
	}


	// Now let's get into the real stuff
	// Use "dot product plus one" as test as it can be re-used later on
	double dotProdPlus1 = 1.0 + sourceVector.dot(targetVector);

	// Check for degenerate case of full u-turn. Use epsilon for detection
	if (dotProdPlus1 < 1e-7) {

		// Get an orthogonal vector of the given vector
		// in a plane with maximum vector coordinates.
		// Then use it as quaternion axis with pi angle
		// Trick is to realize one value at least is >0.6 for a normalized vector.
		if (fabs(sourceVector.x) < 0.6) {
			const double norm = sqrt(1.0 - sourceVector.x * sourceVector.x);
			_v.x = 0.0;
			_v.y = sourceVector.z / norm;
			_v.z = -sourceVector.y / norm;
			_v.w = 0.0;
		}
		else if (fabs(sourceVector.y) < 0.6) {
			const double norm = sqrt(1.0 - sourceVector.y * sourceVector.y);
			_v.x = -sourceVector.z / norm;
			_v.y = 0.0;
			_v.z = sourceVector.x / norm;
			_v.w = 0.0;
		}
		else {
			const double norm = sqrt(1.0 - sourceVector.z * sourceVector.z);
			_v.x = sourceVector.y / norm;
			_v.y = -sourceVector.x / norm;
			_v.z = 0.0;
			_v.w = 0.0;
		}
	}

	else {
		// Find the shortest angle quaternion that transforms normalized vectors
		// into one other. Formula is still valid when vectors are colinear
		const double s = sqrt(0.5 * dotProdPlus1);
		const ofVec3f tmp = sourceVector.getCrossed(targetVector) / (2.0 * s);
		_v.x = tmp.x;
		_v.y = tmp.y;
		_v.z = tmp.z;
		_v.w = s;
	}
}

void ofQuaternion::makeRotate(float angle, float x, float y, float z)
{
	angle = ofDegToRad(angle);

	const float epsilon = 0.0000001f;

	float length = sqrtf(x * x + y * y + z * z);
	if (length < epsilon) {
		// ~zero length axis, so reset rotation to zero.
		*this = ofQuaternion();
		return;
	}

	float inversenorm = 1.0f / length;
	float coshalfangle = cosf(0.5f * angle);
	float sinhalfangle = sinf(0.5f * angle);

	_v.x = x * sinhalfangle * inversenorm;
	_v.y = y * sinhalfangle * inversenorm;
	_v.z = z * sinhalfangle * inversenorm;
	_v.w = coshalfangle;
}

void ofQuaternion::makeRotate(float angle1, const ofVec3f& axis1, float angle2, const ofVec3f& axis2, float angle3, const ofVec3f& axis3)
{
	ofQuaternion q1; q1.makeRotate(angle1, axis1);
	ofQuaternion q2; q2.makeRotate(angle2, axis2);
	ofQuaternion q3; q3.makeRotate(angle3, axis3);

	*this = q1*q2*q3;
}