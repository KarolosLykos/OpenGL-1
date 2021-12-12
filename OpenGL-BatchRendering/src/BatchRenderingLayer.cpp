#include "BatchRenderingLayer.h"

using namespace GLCore;
using namespace GLCore::Utils;

BatchRenderingLayer::BatchRenderingLayer()
	:m_CameraController(16.0f / 9.0f)
{
}

BatchRenderingLayer::~BatchRenderingLayer()
{
}

struct Vec2 { float x, y; };
struct Vec3 { float x, y, z; };
struct Vec4 { float x, y, z, w; };

struct Vertex
{
	Vec3 Position;
	Vec4 Color;
	Vec2 TexCoords;
	float TexID;
};

static GLuint LoadTexture(const std::string& path)
{
	int w, h, bits;
	stbi_set_flip_vertically_on_load(1);
	auto* pixels = stbi_load(path.c_str(), &w, &h, &bits, STBI_rgb_alpha);
	GLuint textureID;
	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	stbi_image_free(pixels);

	return textureID;
}

void BatchRenderingLayer::OnAttach()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	EnableGLDebugging();

	m_Shader = Shader::FromGLSLTextFiles(
		"assets/shaders/shader.glsl.vert",
		"assets/shaders/shader.glsl.frag"
	);

	glUseProgram(m_Shader->GetRendererID());
	auto loc = glGetUniformLocation(m_Shader->GetRendererID(), "u_Textures");
	int samplers[2] = { 0, 1 };
	glUniform1iv(loc, 2, samplers);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glCreateVertexArrays(1, &m_QuadVA);
	glBindVertexArray(m_QuadVA);

	glCreateBuffers(1, &m_QuadVB);
	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 1000, nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(m_QuadVB, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));

	glEnableVertexArrayAttrib(m_QuadVB, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Color));

	glEnableVertexArrayAttrib(m_QuadVB, 2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoords));

	glEnableVertexArrayAttrib(m_QuadVB, 3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexID));

	uint32_t indices[] = {
		0,1,2,2,3,0,
		4,5,6,6,7,4
	};

	glCreateBuffers(1, &m_QuadIB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadIB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	m_Elephant = LoadTexture("assets/img/Cherno.png");
	glActiveTexture(GL_TEXTURE2);
	m_Lupus = LoadTexture("assets/img/Hazel.png");
}

void BatchRenderingLayer::OnDetach()
{
	// Shutdown here
}

void BatchRenderingLayer::OnEvent(Event& event)
{
	m_CameraController.OnEvent(event);
}

static void SetUniformMat4(uint32_t shader, const char* name, const glm::mat4& matrix)
{
	int loc = glGetUniformLocation(shader, name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

std::array<Vertex, 4> CreateQuad(float x, float y, float textureID)
{
	float size = 1.0f;
	Vertex v0;
	v0.Position = { x, y, 0.0f };
	v0.Color = { 0.18f, 0.6f, 0.96f, 1.0f };
	v0.TexCoords = { 0.0f, 0.0f };
	v0.TexID = textureID;

	Vertex v1;
	v1.Position = { x + size, y, 0.0f };
	v1.Color = { 0.18f, 0.6f, 0.96f, 1.0f };
	v1.TexCoords = { 1.0f, 0.0f };
	v1.TexID = textureID;

	Vertex v2;
	v2.Position = { x + size , y + size, 0.0f };
	v2.Color = { 0.18f, 0.6f, 0.96f, 1.0f };
	v2.TexCoords = { 1.0f, 1.0f };
	v2.TexID = textureID;

	Vertex v3;
	v3.Position = { x, y + size, 0.0f };
	v3.Color = { 0.18f, 0.6f, 0.96f, 1.0f };
	v3.TexCoords = { 0.0f, 1.0f };
	v3.TexID = textureID;

	return { v0, v1, v2, v3 };
}

void BatchRenderingLayer::OnUpdate(Timestep ts)
{
	m_CameraController.OnUpdate(ts);


	// Set dynamic vertex buffer
	/*float vertices[] = {
		-1.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,	0.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,	0.0f,
		-1.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,	0.0f,

		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,	1.0f,
		 1.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,	1.0f,
		 1.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,	1.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,	1.0f
	};*/

	auto q0 = CreateQuad(m_QuadOnePosition[0], m_QuadOnePosition[1], 0.0f);
	auto q1 = CreateQuad(m_QuadTwoPosition[0], m_QuadTwoPosition[1], 1.0f);

	Vertex vertices[8];
	memcpy(vertices, q0.data(), q0.size() * sizeof(Vertex));
	memcpy(vertices + q0.size(), q1.data(), q1.size() * sizeof(Vertex));

	glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	

	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(m_Shader->GetRendererID());
	glBindTextureUnit(0, m_Elephant);
	glBindTextureUnit(1, m_Lupus);

	SetUniformMat4(m_Shader->GetRendererID(), "u_ViewProj", m_CameraController.GetCamera().GetViewProjectionMatrix());
	SetUniformMat4(m_Shader->GetRendererID(), "u_Transform", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));

	glBindVertexArray(m_QuadVA);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
}

void BatchRenderingLayer::OnImGuiRender()
{
	// ImGui here
	ImGui::Begin("Controls");
	ImGui::DragFloat2("Quad 1 Position", m_QuadOnePosition, 0.1f);
	ImGui::DragFloat2("Quad 2 Position", m_QuadTwoPosition, 0.1f);
	ImGui::End();
}
