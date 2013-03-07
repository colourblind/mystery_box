/*
 * Definitions:
 * BAILOUT
 * SCALE
 * FOV
 * LIGHT_POS
 */

float mandelbox(float4 z)
{
    float lolwut = pown(2.f, 1 - BAILOUT);
    float dz = 1.f;
    float4 v = z;
    float m;
    
    for (int i = 0; i < BAILOUT; i ++)
    {
        if (v.x > 1)
            v.x = 2 - v.x;
        else if (v.x < -1)
            v.x = -2 - v.x;
            
        if (v.y > 1)
            v.y = 2 - v.y;
        else if (v.y < -1)
            v.y = -2 - v.y;
            
        if (v.z > 1)
            v.z = 2 - v.z;
        else if (v.z < -1)
            v.z = -2 - v.z;
            
        // Sphere fold
        m = length(v);
        if (m < 0.5)
        {
            v *= 4;
            dz *= 4;
        }
        else if (m < 1)
        {
            v *= 1.f / (m * m);
            dz *= 1.f / (m * m);
        }
        
        v *= SCALE;
        v += z;
        
        dz = dz * fabs(convert_float(SCALE)) + 1.f;
    }
    
    return length(v) / fabs(dz) - lolwut;
}


float4 align_to_vector(float4 a, float4 forward)
{
    float4 r;
    float4 up, side;

    forward = normalize(forward);

    if (forward.x == 0 && (forward.y == 1 || forward.y == -1) && forward.z == 0)
    {
        up.x = 0;
        up.y = 0;
        up.z = 1;
    }
    else
    {
        up.x = 0;
        up.y = 1;
        up.z = 0;
    }

    side = cross(up, forward);
    up = cross(side, forward);

    side = normalize(side);
    up = normalize(up);

    r.x = side.x * a.x + up.x * a.y + forward.x * a.z;
    r.y = side.y * a.x + up.y * a.y + forward.y * a.z;
    r.z = side.z * a.x + up.z * a.y + forward.z * a.z;
    r.w = 0;

    return r;
}

float go(float4 start, float4 dir)
{
    float march = 0, distance;
    float4 pos;

    while(march < 30)
    {
        pos = start + (dir * march);
        
        distance = mandelbox(pos);
        if (distance < 0.00025f)
            return march;
    
        march += distance;
    }

    return -1;
}

float colour(float4 position, float4 offset, float4 light_pos)
{
    const float ambient_scale = 0.1f;
    float4 light_dir = normalize(light_pos - position);
    float4 ao_sample_pos;
    float diffuse = 0, ambient;
    float4 offsetx = { offset.x, 0, 0, 0 };
    float4 offsety = { 0, offset.y, 0, 0 };
    float4 offsetz = { 0, 0, offset.z, 0 };

    float x0 = mandelbox(position - offsetx);
    float x1 = mandelbox(position + offsetx);
    float y0 = mandelbox(position - offsety);
    float y1 = mandelbox(position + offsety);
    float z0 = mandelbox(position - offsetz);
    float z1 = mandelbox(position + offsetz);
    
    float4 normal = { x1 - x0, y1 - y0, z1 - z0, 0 };

    normal = normalize(normal);

    if (go(position + light_dir * 0.01f, light_dir) < 0)
        diffuse = clamp(dot(normal, light_dir), 0.f, 1.f);

    ao_sample_pos = position + normal * 0.025f;
    ambient = mandelbox(ao_sample_pos);
    ambient = clamp(ambient * 100, 0.f, 1.f);

    return diffuse * (1 - ambient_scale) + ambient * ambient_scale;
}

__kernel void test(__global float *out, __const float4 camera_pos, __const float4 camera_dir, __const float4 light_pos)
{
    const int2 pos = { get_global_id(0), get_global_id(1) };
    const int2 dims = { get_global_size(0), get_global_size(1) };
    const float half_width = convert_float(dims.x) / 2;
    const float half_height = convert_float(dims.y) / 2;
    const float half_fov_h = radians(convert_float(FOV) / 2);
    const float half_fov_v = radians((FOV / 2) * (convert_float(dims.y) / dims.x));
    float4 ray_dir_screen;
    float4 ray_dir;
    float4 offset;
    float distance;
    
    ray_dir_screen.x = tan(((pos.x - half_width) / half_width) * half_fov_h);
    ray_dir_screen.y = tan(((pos.y - half_height) / half_height) * half_fov_v);
    ray_dir_screen.z = 1;

    ray_dir = normalize(align_to_vector(ray_dir_screen, camera_dir));

    distance = go(camera_pos, ray_dir);

    if (distance >= 0)
    {
        offset.x = tan(((pos.x + 0.5f - half_width) / half_width) * half_fov_h);
        offset.y = tan(((pos.y + 0.5f - half_height) / half_height) * half_fov_v);
        offset.z = 1;
        offset = (offset * distance) - (ray_dir_screen * distance);
        offset.z = offset.x;
        offset = fabs(offset);

        out[pos.y * dims.x + pos.x] = colour(camera_pos + ray_dir * distance, offset, light_pos);
    }
    else
        out[pos.y * dims.x + pos.x] = distance;
}
