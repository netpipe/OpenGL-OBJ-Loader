#ifndef SIMPLE_OBJ_MTL_LOADER_H
#define SIMPLE_OBJ_MTL_LOADER_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <cstdio>

using namespace std;

/* ================= Math ================= */

struct Vec2 {
    float x, y;
    Vec2() : x(0), y(0) {}
    Vec2(float X, float Y) : x(X), y(Y) {}
};

struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
};

/* ================= Material ================= */

struct OBJMaterial {
    std::string name;

    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;
    float shininess;

    OBJMaterial() {
        ambient   = Vec3(0.2f, 0.2f, 0.2f);
        diffuse   = Vec3(0.8f, 0.8f, 0.8f);
        specular  = Vec3(0.0f, 0.0f, 0.0f);
        shininess = 0.0f;
    }
};

/* ================= Vertex ================= */

struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec2 texcoord;
};

/* ================= Mesh ================= */

struct Mesh {
    std::vector<Vertex> vertices;
    OBJMaterial* material;
    Mesh() : material(0) {}
};

/* ================= OBJ Model ================= */

class OBJModel {
public:
    std::vector<Mesh> meshes;
    std::map<std::string, OBJMaterial> materials;

    bool load(const std::string& filename) {
        std::ifstream file(filename.c_str());
        if (!file) {
            std::cerr << "Failed to open OBJ: " << filename << "\n" << endl;
            return false;
        }

        std::vector<Vec3> positions;
        std::vector<Vec3> normals;
        std::vector<Vec2> texcoords;

        Mesh* currentMesh = 0;
        std::string line;

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string prefix;
            ss >> prefix;

            if (prefix == "mtllib") {
                std::string mtlFile;
                ss >> mtlFile;
                loadMTL(mtlFile);
                cout << "mtl loader" << endl;
            }
            else if (prefix == "usemtl") {
                std::string name;
                ss >> name;
                meshes.push_back(Mesh());
                currentMesh = &meshes.back();
                currentMesh->material = &materials[name];
            }
            else if (prefix == "v") {
                Vec3 v;
                ss >> v.x >> v.y >> v.z;
               // cout << v.x << v.y << v.z << endl;
                positions.push_back(v);
            }
            else if (prefix == "vn") {
                Vec3 n;
                ss >> n.x >> n.y >> n.z;
                normals.push_back(n);
            }
            else if (prefix == "vt") {
                Vec2 t;
                ss >> t.x >> t.y;
                texcoords.push_back(t);
            }
            else if (prefix == "f" && currentMesh) {
                std::string v[4];
                int count = 0;
                while (ss >> v[count]) count++;

                for (int i = 1; i + 1 < count; ++i) {
                    parseVertex(v[0], positions, texcoords, normals, *currentMesh);
                    parseVertex(v[i], positions, texcoords, normals, *currentMesh);
                    parseVertex(v[i + 1], positions, texcoords, normals, *currentMesh);
                }
            }
        }
        cout << "loaded" << endl;
        return true;
    }

private:
    void loadMTL(const std::string& filename) {
        std::ifstream file(filename.c_str());
        if (!file) {
            std::cerr << "Failed to open MTL: " << filename << "\n" << endl;
            return;
        }

        OBJMaterial* current = 0;
        std::string line;

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string prefix;
            ss >> prefix;

            if (prefix == "newmtl") {
                std::string name;
                ss >> name;
                materials[name] = OBJMaterial();
                materials[name].name = name;
                current = &materials[name];
            }
            else if (prefix == "Ka" && current) {
                ss >> current->ambient.x >> current->ambient.y >> current->ambient.z;
            }
            else if (prefix == "Kd" && current) {
                ss >> current->diffuse.x >> current->diffuse.y >> current->diffuse.z;
            }
            else if (prefix == "Ks" && current) {
                ss >> current->specular.x >> current->specular.y >> current->specular.z;
            }
            else if (prefix == "Ns" && current) {
                ss >> current->shininess;
            }
        }
        cout << "mtl loaded" << endl;
    }

    void parseVertex(const std::string& token,
                     const std::vector<Vec3>& positions,
                     const std::vector<Vec2>& texcoords,
                     const std::vector<Vec3>& normals,
                     Mesh& mesh)
    {
        Vertex vert;
        int vi = 0, ti = 0, ni = 0;

        std::sscanf(token.c_str(), "%d/%d/%d", &vi, &ti, &ni);

        vert.position = positions[vi - 1];
        vert.texcoord = (ti > 0) ? texcoords[ti - 1] : Vec2();
        vert.normal   = (ni > 0) ? normals[ni - 1]   : Vec3(0, 0, 1);

        mesh.vertices.push_back(vert);
    }
};

#endif
