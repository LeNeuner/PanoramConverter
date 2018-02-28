#ifdef GL_ES
precision mediump int;
precision mediump float;
#endif

uniform sampler2D source;
uniform sampler2D destination;
varying vec2      v_dst_coord;

const float PI   = 3.14159265358979323846;
const float PI_2 = 1.57079632679489661923;
float cubeSizeX = 0.25;
float cubeSizeY = 0.33;

// определение текущей грани куба
bool getCurrentFaceVal(in vec2 coord, out int face)
{
    int quadVal = int(coord.x / cubeSizeX);
    if (quadVal == 1)
    {
        if (coord.y < cubeSizeY)
            face = 4;
        else if (coord.y >= cubeSizeY * 2.0)
            face = 5;
        else
            face = 1;
    }
    else
    {
        if ((coord.y >= cubeSizeY) && (coord.y < cubeSizeY * 2.0))
            face = quadVal;
        else
            return false;
    }
    return true;
}

// координаты с развертки в кубические координаты
vec3 projectToCubic(in vec2 coord, in int faceVal)
{
    //  coord xy [0 - 1.0]
    float coefX = coord.x * 8.0;
    float coefY = coord.y * 6.0;

    vec3 point;
    if (faceVal == 0)
    {
        point = vec3(coefX - 1.0, -1.0, 3.0 - coefY);
    }
    else if (faceVal == 1)
    {
        point = vec3(1.0, coefX - 3.0, 3.0 - coefY);
    }
    else if (faceVal == 2)
    {
        point = vec3(5.0 - coefX, 1.0, 3.0 - coefY);
    }
    else if (faceVal == 3)
    {
        point = vec3(-1.0, 7.0 - coefX, 3.0 - coefY);
    }
    else if (faceVal == 4)
    {
        point = vec3(coefY - 1.0, coefX - 3.0, 1.0);
    }
    else if (faceVal == 5)
    {
        point = vec3(5.0 - coefY, coefX - 3.0, -1.0);
    }
    else
        point = vec3(0.0, 0.0, 0.0);
    return point;
}

// конвертация проекции
vec3 convertProjection(in sampler2D src)
{
    // определение текущей грани куба
    int faceVal = 0;
    if (!getCurrentFaceVal(v_dst_coord, faceVal))
        return vec3(1.0, 0.0, 0.0);

    // координата развертки в кубическую координату
    vec3 point = projectToCubic(v_dst_coord, faceVal);

    // проецирование кубической координаты в полярную
    float theta = atan(point.y, point.x);
    float r = sqrt(point.x * point.x + point.y * point.y);
    float phi = atan(point.z, r);

    // координата на исходом изображении
    float uDbl = 0.5 * (theta + PI) / PI;
    float vDbl = 1.0 * (PI_2 - phi) / PI;

    vec3 color = texture2D(src, vec2(uDbl, vDbl));
    return color;
}

void main()
{
    // устанавливаем цвета из конвертированной текстуры
    gl_FragColor = vec4(convertProjection(source), 1.0);
}


