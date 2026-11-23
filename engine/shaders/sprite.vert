#version 330 core
// ============================================================================
// sprite.vert (Vertex Shader)
// Executed once for each of the 4 quad corners. Transforms local unit-quad
// coordinates into OpenGL clip space and slices UVs for sprite sheets/scrolling.
// ============================================================================

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 model;
uniform vec4 uvRect; // (uOffset, vOffset, uScale, vScale)

// Projects local vertex aPos into NDC via projection * model and remaps [0, 1] UVs
// into the requested sprite-sheet frame or scrolling background window.
void main() {
    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
    TexCoord = uvRect.xy + aTexCoord * uvRect.zw;
}
