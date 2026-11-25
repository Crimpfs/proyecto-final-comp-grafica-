#ifndef MATEMATICAS_3D_H
#define MATEMATICAS_3D_H

// Operaciones matematicas autonomas para las matrices Model/View/Projection.
// Evitamos depender de GLM matematica: JLPERALTA40.rar incluye GLEW y GLFW,
// pero NO incluye <glm/glm.hpp>. Esto NO reemplaza glm.h (lector OBJ).
// Matrices 4x4 en column-major, listas para glUniformMatrix4fv(GL_FALSE).

#include <cmath>

namespace math3d {

struct vec2 {
    float x, y;
    vec2() : x(0), y(0) {}
    vec2(float xx, float yy) : x(xx), y(yy) {}
};

struct vec3 {
    float x, y, z;
    vec3() : x(0), y(0), z(0) {}
    vec3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
};

inline vec3 operator+(const vec3& a, const vec3& b) {
    return vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}
inline vec3 operator-(const vec3& a, const vec3& b) {
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}
inline vec3 operator*(const vec3& a, float s) {
    return vec3(a.x * s, a.y * s, a.z * s);
}
inline vec3 operator*(float s, const vec3& a) { return a * s; }
inline vec3& operator+=(vec3& a, const vec3& b) {
    a.x += b.x; a.y += b.y; a.z += b.z; return a;
}
inline vec3& operator-=(vec3& a, const vec3& b) {
    a.x -= b.x; a.y -= b.y; a.z -= b.z; return a;
}
inline float dot(const vec3& a, const vec3& b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}
inline vec3 cross(const vec3& a, const vec3& b) {
    return vec3(a.y*b.z - a.z*b.y,
                a.z*b.x - a.x*b.z,
                a.x*b.y - a.y*b.x);
}
inline vec3 normalize(const vec3& v) {
    const float l2 = dot(v, v);
    if (l2 <= 1e-16f) return vec3(0, 0, 0);
    const float inv = 1.0f / std::sqrt(l2);
    return v * inv;
}
inline float radians(float degrees) {
    return degrees * (3.14159265358979323846f / 180.0f);
}

struct mat4 {
    float m[16];
    explicit mat4(float diagonal = 1.0f) : m{0} {
        m[0] = m[5] = m[10] = m[15] = diagonal;
    }
    float& at(int row, int column) { return m[column*4 + row]; }
    float at(int row, int column) const { return m[column*4 + row]; }
};

inline mat4 operator*(const mat4& a, const mat4& b) {
    mat4 result(0.0f);
    for (int column = 0; column < 4; ++column)
        for (int row = 0; row < 4; ++row)
            for (int k = 0; k < 4; ++k)
                result.at(row, column) += a.at(row, k) * b.at(k, column);
    return result;
}

inline mat4 translate(const mat4& original, const vec3& v) {
    mat4 t(1.0f);
    t.at(0, 3) = v.x;
    t.at(1, 3) = v.y;
    t.at(2, 3) = v.z;
    return original * t;
}
inline mat4 scale(const mat4& original, const vec3& v) {
    mat4 s(1.0f);
    s.at(0, 0) = v.x;
    s.at(1, 1) = v.y;
    s.at(2, 2) = v.z;
    return original * s;
}
inline mat4 rotate(const mat4& original, float angle,
                   const vec3& axis) {
    // Rotacion alrededor de un eje arbitrario mediante Rodrigues.
    const vec3 a = normalize(axis);
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    const float t = 1.0f - c;
    mat4 r(1.0f);
    r.at(0,0)=t*a.x*a.x+c;
    r.at(0,1)=t*a.x*a.y-s*a.z;
    r.at(0,2)=t*a.x*a.z+s*a.y;
    r.at(1,0)=t*a.x*a.y+s*a.z;
    r.at(1,1)=t*a.y*a.y+c;
    r.at(1,2)=t*a.y*a.z-s*a.x;
    r.at(2,0)=t*a.x*a.z-s*a.y;
    r.at(2,1)=t*a.y*a.z+s*a.x;
    r.at(2,2)=t*a.z*a.z+c;
    return original * r;
}

inline mat4 lookAt(const vec3& eye, const vec3& target,
                   const vec3& up) {
    // Convencion OpenGL: camara mirando hacia -Z (right-handed).
    const vec3 forward = normalize(target - eye);
    const vec3 right = normalize(cross(forward, up));
    const vec3 cameraUp = cross(right, forward);
    mat4 m(1.0f);
    m.at(0,0)=right.x;  m.at(0,1)=right.y;  m.at(0,2)=right.z;
    m.at(1,0)=cameraUp.x; m.at(1,1)=cameraUp.y; m.at(1,2)=cameraUp.z;
    m.at(2,0)=-forward.x; m.at(2,1)=-forward.y; m.at(2,2)=-forward.z;
    m.at(0,3)=-dot(right,eye);
    m.at(1,3)=-dot(cameraUp,eye);
    m.at(2,3)= dot(forward,eye);
    return m;
}

inline mat4 perspective(float fovyRadians, float aspect,
                        float nearPlane, float farPlane) {
    mat4 m(0.0f);
    if (aspect <= 0.0f || nearPlane <= 0.0f ||
        farPlane <= nearPlane) return m;
    const float f = 1.0f / std::tan(fovyRadians * 0.5f);
    m.at(0,0)=f/aspect;
    m.at(1,1)=f;
    m.at(2,2)=(farPlane+nearPlane)/(nearPlane-farPlane);
    m.at(2,3)=(2.0f*farPlane*nearPlane)/(nearPlane-farPlane);
    m.at(3,2)=-1.0f;
    return m;
}

inline const float* value_ptr(const mat4& mat) { return mat.m; }
inline const float* value_ptr(const vec3& v) { return &v.x; }

} // namespace math3d
#endif
