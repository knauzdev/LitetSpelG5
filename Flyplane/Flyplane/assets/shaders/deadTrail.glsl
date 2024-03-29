
layout(local_size_x = LOCAL_SIZE_X, local_size_y = 1, local_size_z = 1) in;

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

float rand(float n);

const vec3 black = vec3(0);
const vec3 fire = vec3(1.0, 0.5, 0.0);

void main()
{
	uint gid = gl_GlobalInvocationID.x;
	if(Positions[gid].xyz == vec3(0))
	{
		Positions[gid].xyz = spawn;
		Lives[gid] = life * (rand(float(gid) + 1000) + 1)/2;
		Colors[gid].rgb = fire;
		Colors[gid].a = 1.0;
		Positions[gid].a = 1.0;
	}
	else
	{
		vec3 dir;
		dir.x = rand(gid*23);
		dir.z = rand(gid*27);
		dir = normalize(dir);
		dir += vec3(0, 13, 0);
		Velocities[gid].xyz = mix(Velocities[gid].xyz, dir, 1-pow(0.1, dt));
		Positions[gid].xyz += Velocities[gid].xyz * dt;


		Colors[gid].rgb = mix(fire, black, smoothstep(0, 0.02*life, Lives[gid]));

		//Colors[gid].a = pow(1-Lives[gid] / life, 4);
		Positions[gid].w = pow(1-Lives[gid] / life, 4);

		//Positions[gid].xyz = spawn + direction * Lives[gid];
	}
	Lives[gid] += dt;
	if(Lives[gid] >= life + 0.1*life*rand(float(gid)))
	{
		Lives[gid] -= life;
		Positions[gid].xyz = spawn + direction*abs(dt * 0.5*(rand(float(gid) + dt) + 1));

		vec3 vel;
		vel.x = rand(float(gid + dt*10));
		vel.y = rand(float(gid + 1000 + dt*10));
		vel.z = rand(float(gid + 2000 + dt*10));
		Velocities[gid].xyz = 4 * normalize(vel);
	}
}

// [-1, 1]
float rand(float n) { 
	return 2.0*fract(sin(n * 12.9898) * 43758.5453)-1.0;
}