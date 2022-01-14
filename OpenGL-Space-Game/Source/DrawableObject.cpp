#include "Headers/DrawableObject.h"

DrawableObject::DrawableObject(Shader* shader)
	: m_shader(shader)
{
	m_shader->use();
	// get base attribs and uniform. expect all shaders to share the same ids
	positionLoc = m_shader->get_attrib("a_position");
	normalLoc = m_shader->get_attrib("a_normal");
	texCoordLoc = m_shader->get_attrib("a_texCoord");

	// not used by all shaders
	samplerLoc = m_shader->get_uniform("s_texture");
}

DrawableObject::~DrawableObject()
{

}