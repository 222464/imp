R"(#version 330
in vec3 vertexPosition;

in vec3 instancePosition;
in vec3 instancePositionNext;

uniform mat4 mvp;

uniform float thickness;

void main() {
    vec3 diff = instancePositionNext - instancePosition;

    vec3 up;

    if (diff == vec3(0.0, 1.0, 0.0))
        up = vec3(0.0, 0.0, 1.0);
    else
        up = vec3(0.0f, 1.0, 0.0);

    // Look-at matrix
    mat4 lookTransform;

    vec3 F = normalize(diff);
    vec3 S = normalize(cross(F, up));
    vec3 U = cross(S, F);

    F *= thickness;
    S *= thickness;
    U *= thickness;

    lookTransform[0][0] = S.x;
    lookTransform[0][1] = S.y;
    lookTransform[0][2] = S.z;
    lookTransform[0][3] = 0.0f;

    lookTransform[1][0] = F.x;
    lookTransform[1][1] = F.y;
    lookTransform[1][2] = F.z;
    lookTransform[1][3] = 0.0f;

    lookTransform[2][0] = U.x;
    lookTransform[2][1] = U.y;
    lookTransform[2][2] = U.z;
    lookTransform[2][3] = 0.0f;

    lookTransform[3][0] = instancePosition.x;
    lookTransform[3][1] = instancePosition.y;
    lookTransform[3][2] = instancePosition.z;
    lookTransform[3][3] = 1.0f;

    mat4 mvpi = mvp * lookTransform;

    gl_Position = mvpi * vec4(vertexPosition, 1.0);
})"
