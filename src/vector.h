#pragma once

struct CVector2f {
    float x, y;
};

struct CVector3f {
    float x, y, z;
};

struct CVector4f {
    float x, y, z, w;
};

struct CMatrix4f {
    CVector4f m[4];

    CMatrix4f()
    {
        // matrix identity
        m[0].x = 1.0f;
        m[1].y = 1.0f;
        m[2].z = 1.0f;
        m[3].w = 1.0f;
    }
};
