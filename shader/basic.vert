#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 viewTransform;
uniform mat4 modelTransform;
uniform bool useTexture;

void main()
{
    gl_Position = viewTransform * modelTransform * vec4(aPos, 1.0);
    ourColor = aColor;
    if(useTexture)
    {
        TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
    }
    else
    {
        TexCoord = vec2(0.0, 0.0);
    }
}
