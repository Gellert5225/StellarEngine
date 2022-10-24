#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct GlobalUniforms
{
    float4x4 viewProjection;
};

struct Push
{
    float4x4 model;
    float3 color;
};

struct main0_out
{
    float4 viewProjection_0 [[user(locn0)]];
    float4 viewProjection_1 [[user(locn1)]];
    float4 viewProjection_2 [[user(locn2)]];
    float4 viewProjection_3 [[user(locn3)]];
    float3 nearPoint [[user(locn4)]];
    float3 farPoint [[user(locn5)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float2 inPosition [[attribute(0)]];
};

// Returns the determinant of a 2x2 matrix.
static inline __attribute__((always_inline))
float spvDet2x2(float a1, float a2, float b1, float b2)
{
    return a1 * b2 - b1 * a2;
}

// Returns the determinant of a 3x3 matrix.
static inline __attribute__((always_inline))
float spvDet3x3(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3)
{
    return a1 * spvDet2x2(b2, b3, c2, c3) - b1 * spvDet2x2(a2, a3, c2, c3) + c1 * spvDet2x2(a2, a3, b2, b3);
}

// Returns the inverse of a matrix, by using the algorithm of calculating the classical
// adjoint and dividing by the determinant. The contents of the matrix are changed.
static inline __attribute__((always_inline))
float4x4 spvInverse4x4(float4x4 m)
{
    float4x4 adj;	// The adjoint matrix (inverse after dividing by determinant)

    // Create the transpose of the cofactors, as the classical adjoint of the matrix.
    adj[0][0] =  spvDet3x3(m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
    adj[0][1] = -spvDet3x3(m[0][1], m[0][2], m[0][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
    adj[0][2] =  spvDet3x3(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[3][1], m[3][2], m[3][3]);
    adj[0][3] = -spvDet3x3(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3]);

    adj[1][0] = -spvDet3x3(m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
    adj[1][1] =  spvDet3x3(m[0][0], m[0][2], m[0][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
    adj[1][2] = -spvDet3x3(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[3][0], m[3][2], m[3][3]);
    adj[1][3] =  spvDet3x3(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3]);

    adj[2][0] =  spvDet3x3(m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
    adj[2][1] = -spvDet3x3(m[0][0], m[0][1], m[0][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
    adj[2][2] =  spvDet3x3(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[3][0], m[3][1], m[3][3]);
    adj[2][3] = -spvDet3x3(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3]);

    adj[3][0] = -spvDet3x3(m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);
    adj[3][1] =  spvDet3x3(m[0][0], m[0][1], m[0][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);
    adj[3][2] = -spvDet3x3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[3][0], m[3][1], m[3][2]);
    adj[3][3] =  spvDet3x3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);

    // Calculate the determinant as a combination of the cofactors of the first row.
    float det = (adj[0][0] * m[0][0]) + (adj[0][1] * m[1][0]) + (adj[0][2] * m[2][0]) + (adj[0][3] * m[3][0]);

    // Divide the classical adjoint matrix by the determinant.
    // If determinant is zero, matrix is not invertable, so leave it unchanged.
    return (det != 0.0f) ? (adj * (1.0f / det)) : m;
}

static inline __attribute__((always_inline))
float3 UnprojectPoint(thread const float& x, thread const float& y, thread const float& z, thread const float4x4& viewProjection)
{
    float4 unprojectedPoint = spvInverse4x4(viewProjection) * float4(x, y, z, 1.0);
    return unprojectedPoint.xyz / float3(unprojectedPoint.w);
}

vertex main0_out main0(main0_in in [[stage_in]], constant GlobalUniforms& ubo [[buffer(0)]])
{
    main0_out out = {};
    float4x4 viewProjection = {};
    float3 p = float3(in.inPosition, 0.0);
    float param = p.x;
    float param_1 = p.y;
    float param_2 = 0.0;
    float4x4 param_3 = ubo.viewProjection;
    out.nearPoint = UnprojectPoint(param, param_1, param_2, param_3);
    float param_4 = p.x;
    float param_5 = p.y;
    float param_6 = 1.0;
    float4x4 param_7 = ubo.viewProjection;
    out.farPoint = UnprojectPoint(param_4, param_5, param_6, param_7);
    out.gl_Position = float4(p, 1.0);
    viewProjection = ubo.viewProjection;
    out.viewProjection_0 = viewProjection[0];
    out.viewProjection_1 = viewProjection[1];
    out.viewProjection_2 = viewProjection[2];
    out.viewProjection_3 = viewProjection[3];
    return out;
}

struct main1_out
{
    float4 outColor [[color(0)]];
    float gl_FragDepth [[depth(any)]];
};

struct main1_in
{
    float4 viewProjection_0 [[user(locn0)]];
    float4 viewProjection_1 [[user(locn1)]];
    float4 viewProjection_2 [[user(locn2)]];
    float4 viewProjection_3 [[user(locn3)]];
    float3 nearPoint [[user(locn4)]];
    float3 farPoint [[user(locn5)]];
};

static inline __attribute__((always_inline))
float computeDepth(thread const float3& pos, thread float4x4& viewProjection)
{
    float4 clip_space_pos = viewProjection * float4(pos, 1.0);
    return clip_space_pos.z / clip_space_pos.w;
}

static inline __attribute__((always_inline))
float computeLinearDepth(thread const float3& pos, thread float4x4& viewProjection)
{
    float4 clip_space_pos = viewProjection * float4(pos, 1.0);
    float clip_space_depth = ((clip_space_pos.z / clip_space_pos.w) * 2.0) - 1.0;
    float linearDepth = 50.0 / (100.09999847412109375 - (clip_space_depth * 99.90000152587890625));
    return linearDepth / 100.0;
}

static inline __attribute__((always_inline))
float scaleFactor(thread const float& scale)
{
    return (0.769999980926513671875 * scale) + 2.2999999523162841796875;
}

static inline __attribute__((always_inline))
float4 grid(thread const float3& fragPos3D, thread const float& scale, thread const bool& drawAxis)
{
    float2 coord = fragPos3D.xz * scale;
    float2 derivative = fwidth(coord);
    float2 grid_1 = abs(fract(coord - float2(0.5)) - float2(0.5)) / derivative;
    float line = fast::min(grid_1.x, grid_1.y);
    float minimumz = fast::min(derivative.y, 1.0);
    float minimumx = fast::min(derivative.x, 1.0);
    float param = scale;
    float4 color = float4(0.3499999940395355224609375, 0.4000000059604644775390625, 0.4000000059604644775390625, (1.0 - fast::min(line, 1.0)) / scaleFactor(param));
    bool _93 = fragPos3D.x > ((-0.100000001490116119384765625) * minimumx);
    bool _102;
    if (_93)
    {
        _102 = fragPos3D.x < (0.100000001490116119384765625 * minimumx);
    }
    else
    {
        _102 = _93;
    }
    if (_102)
    {
        color = float4(0.0, 0.0, 1.0, 1.0);
    }
    bool _112 = fragPos3D.z > ((-0.100000001490116119384765625) * minimumz);
    bool _120;
    if (_112)
    {
        _120 = fragPos3D.z < (0.100000001490116119384765625 * minimumz);
    }
    else
    {
        _120 = _112;
    }
    if (_120)
    {
        color = float4(1.0, 0.0, 0.0, 1.0);
    }
    return color;
}

fragment main1_out main1(main1_in in [[stage_in]])
{
    main1_out out = {};
    float4x4 viewProjection = {};
    viewProjection[0] = in.viewProjection_0;
    viewProjection[1] = in.viewProjection_1;
    viewProjection[2] = in.viewProjection_2;
    viewProjection[3] = in.viewProjection_3;
    float t = (-in.nearPoint.y) / (in.farPoint.y - in.nearPoint.y);
    float3 fragPos3D = in.nearPoint + ((in.farPoint - in.nearPoint) * t);
    float3 param = fragPos3D;
    out.gl_FragDepth = computeDepth(param, viewProjection);
    float3 param_1 = fragPos3D;
    float linearDepth = computeLinearDepth(param_1, viewProjection);
    float fading = fast::max(0.0, 0.5 - linearDepth);
    float3 param_2 = fragPos3D;
    float param_3 = 1.0;
    bool param_4 = true;
    float3 param_5 = fragPos3D;
    float param_6 = 10.0;
    bool param_7 = true;
    out.outColor = (grid(param_2, param_3, param_4) + grid(param_5, param_6, param_7)) * float(t > 0.0);
    out.outColor.w *= fading;
    if (out.outColor.w == 0.0)
    {
        discard_fragment();
    }
    return out;
}

