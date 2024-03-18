// "GPU Random Numbers via the Tiny Encryption Algorithm"
// https://dl.acm.org/doi/10.5555/1921479.1921500
uint tea(uint val0, uint val1)
{
    uint v0 = val0;
    uint v1 = val1;
    uint s0 = 0;

    for (uint n = 0; n < 16; n++)
    {
        s0 += 0x9e3779b9;
        v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + s0) ^ ((v1 >> 5) + 0xc8013ea4);
        v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + s0) ^ ((v0 >> 5) + 0x7e95761e);
    }

    return v0;
}

// Numerical Recipes linear congruential generator
uint lcg(inout uint prev)
{
    uint LCG_A = 1664525u;
    uint LCG_C = 1013904223u;
    prev = (LCG_A * prev + LCG_C);
    return prev & 0x00FFFFFF;
}

// Random float given previous state
float rnd(inout uint prev)
{
    return (float(lcg(prev)) / float(0x01000000));
}

vec3 samplingHemisphere(inout uint seed, in vec3 x, in vec3 y, in vec3 z)
{
    float r1 = rnd(seed);
    float r2 = rnd(seed);
    float sq = sqrt(r1);

    vec3 direction = vec3(cos(2 * PI * r2) * sq, sin(2 * PI * r2) * sq, sqrt(1. - r1));
    direction = direction.x * x + direction.y * y + direction.z * z;

    return direction;
}

// Return the tangent and binormal from the incoming normal
void createCoordinateSystem(in vec3 N, out vec3 Nt, out vec3 Nb)
{
    if (abs(N.x) > abs(N.y))
        Nt = vec3(N.z, 0, -N.x) / sqrt(N.x * N.x + N.z * N.z);
    else
        Nt = vec3(0, -N.z, N.y) / sqrt(N.y * N.y + N.z * N.z);
    Nb = cross(N, Nt);
}