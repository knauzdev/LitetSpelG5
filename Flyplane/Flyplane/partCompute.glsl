#version 430 core
layout(std430, binding=7) buffer Pos
{
	vec4 Positions[];
};
layout(std430, binding=8) buffer Vel
{
	vec4 Velocities[];
};
layout(std430, binding=9) buffer Life
{
	float Lives[];
};
layout(std430, binding=10) buffer Col
{
	vec4 Colors[];
};
uniform float dt;
uniform float life;
uniform vec3 spawn;
uniform vec3 direction;
layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;
void main()
{
	uint gid = gl_GlobalInvocationID.x;
	if(Positions[gid].xyz == vec3(0))
	{
		Positions[gid].xyz = spawn;
	}
	else
	{
		Positions[gid].xyz = spawn + direction * Lives[gid];
	}
	Lives[gid] += dt;
	if(Lives[gid] > life)
	{
		Lives[gid] = 0.0;
	}
}