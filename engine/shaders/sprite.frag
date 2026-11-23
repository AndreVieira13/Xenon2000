#version 330 core
// ============================================================================
// sprite.frag (Fragment Shader)
// Executed once per screen pixel covered by a quad. Samples the bound sprite
// texture, discards transparent keyed pixels, and applies per-draw opacity.
// ============================================================================

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D spriteTexture;
uniform float alpha;

// Samples the texture at interpolated TexCoord, drops fully transparent magenta-keyed
// pixels, and scales the output alpha by the per-draw opacity multiplier.
void main() {
    vec4 color = texture(spriteTexture, TexCoord);
    if (color.a < 0.01) {
        discard;
    }
    FragColor = vec4(color.rgb, color.a * alpha);
}
