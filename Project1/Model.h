#pragma once
#include <string>
#include <vector>
#include <GLM/glm.hpp>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
using namespace std;
class Model {
	private:
		string fileName;
		std::vector < glm::vec3 > outVertices;
		std::vector < glm::vec2 > outUvs;
		std::vector < glm::vec3 > outNormals;
		GLuint*	texture;
	public:
		Model(string modelFile) : fileName(modelFile) 
		{
			readObj(fileName);
		}
		std::vector< glm::vec3 > GetOutVertices()
		{
			return outVertices;
		}
		std::vector< glm::vec2 > GetOutUvs()
		{
			return outUvs;
		}
		std::vector< glm::vec3 > GetOutNormals()
		{
			return outNormals;
		}
		GLuint* GetTexture()
		{
			return texture;
		}
		void readObj(string name) {
			cout << "Loading model " << name << "\n";

			std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
			std::vector< string > materials, textures;
			std::vector< glm::vec3 > objVertices;
			std::vector< glm::vec2 > objUvs;
			std::vector< glm::vec3 > objNormals;

			std::ifstream dataFile(name);

			string rawData;		// store the raw data.
			int countLines = 0;
			if (dataFile.is_open()) {
				string dataLineRaw;
				while (getline(dataFile, dataLineRaw)) {
					rawData += dataLineRaw;
					rawData += "\n";
					countLines++;
				}
				dataFile.close();
			}

			cout << "Finished reading model file " << name << "\n";

			istringstream rawDataStream(rawData);
			string dataLine;
			int linesDone = 0;
			while (std::getline(rawDataStream, dataLine)) {
				if (dataLine.find("v ") != string::npos) {	// does this line have a vector?
					glm::vec3 vertex;

					int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
					vertex.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

					foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
					vertex.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

					foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
					vertex.z = stof(dataLine.substr(foundStart, foundEnd - foundStart));

					objVertices.push_back(vertex);
				}
				else if (dataLine.find("vt ") != string::npos) {	// does this line have a uv coordinates?
					glm::vec2 uv;

					int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
					uv.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

					foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
					uv.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

					objUvs.push_back(uv);
				}
				else if (dataLine.find("vn ") != string::npos) { // does this line have a normal coordinates?
					glm::vec3 normal;

					int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
					normal.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

					foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
					normal.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

					foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
					normal.z = stof(dataLine.substr(foundStart, foundEnd - foundStart));

					objNormals.push_back(normal);
				}
				else if (dataLine.find("f ") != string::npos) { // does this line defines a triangle face?
					string parts[3];

					int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
					parts[0] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

					foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
					parts[1] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

					foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
					parts[2] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

					for (int i = 0; i < 3; i++) {		// for each part

						vertexIndices.push_back(stoul(parts[i].substr(0, parts[i].find("/"))));

						int firstSlash = parts[i].find("/"); int secondSlash = parts[i].find("/", firstSlash + 1);

						if ((firstSlash + 1) != (secondSlash)) {	// there are texture coordinates.
							uvIndices.push_back(stoul(parts[i].substr(firstSlash + 1, secondSlash - firstSlash + 1)));
						}


						normalIndices.push_back(stoul(parts[i].substr(secondSlash + 1)));

					}
				}
				else if (dataLine.find("mtllib ") != string::npos) { // does this object have a material?
					materials.push_back(dataLine.substr(dataLine.find(" ") + 1));
				}
	
				linesDone++;
				
				if (linesDone % 50000 == 0) {
					cout << "Parsed " << linesDone << " of " << countLines << " from model...\n";
				}

			}
			cout << "finished file push\n";
			for (unsigned int i = 0; i < vertexIndices.size(); i += 3) {
				outVertices.push_back(objVertices[vertexIndices[i + 2] - 1]);
				outNormals.push_back(objNormals[normalIndices[i + 2] - 1]);
				outUvs.push_back(objUvs[uvIndices[i + 2] - 1]);

				outVertices.push_back(objVertices[vertexIndices[i + 1] - 1]);
				outNormals.push_back(objNormals[normalIndices[i + 1] - 1]);
				outUvs.push_back(objUvs[uvIndices[i + 1] - 1]);

				outVertices.push_back(objVertices[vertexIndices[i] - 1]);
				outNormals.push_back(objNormals[normalIndices[i] - 1]);
				outUvs.push_back(objUvs[uvIndices[i + 0] - 1]);
			}
			cout << "finished array push\n";
			/*
			for (unsigned int i = 0; i < vertexIndices.size(); i++) {
			out_vertices.push_back(obj_vertices[vertexIndices[i] - 1]);
			out_normals.push_back(obj_normals[normalIndices[i] - 1]);
			out_uvs.push_back(obj_uvs[uvIndices[i] - 1]);
			}
			*/

			// Load Materials
			for (unsigned int i = 0; i < materials.size(); i++) {

				std::ifstream dataFileMat(materials[i]);

				string rawDataMat;		// store the raw data.
				int countLinesMat = 0;
				if (dataFileMat.is_open()) {
					string dataLineRawMat;
					while (getline(dataFileMat, dataLineRawMat)) {
						rawDataMat += dataLineRawMat;
						rawDataMat += "\n";
					}
					dataFileMat.close();
				}

				istringstream rawDataStreamMat(rawDataMat);
				string dataLineMat;
				while (std::getline(rawDataStreamMat, dataLineMat)) {
					if (dataLineMat.find("map_Kd ") != string::npos) {	// does this line have a texture map?
						textures.push_back(dataLineMat.substr(dataLineMat.find(" ") + 1));
					}
				}
			}
			cout << "finished matierials push\n";
			texture = new GLuint[textures.size()];		// Warning possible memory leak here - there is a new here, where is your delete?
			glGenTextures(textures.size(), texture);

			for (int i = 0; i < textures.size(); i++) {
				readTexture(textures[i], texture[i]);
			}
			
			cout << "done";
		}

		void readTexture(string name, GLuint textureName) {
			cout << "in read etxture\n";
			// Flip images as OpenGL expects 0.0 coordinates on the y-axis to be at the bottom of the image.
			stbi_set_flip_vertically_on_load(true);
			cout << "stbi success\n";
			// Load image Information.
			int iWidth, iHeight, iChannels;
			unsigned char *iData = stbi_load(name.c_str(), &iWidth, &iHeight, &iChannels, 0);

			// Load and create a texture 
			glBindTexture(GL_TEXTURE_2D, textureName); // All upcoming operations now have effect on this texture object
			cout << textureName << "\n";
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, iWidth, iHeight);
			cout << "tex fine\n";
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth, iHeight, GL_RGBA, GL_UNSIGNED_BYTE, iData);

			// This only works for 2D Textures...
			// Set the texture wrapping parameters (next lecture)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			// Set texture filtering parameters (next lecture)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// Generate mipmaps (next lecture)
			glGenerateMipmap(GL_TEXTURE_2D);

		}

		void DeleteTexture() {
			delete[] texture;
		}
};