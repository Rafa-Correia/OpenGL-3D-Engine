#ifndef MAT4_HPP
#define MAT4_HPP

#include "math/vec3.hpp"
// #include "math/vec4.hpp"

#include "helper/printer.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class mat4
{
public:
    /**
     * Empty constructor.
     *
     * @returns Identity matrix representation.
     */
    mat4()
    {
        for (int i = 0; i < 16; i++)
        {
            m_data[i] = 0;
        }

        m_data[0] = 1;
        m_data[5] = 1;
        m_data[10] = 1;
        m_data[15] = 1;
    }

    /**
     * Creates matrix from float array content.
     */
    mat4(std::vector<float> content)
    {
        if (content.size() != 16)
        {
            std::stringstream ss;

            ss << "Trying to build matrix with " << content.size() << " element(s). Matrix can only be built with 16 elements, returning identity...";
            printer::print_warning(ss.str(), "matrix4x4 constructor");

            for (int i = 0; i < 16; i++)
            {
                this->m_data[i] = 0;
            }

            this->m_data[0] = 1;
            this->m_data[5] = 1;
            this->m_data[10] = 1;
            this->m_data[15] = 1;

            return;
        }

        for (size_t i = 0; i < 16; i++)
        {
            this->m_data[i] = content.at(i);
        }
    }

    /**
     * Creates Identity 4 by 4 matrix.
     *
     * @returns Identity matrix representation.
     */
    static mat4 Identity() { return mat4(); }

    /**
     * Creates transformation matrix for a translation.
     *
     * @param translation_vector Vector to translate by.
     *
     * @returns Representation of 4 by 4 translation matrix.
     */
    static mat4 Translate(vec3<float> translation_vector)
    {
        mat4 result;

        result.m_data[12] = translation_vector.x;
        result.m_data[13] = translation_vector.y;
        result.m_data[14] = translation_vector.z;

        return result;
    }

    /**
     * Creates transformation matrix for a rotation.
     *
     * @param theta Angle to rotate around the rotation vector. In radians.
     * @param rotation_vector Vector to rotate around.
     *
     * @returns Representation of 4 by 4 rotation matrix.
     */
    static mat4 Rotate(float theta, vec3<float> rotation_vector)
    {
        mat4 result;
        float x = rotation_vector.x, y = rotation_vector.y, z = rotation_vector.z;
        float c = cosf(theta);
        float s = sinf(theta);
        float oneMinusC = 1.0f - c;

        // Normalize axis
        float len = sqrtf(x * x + y * y + z * z);
        if (len == 0.0f)
            return result; // Return identity if invalid axis
        x /= len;
        y /= len;
        z /= len;

        result.m_data[0] = c + x * x * oneMinusC;
        result.m_data[1] = y * x * oneMinusC + z * s;
        result.m_data[2] = z * x * oneMinusC - y * s;
        result.m_data[3] = 0.0f;

        result.m_data[4] = x * y * oneMinusC - z * s;
        result.m_data[5] = c + y * y * oneMinusC;
        result.m_data[6] = z * y * oneMinusC + x * s;
        result.m_data[7] = 0.0f;

        result.m_data[8] = x * z * oneMinusC + y * s;
        result.m_data[9] = y * z * oneMinusC - x * s;
        result.m_data[10] = c + z * z * oneMinusC;
        result.m_data[11] = 0.0f;

        result.m_data[12] = result.m_data[13] = result.m_data[14] = 0.0f;
        result.m_data[15] = 1.0f;

        return result;
    }

    /**
     * Catmul rom stuff
     */
    static mat4 Rotate(vec3<float> x, vec3<float> y, vec3<float> z)
    {
        mat4 result;

        result.m_data[0] = x.x;
        result.m_data[1] = x.y;
        result.m_data[2] = x.z;
        result.m_data[3] = 0;

        result.m_data[4] = y.x;
        result.m_data[5] = y.y;
        result.m_data[6] = y.z;
        result.m_data[7] = 0;

        result.m_data[8] = z.x;
        result.m_data[9] = z.y;
        result.m_data[10] = z.z;
        result.m_data[11] = 0;

        result.m_data[12] = 0;
        result.m_data[13] = 0;
        result.m_data[14] = 0;
        result.m_data[15] = 1;

        return result;
    }

    /**
     * Creates transformation matrix for scaling.
     *
     * @param scale_vector Vector containing scaling information.
     *
     * @returns Representation of 4 by 4 scaling matrix.
     */
    static mat4 Scale(vec3<float> scale_vector)
    {
        mat4 result;

        result.m_data[0] = scale_vector.x;
        result.m_data[5] = scale_vector.y;
        result.m_data[10] = scale_vector.z;

        return result;
    }

    /**
     * Creates view matrix from camera parameters.
     *
     * @param eye Camera position (eye).
     * @param center Camera target (center).
     * @param up Camera's up vector.
     *
     * @returns Representation of 4 by 4 View matrix.
     */
    static mat4 View(vec3<float> eye, vec3<float> center, vec3<float> up)
    {
        vec3<float> F = center - eye;
        F.normalize();

        vec3<float> S = vec3<float>::cross(F, up);
        S.normalize();

        vec3<float> U = vec3<float>::cross(S, F);
        U.normalize();

        mat4 result;

        // row 1
        result.m_data[0] = S.x;
        result.m_data[4] = S.y;
        result.m_data[8] = S.z;
        result.m_data[12] = 0.0f;

        // row 2
        result.m_data[1] = U.x;
        result.m_data[5] = U.y;
        result.m_data[9] = U.z;
        result.m_data[13] = 0.0f;

        // row 3
        result.m_data[2] = -(F.x);
        result.m_data[6] = -(F.y);
        result.m_data[10] = -(F.z);
        result.m_data[14] = 0.0f;

        // row 4
        result.m_data[3] = 0.0f;
        result.m_data[7] = 0.0f;
        result.m_data[11] = 0.0f;
        result.m_data[15] = 1.0f;

        result = result * mat4::Translate(eye * -1.0f);

        return result;
    }

    /**
     * Creates projection matrix from projection attributes.
     *
     * @param fov Field of view.
     * @param aspect_ratio Aspect ratio of viewport.
     * @param near_plane Near plane distance.
     * @param far_plane Far plane distance.
     */
    static mat4 Projection(float fov, float aspect_ratio, float near_plane, float far_plane)
    {
        float fov_rad = fov * ((float)M_PI / 180.0f);
        float f = 1 / tanf(fov_rad / 2.0f);

        mat4 result;
        // row - col (starting at 0)
        result.m_data[0] = f / aspect_ratio; // 00

        result.m_data[5] = f; // 11

        result.m_data[10] = (far_plane + near_plane) / (near_plane - far_plane); // 22
        result.m_data[11] = -1;                                                  // 32

        result.m_data[14] = (2 * far_plane * near_plane) / (near_plane - far_plane); // 23
        result.m_data[15] = 0;                                                       // 33

        return result;
    }

    /**
     * Creates matrix for catmul rom curve calcs.
     */
    // static mat4 Catmul_rom();

    /**
     * Creates matrix for bezier curves
     */
    // static mat4 Bezier();

    /**
     * Getter for matrix data in specific position.
     *
     * @param row Row of intended value.
     * @param column Column of intented value.
     *
     * @returns Value at row and column.
     */
    float get_data_at_point(int row, int column) { return m_data[row * 4 + column]; }

    mat4 operator*(const mat4 &other) const
    {
        mat4 result;
        int row, col;

        for (row = 0; row < 4; row++)
        {
            for (col = 0; col < 4; col++)
            {
                result.m_data[col * 4 + row] =
                    m_data[0 * 4 + row] * other.m_data[col * 4 + 0] +
                    m_data[1 * 4 + row] * other.m_data[col * 4 + 1] +
                    m_data[2 * 4 + row] * other.m_data[col * 4 + 2] +
                    m_data[3 * 4 + row] * other.m_data[col * 4 + 3];
            }
        }

        return result;
    }

    vec3<float> operator*(const vec3<float> &vec) const // i think this is unused, anyway, this is the same as multiplying by a vec4 with w = 1
    {
        float new_x, new_y, new_z;
        new_x = m_data[0] * vec.x + m_data[4] * vec.y + m_data[8] * vec.z + m_data[12];
        new_y = m_data[1] * vec.x + m_data[5] * vec.y + m_data[9] * vec.z + m_data[13];
        new_z = m_data[2] * vec.x + m_data[6] * vec.y + m_data[10] * vec.z + m_data[14];

        return vec3<float>(new_x, new_y, new_z);
    }

    // vector4 operator*(const vector4 &vec) const;

    operator const float *() const { return m_data; }

private:
    float m_data[16]; // < -- Inner structure storing matrix data.
};

#endif