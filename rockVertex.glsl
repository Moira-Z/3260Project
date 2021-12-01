#version 410
layout(location=0) in vec3 aPos;
layout(location=2) in vec3 aTexCoords;
layout(location=3) in vec3 instanceMatrix;
out vec3 TexCoords;
uniform mat4 projection;
uniform mat4 view;
void main()
{
    gl_Position = projection * view instanceMatrix * vec4(aPos, 1.0);
    TexCoords = aTexCoords;
}
