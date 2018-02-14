#version 420

uniform sampler2D shadowMap;
uniform mat4 shadowMatrix;
in vec3 vPos;
in vec3 vNormal;
in vec2 vTex;
in vec3 vMaterials;

uniform sampler2D heightmap;
uniform sampler2D materialmap;
uniform sampler2D material1;
uniform sampler2D material2;
uniform sampler2D material3;
uniform vec2 heightmapSize;

uniform vec3 cameraPos;

float noise(vec2 pos);

float detail(vec2 pos) {
	return 0.1*(noise(pos*0.1) + 0.5*noise(pos*0.5));
}

float sampleHeightmap(vec2 uv, vec2 offset) {
	float result = 0;
	uv += offset/heightmapSize;
	result += texture(heightmap, uv).r;
	//result += 0.1*(noise(uv*3) + 0.5*noise(uv*10.0));
	return result;
}
float sampleHeightmap(vec2 uv) {
	return sampleHeightmap(uv, vec2(0));
}
vec3 sampleNormal(vec2 hmUV) {
	vec4 h;
	h[0] = sampleHeightmap(hmUV, vec2(0, -1));
	h[1] = sampleHeightmap(hmUV, vec2(-1, 0));
	h[2] = sampleHeightmap(hmUV, vec2(1, 0));
	h[3] = sampleHeightmap(hmUV, vec2(0, 1));

	vec3 n;
	n.z = h[0] - h[3];
	n.x = h[1] - h[2];
	n.y = 1.0/255.0;
	return normalize(n);
}

void main() {
	vec3 shadowCoord = (shadowMatrix * vec4(vPos, 1)).xyz;
	float depth = texture(shadowMap, shadowCoord.xy).r;
	float visibility = 1.0;
	
	if(depth < shadowCoord.z - 0.001) {
		visibility = 0.0;
	}
	float x = shadowCoord.x;
	float y = shadowCoord.y;
	if(x < 0 || x > 1 || y < 0 || y > 1) {
		visibility = 1.0;
	}

	
	vec4 h;
	h[0] = detail(vPos.xz + vec2(0, -1));
	h[1] = detail(vPos.xz + vec2(-1, 0));
	h[2] = detail(vPos.xz + vec2(1, 0));
	h[3] = detail(vPos.xz + vec2(0, 1));
	vec3 Normal;
	Normal.z = h[0] - h[3];
	Normal.x = h[1] - h[2];
	Normal.y = 0.15;
	Normal = normalize(Normal);
	
	vec3 sun = vec3(0, 1, 1);
	sun = normalize(sun);
	vec3 n = normalize(vNormal + Normal);

	

	vec3 color1 = texture(material1, vec2(vTex.x, 1 - vTex.y)).rgb; 
	vec3 color2 = texture(material2, vec2(vTex.x, 1 - vTex.y)).rgb;
	vec3 color3 = texture(material3, vec2(vTex.x, 1 - vTex.y)).rgb;

	vec3 color = vec3(0);
	color += color1*vMaterials.x;
	color += color2*vMaterials.y;
	color += color3*vMaterials.z;

	color = texture(materialmap, vec2(vTex.x, vTex.y)).rgb; 

	float result = dot(sun, n);
	result = clamp(result, 0, 1);

	//gl_FragColor = vec4(color * result * visibility * 0.7 + color * 0.3, 1);

	float dist = length(cameraPos - vPos);
	float fog = pow(clamp(dist/48000.0, 0.0, 1.0), 1.5);
	vec3 fogColor = 0.95*vec3(100.0/255,149.0/255,234.0/255);

	color = mix(color, vec3(fogColor), fog);

	gl_FragColor = vec4(color, 1.0);
	//gl_FragColor = vec4(vMaterials, 1.0);
}



float rand(vec2 n) { 
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}
float noise(vec2 n) {
	const vec2 d = vec2(0.0, 1.0);
	vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
	return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}