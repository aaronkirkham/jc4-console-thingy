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
};
