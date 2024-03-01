#ifndef MICROCOMPUTE_VEC_H_INCLUDE_GUARD
#define MICROCOMPUTE_VEC_H_INCLUDE_GUARD

typedef unsigned int mc_uint;

typedef union {
    struct {
        float x;
        float y;
    };
    struct {
        float r;
        float g;
    };
    struct {
        float _0;
        float _1;
    };
} mc_vec2;

typedef union {
    struct {
        float x;
        float y;
        float z;
    };
    struct {
        float r;
        float g;
        float b;
    };
    struct {
        float _0;
        float _1;
        float _2;
        float _padding;
    };
} mc_vec3;

typedef union {
    struct {
        float x;
        float y;
        float z;
        float w;
    };
    struct {
        float r;
        float g;
        float b;
        float a;
    };
    struct {
        float _0;
        float _1;
        float _2;
        float _3;
    };
} mc_vec4;

typedef union {
    struct {
        int x;
        int y;
    };
    struct {
        int r;
        int g;
    };
    struct {
        int _0;
        int _1;
    };
} mc_ivec2;

typedef union {
    struct {
        int x;
        int y;
        int z;
    };
    struct {
        int r;
        int g;
        int b;
    };
    struct {
        int _0;
        int _1;
        int _2;
        int _padding;
    };
} mc_ivec3;

typedef union {
    struct {
        int x;
        int y;
        int z;
        int w;
    };
    struct {
        int r;
        int g;
        int b;
        int a;
    };
    struct {
        int _0;
        int _1;
        int _2;
        int _3;
    };
} mc_ivec4;

typedef union {
    struct {
        mc_uint x;
        mc_uint y;
    };
    struct {
        mc_uint r;
        mc_uint g;
    };
    struct {
        mc_uint _0;
        mc_uint _1;
    };
} mc_uvec2;

typedef union {
    struct {
        mc_uint x;
        mc_uint y;
        mc_uint z;
    };
    struct {
        mc_uint r;
        mc_uint g;
        mc_uint b;
    };
    struct {
        mc_uint _0;
        mc_uint _1;
        mc_uint _2;
        mc_uint _padding;
    };
} mc_uvec3;

typedef union {
    struct {
        mc_uint x;
        mc_uint y;
        mc_uint z;
        mc_uint w;
    };
    struct {
        mc_uint r;
        mc_uint g;
        mc_uint b;
        mc_uint a;
    };
    struct {
        mc_uint _0;
        mc_uint _1;
        mc_uint _2;
        mc_uint _3;
    };
} mc_uvec4;

#ifdef MC_VEC_TYPES_SHORT_NAMES
#define uvec mc_uvec
#define vec2 mc_vec2;
#define vec3 mc_vec3;
#define vec4 mc_vec4;
#define ivec2 mc_ivec2;
#define ivec3 mc_ivec3;
#define ivec4 mc_ivec4;
#define uvec2 mc_uvec2;
#define uvec3 mc_uvec3;
#define uvec4 mc_uvec4;
#endif

#endif

