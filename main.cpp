#include <GLUT/glut.h>
#include "simple_obj_mtl_loader.h"

using namespace std;

/* Global model */
OBJModel model;
float angle = 0.0f;

/* Apply OBJ material to OpenGL fixed pipeline */
void applyMaterial(const OBJMaterial* m) {
    if (!m) return;

    float Ka[4] = { m->ambient.x,  m->ambient.y,  m->ambient.z,  1.0f };
    float Kd[4] = { m->diffuse.x,  m->diffuse.y,  m->diffuse.z,  1.0f };
    float Ks[4] = { m->specular.x, m->specular.y, m->specular.z, 1.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  Ka);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  Kd);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Ks);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, m->shininess);
}

void drawText(float x, float y, const char *s) {
    glRasterPos2f(x, y);
    while (*s) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *s++);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, -0.8f, 1.5f);
    glRotatef(angle, 0.0f, 1.0f, 0.0f);

        
    gluLookAt(   	 0.0, -0.8, 1.0,   // camera position (above & back)
   	 -1.0, 1.0, -1.0,   // look at center of model
   	 0.0, 1.0, 0.0 );   // up vector


    for (size_t i = 0; i < model.meshes.size(); ++i) {
        const Mesh& mesh = model.meshes[i];
        applyMaterial(mesh.material);
		
        glBegin(GL_TRIANGLES);
        for (size_t v = 0; v < mesh.vertices.size(); ++v) {
            const Vertex& vert = mesh.vertices[v];
            glNormal3f(vert.normal.x, vert.normal.y, vert.normal.z);
            glVertex3f(vert.position.x, vert.position.y, vert.position.z);
        }
        glEnd();
    }
	//drawText(20,20,"testing");
    glutSwapBuffers();
}

void idle() {
	//drawText(20,20,"testing");
	angle += 0.1f;
    glutPostRedisplay();
    
}

void initGL() {
glEnable(GL_DEPTH_TEST);

glEnable(GL_LIGHTING);
glEnable(GL_LIGHT0);

glDisable(GL_COLOR_MATERIAL);

float lightPos[] = { 2, 5, 5, 1 };
float white[]    = { 1, 1, 1, 1 };

glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
glLightfv(GL_LIGHT0, GL_DIFFUSE,  white);
glLightfv(GL_LIGHT0, GL_SPECULAR, white);

float ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

}

int main(int argc, char** argv) {
	cout << "testing" << endl;
	
    if (!model.load("cake.obj")) {
        return 1;
    }
fprintf(stderr, "Materials loaded: %zu\n", model.materials.size());

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OBJ + MTL Loader Demo");

    initGL();

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
