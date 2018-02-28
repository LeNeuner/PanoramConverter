#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

uniform mat4 mvp_matrix;

attribute vec4 a_position;
attribute vec2 a_texcoord;

varying vec2 v_dst_coord;

void main()
{
    // расчет положения вершин в экранных координатах
    gl_Position = mvp_matrix * a_position;

    // передать координаты во фрагментный шейдер
    v_dst_coord = a_texcoord;
}
