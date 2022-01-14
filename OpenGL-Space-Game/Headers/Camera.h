#pragma once

// Include GLM
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera
{
protected:
    // used in projection matrix
    float m_fov = 45.f;
    float m_ratio = 4.f / 3.f;
    float m_near_plane = 0.1f; // display range : near 0.1 unit <->  far 100 units
    float m_far_plane = 1000.f;

    float roll = 0.0f;
    float pitch = 0.0f;
    float yaw = 0.0f;
	
    glm::vec3 m_world_pos;
    glm::vec3 m_world_look_at_pos;
    glm::vec3 m_up_dir; // changing it can cause rolling

    glm::mat4 m_projection;
    glm::mat4 m_view;

    bool m_dirty_view = false;
    bool m_dirty_proj = false;

public:
    Camera(glm::vec3 world_pos, glm::vec3 look_at_world_pos, glm::vec3 up_dir);
    void Init();

    // Updates projection matrix : Field of View, ratio, display range
    void update_projection();
    // Updates view matrix : World pos, look at pos, up dir
    void update_view();

    // void LerpToTransform();

    void set_world_pos(const glm::vec3 new_w_pos) { m_world_pos = new_w_pos; }
    void add_to_world_pos(const glm::vec3 w_add_pos) { m_world_pos += w_add_pos; }
    glm::vec3 get_world_pos_v3() const { return m_world_pos; }

    void add_to_look_at_pos(const glm::vec3 w_add_pos) { m_world_look_at_pos += w_add_pos; }
    void set_world_look_at_pos(const glm::vec3 w_new_pos) { m_world_look_at_pos = w_new_pos; }
    glm::vec3 get_world_look_at_v3() const { return m_world_look_at_pos; }
	
    glm::mat4 get_projection_m4()           { if (m_dirty_proj) update_projection(); m_dirty_proj = false; return m_projection; }
    glm::mat4 get_view_m4()                 { if (m_dirty_view) update_projection(); m_dirty_view = false; return m_view; }
    glm::mat4 get_projection_m4()   const   { return m_projection; }
    glm::mat4 get_view_m4()         const   { return m_view; }

    float get_fov() const { return m_fov; }
    void set_fov(const float new_fov) { m_dirty_proj = true;  m_fov = new_fov; }

    void set_up_dir(const glm::vec3 up_dir) { m_up_dir = up_dir; }
};