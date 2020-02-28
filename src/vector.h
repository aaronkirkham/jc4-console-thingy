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
        m[0] = {1, 0, 0, 0};
        m[1] = {0, 1, 0, 0};
        m[2] = {0, 0, 1, 0};
        m[3] = {0, 0, 0, 1};
    }
};
