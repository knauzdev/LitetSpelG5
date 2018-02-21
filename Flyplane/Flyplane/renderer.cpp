#include "renderer.h"
#include "transform.h"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\mat4x4.hpp>
#include <glm\vec3.hpp>
#include <iostream>
#include "window.h"
#include "assetloader.h"
#include "input.h"
#define FISHROD 0
#define STINGER 1
#define ROCKETPOD 2


using namespace std;

Renderer::Renderer() {
	this->shader.create("vertexShader.glsl", "fragmentShader.glsl");
	this->terrain_shader.create("terrainVertexShader.glsl","geometryShader.glsl", "terrainFragmentShader.glsl");
	this->shadow.create("shadowVertexShader.glsl", "shadowFragmentShader.glsl");
	this->guiShader.create("guiVertexSHader.glsl", "guiFragmentShader.glsl");
	this->enemyMarkerShader.create("enemymarkerVS.glsl","enemymarkerGS.glsl", "enemymarkerFS.glsl");
	this->missileShader.create("missileVS.glsl", "missileFS.glsl");

	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 4*1024, 4*1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "framebuffer broken" << endl;

	
	m = glm::mat4(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1
	);
	
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float vertexbuffer[] = {
		-1.0,  1.0, 0.0,
		 0.0,  1.0,

		 1.0,  1.0, 0.0,
		 1.0,  1.0,

		-1.0, -1.0, 0.0,
		 0.0,  0.0,

		 1.0, -1.0, 0.0,
		 1.0,  0.0
	};

	glGenVertexArrays(1, &quadVao);
	glGenBuffers(1, &quadVbo);
	glBindVertexArray(quadVao);
	glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
	glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(float), vertexbuffer, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));

	glBindVertexArray(0);

	hpbar.loadTexture("assets/textures/hpbar.png");
	hpTexture.loadTexture("assets/textures/hp.png", 1);
	hpMatrix = glm::translate(glm::vec3(-0.8, -0.8, 0)) * glm::scale(glm::vec3(0.15, 0.05, 1));

	missileVPMatrix = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -5.0f, 10.0f);// glm::ortho(-0.05f, 0.05f, -0.05f, 0.05f, -1.0f, 1.0f) * glm::perspective(glm::radians(80.0f), 1.0f, .01f, 220.0f); //glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, -5.0f, 10.0f) * glm::rotate(4.0f / 4.0f, glm::vec3(0, 0, -1)) * glm::rotate(3.14f / 4.0f, glm::vec3(-1, 0, 0));
	missileModelMatrix = glm::rotate(3.14f / 4.0f, glm::vec3(0, 0, -1)) * glm::rotate(3.14f / 4.0f, glm::vec3(-1, 0, 0));//glm::translate(glm::vec3(0.7, 0, -200)) * glm::rotate(3.14f / 4.0f, glm::vec3(0, 0, -1)) * glm::rotate(3.14f / 4.0f, glm::vec3(-1, 0, 0));
}

Renderer::~Renderer() {

}

void Renderer::addToList(Model* model, Transform trans) {
	list.push_back({ model, trans });
}



void Renderer::addToList(const std::vector<Patch>& patches) {
	this->patches = patches;
	//mapList.push_back(map);
}

void Renderer::Render(Model &model, Transform &trans) {
	glm::mat4 modelMatrix = glm::translate(trans.pos) * glm::toMat4(trans.orientation) * glm::scale(trans.scale);
	model.texture.bind(0);

	for (int i = 0; i < model.model_meshes.size(); i++) {
		model.model_meshes[i].first->bind();
		this->shader.uniform("modelMatrix", modelMatrix*model.model_meshes[i].second);
		glDrawElements(GL_TRIANGLES, model.model_meshes[i].first->numIndices(), GL_UNSIGNED_INT, 0);
	}
}


void Renderer::Render(RenderObject& obj) {
	Render(*obj.model, obj.trans);
}


void Renderer::RenderShadow(Model & model, Transform & trans) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glm::mat4 modelMatrix = glm::translate(trans.pos) * glm::toMat4(trans.orientation);

	this->shadow.use();
	for (int i = 0; i < model.model_meshes.size(); i++) {
		model.model_meshes[i].first->bind();
		this->shader.uniform("MVP", shadowMatrix * modelMatrix);
		glDrawElements(GL_TRIANGLES, model.model_meshes[i].first->numIndices(), GL_UNSIGNED_INT, 0);
	}
}

void Renderer::RenderWeapon() {
	glEnable(GL_DEPTH_TEST); //
	auto s = Window::getWindow().size();
	glViewport(s.x - 300, 0, 150, 150);
	
	this->missileShader.use();
	missile->texture.bind(0);
	missile->model_meshes[0].first->bind();

	float scale = 1.0f / missile->getBoundingRadius();

	if (scale > 10000) {
		scale = 0.8;
	}

	this->missileShader.uniform("ViewProjMatrix", missileVPMatrix);
	float angle = time.elapsed();

	if (angle > 2 * 3.14) {
		time.restart();
	}
	this->missileShader.uniform("modelMatrix", missileModelMatrix * glm::rotate(angle, glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(scale)));
	glDrawElements(GL_TRIANGLES, missile->model_meshes[0].first->numIndices(), GL_UNSIGNED_INT, 0);

	glViewport(0, 0, s.x, s.y);
	AssetLoader::getLoader().getText()->drawText("Ammo: " + std::to_string(weaponAmmo), glm::vec2(s.x - 400, 20), glm::vec3(1, 1, 0), 0.4);
}

Timer t;


void Renderer::RenderScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	shadow.use();
	glViewport(0, 0, 4*1024, 4*1024);
	glClear(GL_DEPTH_BUFFER_BIT);

	//Render shadow
	/*
	for (int i = 0; i < list.size(); i++) {
		glm::mat4 modelMatrix = glm::translate(list[i].trans.pos) * glm::toMat4(list[i].trans.orientation) * glm::scale(list[i].trans.scale);

		for (int j = 0; j < list[i].model->model_meshes.size(); j++) {
			list[i].model->model_meshes[j].first->bind();
			shadow.uniform("MVP", shadowMatrix * modelMatrix * list[i].model->model_meshes[j].second);
			glDrawElements(GL_TRIANGLES, list[i].model->model_meshes[j].first->numIndices(), GL_UNSIGNED_INT, 0);
		}
	}
	for (int i = 0; i < mapList.size(); i++) {
		mapList[i]->bind();
		glm::mat4 trans = glm::translate(mapList[i]->pos);
		this->shadow.uniform("MVP", shadowMatrix*trans);
		glDrawElements(GL_TRIANGLES, (GLuint)mapList[i]->indices.size(), GL_UNSIGNED_INT, 0);
	}
	*/

	//Render scene
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	auto s = Window::getWindow().size();
	glViewport(0, 0, s.x, s.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.use();
	shader.uniform("texSampler", 0);
	shader.uniform("shadowMap", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glm::mat4 viewProjMatrix = this->camera.getProjMatrix() * this->camera.getViewMatrix();
	shader.uniform("ViewProjMatrix", viewProjMatrix);
	shader.uniform("shadowMatrix", m * shadowMatrix);
	shader.uniform("cameraPos", camera.getTransform().pos);
	for (int i = 0; i < list.size(); i++) {
		Render(list[i]);
	}
	//Render terrain
	terrain_shader.use();
	terrain_shader.uniform("shadowMatrix", m * shadowMatrix);
	//terrain_shader.uniform("shadowMatrix", shadowMatrix);
	terrain_shader.uniform("ViewProjMatrix", viewProjMatrix);

	terrain_shader.uniform("shadowMap", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	if (hm != NULL) {
		terrain_shader.uniform("time", (float)globalTime.elapsed());
		terrain_shader.uniform("cameraPos", camera.getTransform().pos);
		hm->bind(terrain_shader);
		for (int i = 0; i < patches.size(); i++) {
			int indices = patches[i].indices;
			hm->bindIndices(indices);

			terrain_shader.uniform("offset", patches[i].offset);
			terrain_shader.uniform("patch_size", glm::vec2(patches[i].size));
			glDrawElements(GL_TRIANGLES, (GLuint)hm->indices[indices].size(), GL_UNSIGNED_INT, 0);
		}
	}

	//Render markers
	enemyMarkerShader.use();
	markers.bind();
	std::vector<Marker> p = markers.getMarkers();
	enemyMarkerShader.uniform("aspectMatrix", viewProjMatrix);
	enemyMarkerShader.uniform("cameraPos", this->camera.getTransform().pos);
	for (int i = 0; i < p.size(); i++) {
		enemyMarkerShader.uniform("transform", p[i].pos);
		enemyMarkerShader.uniform("scale", p[i].scale);
		enemyMarkerShader.uniform("color", p[i].color);
		glDrawArrays(GL_POINTS, 0, 1);
	}

	//Render weapon
	if (weaponAmmo)
		RenderWeapon();

	markers.clear();
	list.clear();
	mapList.clear();
}

void Renderer::RenderCrosshair() {
	guiShader.use();
	glm::mat4 pos = camera.getProjMatrix() * camera.getViewMatrix() * crosshair.getMatrix();
	guiShader.uniform("matrix", pos * glm::mat4(orientation) * glm::rotate(3.14f / 4, glm::vec3(0, 0, 1)) * glm::scale(glm::vec3(50, 50, 1)));
	guiShader.uniform("texSampler", 0);
	crosshair.Bind();
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::RenderClouds() {
	for (int i = 0; i < clouds.transform.size(); i++) {
		renderTexture(clouds.texture, camera.getProjMatrix() * camera.getViewMatrix() * clouds.transform[i]);
	}
}

void Renderer::RenderHPBar(float hp) {
	renderTexture(hpbar, hpMatrix);
	renderTexture(hpTexture, hpMatrix * glm::translate(glm::vec3(-1, 0, -0.01)) * glm::scale(glm::vec3(hp, 1, 1)) * glm::translate(glm::vec3(1, 0, 0)));
}

void Renderer::setWeaponModel(Model * mptr) {
	missile = mptr;
}

void Renderer::setAmmo(int ammo) {
	weaponAmmo = ammo;
}

Camera Renderer::getCamera() &
{
	return camera;
}
void Renderer::setCamera(const Camera & camera)
{
	this->camera = camera;
	auto pos = camera.getTransform().pos;
	pos += glm::toMat3(camera.getTransform().orientation)*glm::vec3(0,0, 1000.f);
	glm::mat4 proj = glm::ortho<float>(-2000.f, 2000.f, -2000.f, 2000.f, 0.f, 4000.f);
	glm::mat4 view = glm::lookAt(glm::vec3(pos.x, 2000.0f, 2000.0f + pos.z), glm::vec3(pos.x, 0, pos.z), glm::vec3(0, 1, 0));
	this->shadowMatrix = proj * view;
}

void Renderer::addMarker(glm::vec3 pos, float scale) {
	markers.addPosition(pos, scale);
}

void Renderer::addMarker(glm::vec3 pos, glm::vec3 color, float scale)
{
	markers.addPosition(pos, color, scale);
}

void Renderer::renderTexture(const Texture& texture, const glm::mat4& matrix) {
	guiShader.use();
	guiShader.uniform("matrix", matrix);
	guiShader.uniform("texSampler", 0);

	glBindVertexArray(quadVao);
	glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
	texture.bind(0);

	glDisable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glEnable(GL_CULL_FACE);
	//glEnable(GL_DEPTH_TEST);
}

void Renderer::setCrosshairPos(glm::vec3 pos) {
	crosshair.setMatrix(glm::translate(pos));
}

glm::mat4& Renderer::getCrosshairPos()
{
	return crosshair.getMatrix();
}

void Renderer::update(float dt)
{
	if (Input::isKeyPressed(GLFW_KEY_F8)) {
		this->terrain_shader.reload();
	}
}

