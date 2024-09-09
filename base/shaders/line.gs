R"(
#version 330 core

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

uniform float aspectRatio;
uniform float lineThickness;

vec4 V2toV4(vec2 v)
{
    return vec4(v, 1.0, 1.0);
}

void main()
{
    vec2 point1 = gl_in[0].gl_Position.xy;
    vec2 point2 = gl_in[1].gl_Position.xy;

    vec2 diff = point2 - point1;
    vec2 perpendicular = normalize(vec2(-diff.y, diff.x)) * lineThickness;
    perpendicular.x /= aspectRatio;

    // Triangle 1
    gl_Position = V2toV4(point1 + perpendicular);
    EmitVertex();

    gl_Position = V2toV4(point2 + perpendicular);
    EmitVertex();

    gl_Position = V2toV4(point1 - perpendicular);
    EmitVertex();

    // Triangle 2
    gl_Position = V2toV4(point2 - perpendicular);
    EmitVertex();


    EndPrimitive();
}
)"
