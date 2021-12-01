#version 410
out vec4 FragColor;
in vec3 TexCoords;
uniform samplerCube skybox;
void main()
{
    FragColor = texture(skybox, TexCoords);
//    instanceShader.use();
//    for(unsigned int i=0; i< rock.meshes.size();i++){
//        glBindVertexArray(rock.meshed[i].VAO);
//        glDrawElementsInstanced();
//    }
}

