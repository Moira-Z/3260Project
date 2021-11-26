#version 430

out vec4 FragColor;

in vec2 TexCoords;
in vec3 normalWorld;
in vec3 vertexPositionWorld;

struct attenuation
{
    float constant;
    float linear;
    float quadratic;  
};

uniform vec3 dirDirection;
uniform vec3 dirColor;
uniform float intensity;
uniform attenuation point;
uniform vec3 pointPosition;
uniform vec3 pointColor;
uniform vec3 eyePositionWorld;
uniform sampler2D texture_diffuse1;

vec3 CalcDirLight(vec3 normal, vec3 viewDir); 
vec3 CalcPointLight(vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    vec3 viewDir = normalize(eyePositionWorld - vertexPositionWorld);
    
    vec3 res = vec3(0.0);

    res += CalcPointLight(normalWorld, vertexPositionWorld, viewDir);

    FragColor = vec4(res, 1.0);
}

vec3 CalcDirLight(vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-dirDirection);

    float diff = max(dot(normal, lightDir), 0.0);
  
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 50);

    vec3 ambient  = intensity * dirColor * vec3(texture(texture_diffuse1, TexCoords));;
    vec3 diffuse  = intensity * dirColor * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = intensity * dirColor * spec * vec3(0.3, 0.3, 0.3);
    
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(pointPosition - fragPos);
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 50);
    
    // attenuation
    float distance = length(pointPosition - fragPos);
    float attenuation = 1.0 / (point.constant + point.linear * distance + 
  			     point.quadratic * (distance * distance));    
    
    // combine results
    vec3 ambient  = pointColor * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse  = pointColor * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = pointColor * spec * vec3(0.3, 0.3, 0.3);
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
} 