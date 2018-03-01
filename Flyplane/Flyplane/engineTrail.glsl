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

const float life = 0.15;
//uniform float life;
uniform float thrust;
uniform float radius;
uniform float dt;
uniform vec3 spawn;
uniform vec3 direction;

// [-1, 1]
uniform float throttle;

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

// [-1, 1]
float rand(float n) { 
	return 2.0*fract(sin(n * 12.9898) * 43758.5453)-1.0;
}

void main()
{
	float r = radius;

	uint gid = gl_GlobalInvocationID.x;
	if(Positions[gid].xyz == vec3(0))
	{
		// INITAL SPAWN

		Positions[gid].xyz = spawn;
		Lives[gid] = life * (rand(float(gid) + 1000) + 1)/2;
		Colors[gid].rgb = vec3(1.0,0.5,0.0);
	}
	else
	{
		// UPDATE
		float t = throttle;
		
		Colors[gid].a = 1.0 * r * smoothstep(life+ 0.1*life*abs(rand(gid)), 0, Lives[gid]);
		Colors[gid].a *= 0.4 + pow(t*0.5 + 0.5, 2);

		//Colors[gid].a = 1.0;

		Positions[gid].xyz = spawn + Velocities[gid].xyz + (direction - 0.15*Velocities[gid].xyz) * Lives[gid]*(24 + 4*t);
	}
	Lives[gid] += dt;
	if(Lives[gid] >= life + 0.1*life*abs(rand(gid)))
	{
		// RESPAWN
		Lives[gid] -= life;

		vec3 d = normalize(direction);

		vec3 offset;
		offset.x = rand(gid * 7 + 100 + dt);
		offset.y = rand(gid * 13 - 100+ dt);
		offset.z = rand(gid * 17 - 200+ dt);

		
		offset = r*rand(gid * 19)*normalize(offset);
		offset  = r * normalize(offset);

		Velocities[gid].xyz = offset;

		Positions[gid].xyz = spawn + offset; 

		//Lives[gid] = dt * life * (rand(float(gid) + 1000 + dt) + 1)/2;
	}
}

