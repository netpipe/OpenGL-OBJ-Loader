
#include <GLUT/glut.h>
#include <cstdio>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "simple_obj_mtl_loader.h"

OBJModel model;
float angle = 0.0f;

GLuint loadTexture(const std::string& file) {
    int w,h,c;
    unsigned char* data = stbi_load(file.c_str(), &w, &h, &c, 0);
    if (!data) return 0;

    GLenum fmt = (c == 4) ? GL_RGBA : GL_RGB;
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    return tex;
}

void applyMaterial(const OBJMaterial* m) {
    float Ka[] = { m->ambient.x, m->ambient.y, m->ambient.z, 1 };
    float Kd[] = { m->diffuse.x, m->diffuse.y, m->diffuse.z, 1 };
    float Ks[] = { m->specular.x,m->specular.y,m->specular.z,1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, Ka);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Kd);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Ks);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m->shininess);

    if (m->textureID) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m->textureID);
    } else {
        glDisable(GL_TEXTURE_2D);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0,-0.8,1, 0,0,0, 0,1,0);

    for (size_t i=0;i<model.meshes.size();++i) {
        Mesh& mesh = model.meshes[i];
        applyMaterial(mesh.material);
        glBegin(GL_TRIANGLES);
        for (size_t v=0; v<mesh.vertices.size(); ++v) {
            Vertex& vert = mesh.vertices[v];
            glTexCoord2f(vert.texcoord.x, vert.texcoord.y);
            glNormal3f(vert.normal.x, vert.normal.y, vert.normal.z);
            glVertex3f(vert.position.x, vert.position.y, vert.position.z);
        }
        glEnd();
    }
    glutSwapBuffers();
}

void initGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    float pos[]={2,5,5,1};
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
}

int main(int argc,char**argv) {
    if (!model.load("capsule.obj")) return 1;

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(800,600);
    glutCreateWindow("OBJ+MTL+Texture Demo");

    initGL();

    for (std::map<std::string,OBJMaterial>::iterator it=model.materials.begin();
         it!=model.materials.end(); ++it)
        if (!it->second.diffuseMap.empty())
            it->second.textureID = loadTexture(it->second.diffuseMap);

    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
