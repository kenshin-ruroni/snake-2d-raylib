#version 430

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;

// Output fragment color
out vec4 finalColor;

// Input uniform values
uniform sampler2D screen_texture;

void main()
{
    ivec2 coords = ivec2(fragTexCoord);
    finalColor = texture(screen_texture, fragTexCoord);
}