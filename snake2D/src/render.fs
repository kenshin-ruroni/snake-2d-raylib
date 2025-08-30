#version 430

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;

// Output fragment color
out vec4 finalColor;

// Input uniform values
uniform sampler2D screen_texture;

void main()
{
    ivec2 textureSize2d = textureSize(screen_texture,0);
    ivec2 coords = ivec2(fragTexCoord.x,textureSize2d.y -fragTexCoord.y );

    finalColor = texture(screen_texture, fragTexCoord);
}