#include "renderer.h"
#include <fstream>
#include <iostream>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\mat4x4.hpp>
#include <glm\vec3.hpp>

using namespace std;

Renderer::Renderer() {
	glEnable(GL_TEXTURE_2D);
	this->shader.create("vertexShader.glsl", "fragmentShader.glsl");
	this->shader.use();
}

Renderer::~Renderer() {

}

void Renderer::Render(Model &model) {
	shader.uniform("texSampler", 0);
	model.texture.bind(0);
	this->tal += 0.0005;
	this->shader.uniform("ViewProjMatrix", this->camera.getProjMatrix() * this->camera.getViewMatrix());
	for (int i = 0; i < model.model_meshes.size(); i++) {
		model.model_meshes[i].first->bind();
		this->shader.uniform("modelMatrix", glm::rotate(model.model_meshes[i].second, this->tal, glm::vec3(0, 1, 0)));
		glDrawElements(GL_TRIANGLES, model.model_meshes[i].first->numIndices(), GL_UNSIGNED_INT, 0);
	}
}

void Renderer::Render(Heightmap &map) {
	this->shader.uniform("ViewProjMatrix", this->camera.getProjMatrix() * this->camera.getViewMatrix());
	map.bind();
	glm::mat4 trans(1);
	trans = glm::translate(trans, map.pos);
	this->shader.uniform("modelMatrix", trans);
	glDrawElements(GL_TRIANGLE_STRIP, (GLuint)map.indices.size(), GL_UNSIGNED_INT, 0);
}

