R"(
#version 330 core

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 12) out;

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
    vec2 point3 = gl_in[2].gl_Position.xy;

    // Calculate perpendiculars, 1 = current line, 2 = next line
    vec2 diff = point2 - point1;
    vec2 perpendicular1 = normalize(vec2(-diff.y, diff.x)) * lineThickness;
    perpendicular1.x /= aspectRatio;

    diff = point3 - point2;
    vec2 perpendicular2 = normalize(vec2(-diff.y, diff.x)) * lineThickness;
    perpendicular2.x /= aspectRatio;

    vec2 point1up = point1 + perpendicular1;
    vec2 point2up1 = point2 + perpendicular1;
    vec2 point2up2 = point2 + perpendicular2;
    vec2 point1down = point1 - perpendicular1;
    vec2 point2down1 = point2 - perpendicular1;
    vec2 point2down2 = point2 - perpendicular2;

    /* Draw the line */
    // Triangle 1
    gl_Position = V2toV4(point1up);
    EmitVertex();

    gl_Position = V2toV4(point2up1);
    EmitVertex();

    gl_Position = V2toV4(point1down);
    EmitVertex();

    // Triangle 2
    gl_Position = V2toV4(point2down1);
    EmitVertex();

    EndPrimitive();

    /* Draw the connector, using a miter join */
    float gradient1 = (point2 - point1).y / (point2 - point1).x;
    float gradient2 = (point3 - point2).y / (point3 - point2).x;

    //if (gradient1 - gradient2 < 0.0001)
    //    return;

    // Draw the connector on the top, calculate the intersection and draw
    // triangles
    float yintersect1 = point2up1.y - (gradient1 * point2up1.x);
    float yintersect2 = point2up2.y - (gradient2 * point2up2.x);
    float intersectX = (yintersect2 - yintersect1) / (gradient1 - gradient2);
    vec2 intersection = vec2(intersectX,
                             (gradient1 * intersectX) + yintersect1);

    gl_Position = V2toV4(point2up1);
    EmitVertex();

    gl_Position = V2toV4(point2);
    EmitVertex();

    gl_Position = V2toV4(intersection);
    EmitVertex();

    gl_Position = V2toV4(point2up2);
    EmitVertex();

    EndPrimitive();

    // Draw the connector on the bottom, calculate the intersection and draw
    // triangles
    yintersect1 = point2down1.y - (gradient1 * point2down1.x);
    yintersect2 = point2down2.y - (gradient2 * point2down2.x);
    intersectX = (yintersect2 - yintersect1) / (gradient1 - gradient2);
    intersection = vec2(intersectX,
                       (gradient1 * intersectX) + yintersect1);

    gl_Position = V2toV4(point2down1);
    EmitVertex();

    gl_Position = V2toV4(point2);
    EmitVertex();

    gl_Position = V2toV4(intersection);
    EmitVertex();

    gl_Position = V2toV4(point2down2);
    EmitVertex();

    EndPrimitive();

}
)"
