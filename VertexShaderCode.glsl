#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 normalWorld;
out vec3 vertexPositionWorld;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;  
    
    vec4 v = vec4(aPos, 1.0);
	vec4 newPosition = model * v;
    gl_Position = projection * view * newPosition;

    vec4 normal_temp = model * vec4(aNormal, 0);
	normalWorld = normal_temp.xyz;

    vertexPositionWorld = newPosition.xyz;
}