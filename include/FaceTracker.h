#pragma once

#include <deque>
#include <vector>
#include <math.h>
using std::vector;
#if (defined WIN32 || defined _WIN32 || defined WINCE)
#ifdef _WINDLL
#  define OFX_EXPORTS __declspec(dllexport)
#else
#  define OFX_EXPORTS __declspec(dllimport)
#endif
#else
#  define OFX_EXPORTS __attribute__((visibility("default")))
#endif

struct ofVec2f{
	static const int DIM = 2;
	float x, y;

	ofVec2f() { x = y = 0; }
	ofVec2f(float f1, float f2) :x(f1), y(f2) {}

	float * getPtr() {
		return (float*)&x;
	}
	const float * getPtr() const {
		return (const float *)&x;
	}

	float& operator[](int n) {
		return getPtr()[n];
	}

	float operator[](int n) const {
		return getPtr()[n];
	}

	bool operator==(const ofVec2f& vec) const {
		return (x == vec.x) && (y == vec.y);
	}

	bool operator!=(const ofVec2f& vec) const {
		return (x != vec.x) || (y != vec.y);
	}

	ofVec2f operator+(const ofVec2f& vec) const {
		return ofVec2f(x + vec.x, y + vec.y);
	}

	ofVec2f& operator+=(const ofVec2f& vec) {
		x += vec.x;
		y += vec.y;
		return *this;
	}

	ofVec2f operator-(const ofVec2f& vec) const {
		return ofVec2f(x - vec.x, y - vec.y);
	}

	ofVec2f& operator-=(const ofVec2f& vec) {
		x -= vec.x;
		y -= vec.y;
		return *this;
	}

	ofVec2f operator*(const ofVec2f& vec) const {
		return ofVec2f(x*vec.x, y*vec.y);
	}

	ofVec2f& operator*=(const ofVec2f& vec) {
		x *= vec.x;
		y *= vec.y;
		return *this;
	}

	ofVec2f operator/(const ofVec2f& vec) const {
		return ofVec2f(vec.x != 0 ? x / vec.x : x, vec.y != 0 ? y / vec.y : y);
	}

	ofVec2f& operator/=(const ofVec2f& vec) {
		vec.x != 0 ? x /= vec.x : x;
		vec.y != 0 ? y /= vec.y : y;
		return *this;
	}
	ofVec2f operator+(const float f) const {
		return ofVec2f(x + f, y + f);
	}

	ofVec2f& operator+=(const float f) {
		x += f;
		y += f;
		return *this;
	}

	ofVec2f operator-(const float f) const {
		return ofVec2f(x - f, y - f);
	}

	ofVec2f& operator-=(const float f) {
		x -= f;
		y -= f;
		return *this;
	}

	ofVec2f operator-() const {
		return ofVec2f(-x, -y);
	}

	ofVec2f operator*(const float f) const {
		return ofVec2f(x*f, y*f);
	}

	ofVec2f& operator*=(const float f) {
		x *= f;
		y *= f;
		return *this;
	}

	ofVec2f operator/(const float f) const {
		if (f == 0) return ofVec2f(x, y);

		return ofVec2f(x / f, y / f);
	}

	ofVec2f& operator/=(const float f) {
		if (f == 0) return *this;

		x /= f;
		y /= f;
		return *this;
	}
};

struct ofVec3f {
	static const int DIM = 3;
	float x, y, z;
	ofVec3f() { x = y = z = 0; }
	ofVec3f(const ofVec2f& f):x(f.x),y(f.y),z(0) {}
	ofVec3f(float f1, float f2, float f3):x(f1),y(f2),z(f3) {}

	float * getPtr() {
		return (float*)&x;
	}
	const float * getPtr() const {
		return (const float *)&x;
	}

	float& operator[](int n) {
		return getPtr()[n];
	}

	float operator[](int n) const {
		return getPtr()[n];
	}

	bool operator==(const ofVec3f& vec) const {
		return (x == vec.x) && (y == vec.y) && (z == vec.z);
	}

	bool operator!=(const ofVec3f& vec) const {
		return (x != vec.x) || (y != vec.y) || (z != vec.z);
	}

	ofVec3f operator-(const ofVec3f& vec) const {
		return ofVec3f(x - vec.x, y - vec.y, z - vec.z);
	}
	ofVec3f operator+(const float f) const {
		return ofVec3f(x + f, y + f, z + f);
	}

	ofVec3f& operator+=(const float f) {
		x += f;
		y += f;
		z += f;
		return *this;
	}

	ofVec3f operator-(const float f) const {
		return ofVec3f(x - f, y - f, z - f);
	}

	ofVec3f& operator-=(const float f) {
		x -= f;
		y -= f;
		z -= f;
		return *this;
	}

	ofVec3f operator*(const float f) const {
		return ofVec3f(x*f, y*f, z*f);
	}

	ofVec3f& operator*=(const float f) {
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	ofVec3f operator/(const float f) const {
		if (f == 0) return ofVec3f(x, y, z);

		return ofVec3f(x / f, y / f, z / f);
	}

	ofVec3f& operator/=(const float f) {
		if (f == 0) return *this;

		x /= f;
		y /= f;
		z /= f;
		return *this;
	}

	float length() const {
		return (float)sqrt(x*x + y*y + z*z);
	}

	float lengthSquared() const {
		return (float)(x*x + y*y + z*z);
	}

	float dot(const ofVec3f& vec) const {
		return x*vec.x + y*vec.y + z*vec.z;
	}

	ofVec3f crossed(const ofVec3f& vec) const {
		return getCrossed(vec);
	}
	ofVec3f getCrossed(const ofVec3f& vec) const {
		return ofVec3f(y*vec.z - z*vec.y,
			z*vec.x - x*vec.z,
			x*vec.y - y*vec.x);
	}

	ofVec3f& cross(const ofVec3f& vec) {
		float _x = y*vec.z - z*vec.y;
		float _y = z*vec.x - x*vec.z;
		z = x*vec.y - y*vec.x;
		x = _x;
		y = _y;
		return *this;
	}
};

enum ofPrimitiveMode {
	OF_PRIMITIVE_TRIANGLES,
	OF_PRIMITIVE_TRIANGLE_STRIP,
	OF_PRIMITIVE_TRIANGLE_FAN,
	OF_PRIMITIVE_LINES,
	OF_PRIMITIVE_LINE_STRIP,
	OF_PRIMITIVE_LINE_LOOP,
	OF_PRIMITIVE_POINTS,
#ifndef TARGET_OPENGLES
	OF_PRIMITIVE_LINES_ADJACENCY,
	OF_PRIMITIVE_LINE_STRIP_ADJACENCY,
	OF_PRIMITIVE_TRIANGLES_ADJACENCY,
	OF_PRIMITIVE_TRIANGLE_STRIP_ADJACENCY,
	OF_PRIMITIVE_PATCHES
#endif
};

struct ofPolyline {
	//std::deque<ofVec3f> curveVertices;
	std::vector<ofVec3f> points;
	bool bClosed;
	void addVertex(const ofVec3f& p) {
		//curveVertices.clear();
		points.push_back(p);
	}
	void clear() {
		setClosed(false);
		points.clear();
		//curveVertices.clear();
	}
	void setClosed(bool tf) {
		bClosed = tf;
	}
	//----------------------------------------------------------
	bool isClosed() const {
		return bClosed;
	}

	//----------------------------------------------------------
	void close() {
		setClosed(true);
	}
};

struct ofMesh
{
	std::vector<int> indices;
	std::vector<ofVec2f> texCoords;
	std::vector<ofVec3f> vertices;
	ofPrimitiveMode mode;
	mutable bool bFacesDirty;
	void setMode(ofPrimitiveMode m) {
		mode = m;
	}
	void addIndex(int i) {
		indices.push_back(i);
		bFacesDirty = true;
	}
	void addTexCoord(const ofVec2f& t) {
		//TODO: figure out if we add to all other arrays to match
		texCoords.push_back(t);
		bFacesDirty = true;
	}
	void addVertex(const ofVec3f& v) {
		vertices.push_back(v);
		bFacesDirty = true;
	}
};

struct ofRectangle{
	ofRectangle() { x = y = width = height = 0; }
	ofRectangle(float x, float y, float w, float h):x(x),y(y),width(w), height(h) {}

	/// \brief Set the x-position of the rectangle.
	/// \param px The new x-position.
	void setX(float px) { x = px; }

	/// \brief Set the y-position of the rectangle.
	/// \param py The new y-position.
	void setY(float py) { y = py; }

	/// \brief Set the width of the rectangle.
	/// \param w The new width.
	void setWidth(float w) { width = w; }

	/// \brief Set the height of the rectangle.
	/// \param h The new height.
	void setHeight(float h) { height = h; }
	/// \brief Set the size of the rectangle.
	/// \param w The new width.
	/// \param h The new height.
	void setSize(float w, float h) { width = w; height = h; }
	float x, y;
	float width;
	float height;
};

class OFX_EXPORTS IFaceTracker {
public:
	static IFaceTracker* New();
	static void Delete(IFaceTracker* inst);

	virtual bool setup(const char* path) = 0;
	virtual bool updateYUV(void* image, int width, int height, int step =0) = 0;
	virtual bool updateRGB(void* image, int width, int height, int step =0) = 0;

	virtual void reset() = 0;

	virtual int size() const = 0;
	virtual bool getFound() const = 0;
	virtual bool getHaarFound() const = 0;
	virtual bool getVisibility(int i) const = 0;

	virtual vector<ofVec2f> getImagePoints() const = 0;
	virtual vector<ofVec3f> getObjectPoints() const = 0;
	virtual vector<ofVec3f> getMeanObjectPoints() const = 0;

	virtual ofVec2f getImagePoint(int i) const = 0;
	virtual ofVec3f getObjectPoint(int i) const = 0;
	virtual ofVec3f getMeanObjectPoint(int i) const = 0;

	virtual ofRectangle getHaarRectangle() const = 0;
	virtual ofVec2f getPosition() const = 0; // pixels
	virtual float getScale() const = 0; // arbitrary units
	virtual ofVec3f getOrientation() const = 0; // radians
	//virtual ofMatrix4x4 getRotationMatrix() const = 0;

	enum Direction {
		FACING_FORWARD,
		FACING_LEFT, FACING_RIGHT,
		FACING_UNKNOWN
	};
	virtual Direction getDirection() const = 0;

	enum Feature {
		LEFT_EYE_TOP, RIGHT_EYE_TOP,
		LEFT_EYEBROW, RIGHT_EYEBROW,
		LEFT_EYE, RIGHT_EYE,
		LEFT_JAW, RIGHT_JAW, JAW,
		OUTER_MOUTH, INNER_MOUTH,
		NOSE_BRIDGE, NOSE_BASE,
		FACE_OUTLINE, ALL_FEATURES
	};
	virtual ofPolyline getImageFeature(Feature feature) const = 0;
	virtual ofPolyline getObjectFeature(Feature feature) const = 0;
	virtual ofPolyline getMeanObjectFeature(Feature feature) const = 0;
	virtual bool getFeatureRect(Feature feature, ofVec2f& center, ofRectangle& rect) const = 0;

	enum Gesture {
		MOUTH_WIDTH, MOUTH_HEIGHT,
		LEFT_EYEBROW_HEIGHT, RIGHT_EYEBROW_HEIGHT,
		LEFT_EYE_OPENNESS, RIGHT_EYE_OPENNESS,
		JAW_OPENNESS,
		NOSTRIL_FLARE
	};
	virtual float getGesture(Gesture gesture) const = 0;

	// setting interface
	virtual void setRescale(float rescale) = 0;
	virtual void setIterations(int iterations) = 0;
	virtual void setClamp(float clamp) = 0;
	virtual void setTolerance(float tolerance) = 0;
	virtual void setAttempts(int attempts) = 0;
	virtual void setUseInvisible(bool useInvisible) = 0;
	virtual void setHaarMinSize(float minSize) = 0;

};
