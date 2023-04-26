#version 410 core
#ifdef VERTEX_PROGRAM
	layout (location = 0) in vec3 aPos; 
	layout (location = 1) in vec3 normals;
	layout (location = 2) in vec2 textCoords;
	layout (location = 3) in ivec4 boneIds;
	layout (location = 4) in vec4 weights;
	
	const int MAX_BONES = 100;
	const int MAX_BONE_INFLUENCE = 4;
	
	uniform mat4 view;
	uniform mat4 projection;
	uniform mat4 model;
	uniform mat4 finalBonesMatrices[MAX_BONES];
	
	uniform sampler2D normal_texture;
	
	out vec2 TextureCoords;
	out vec3 Normal;
	out vec3 FragPos;
	
	mat4 ScaleNormalize(mat4 matrix)
	{
		matrix[0][0] = normalize(matrix[0][0]);
		matrix[1][1] = normalize(matrix[1][1]);
		matrix[2][2] = normalize(matrix[2][2]);
		matrix[3][3] = normalize(matrix[3][3]);
		
		return matrix;
	}
	
	void main()
	{
		//BONES
		vec4 totalPosition = vec4(0.0f);
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
		{
			if (boneIds[i] == -1) 
				continue;

			if (boneIds[i] >= MAX_BONES)
			{
				totalPosition = vec4(aPos, 1.0f);
				break;
			}

			vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(aPos, 1.0f);
			totalPosition += localPosition * weights[i];
			vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * normals;
		}

		if (totalPosition.x == 0.0f && totalPosition.y == 0.0f && totalPosition.z == 0.0f && totalPosition.w == 0.00f)
		{
			totalPosition = vec4(aPos, 1.0f);
		}
	
		vec4 aPos4 = vec4(aPos, 1.0);
		
		//OUT
		FragPos = vec3(model * aPos4);
		TextureCoords = textCoords;
		Normal = normalize(mat3(transpose(inverse(model))) * normals);
		
		gl_Position = projection * view * model * totalPosition;
	}
#endif
#ifdef FRAGMENT_PROGRAM
	const uint MAX_POINT = 32;
	const uint MAX_SPOT = 32;

	struct Light
	{
		vec3 Color;
		float AmbientIntensity;
		float DiffuseIntensity;
	};
	
	struct DirectionalLight
	{
		Light Base;
		vec3 Direction;
	};
	
	struct PointLight
	{
		Light Base;
		
		float Constant;
		float Linear;
		float Exp;
		
		vec3 Position;
		
		float Distance;
	};
	
	struct SpotLight
	{
		Light Base;
		
		float Constant;
		float Linear;
		float Exp;
		float Cutoff;
		
		vec3 Position;
		vec3 Direction;
		vec3 Euler;
		float Distance;
	};
	
	uniform DirectionalLight Light_Directional;
	uniform PointLight[MAX_POINT] Light_Point;
	uniform SpotLight[MAX_SPOT] Light_Spot; 
	
	uniform int Actual_Point;
	uniform int Actual_Spot;
	
	uniform sampler2D albedo_texture;
	
	uniform vec3 ViewPoint;
	
	int steps = 2;
	float scaleFactor = 1.0f/steps;
	
	in vec2 TextureCoords;
	in vec3 FragPos;
	in vec3 Normal;
	
	out vec4 FragColor;
	
	//AUXILIAR METHODS
	mat4 rotate(mat4 matrix, float angle, vec3 axis) {
	    axis = normalize(axis);
	    float s = sin(angle);
	    float c = cos(angle);
	    float oc = 1.0 - c;
	    float x = axis.x;
	    float y = axis.y;
	    float z = axis.z;
	
	    mat4 rotation = mat4(oc * x * x + c, oc * x * y - z * s,  oc * x * z + y * s, 0.0,
	                         oc * x * y + z * s,  oc * y * y + c, oc * y * z - x * s, 0.0,
	                         oc * x * z - y * s,  oc * y * z + x * s,  oc * z * z + c, 0.0,
	                         0.0, 0.0, 0.0, 1.0);
	
		return matrix * rotation;
	}
	
	vec3 CalculateDirection(vec3 euler)
	{
		vec3 direction = vec3(0.0f, 0.0f, 1.0f);
		mat4 rotation;
		rotation[0] = vec4(1.0f, 0.0f, 0.0f, 0.0f);
		rotation[1] = vec4(0.0f, 1.0f, 0.0f, 0.0f);
		rotation[2] = vec4(0.0f, 0.0f, 1.0f, 0.0f);
		rotation[3] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		
		rotation = rotate(rotation, radians(euler.x), vec3(-1.0f, 0.0f, 0.0f));
		rotation = rotate(rotation, radians(euler.y), vec3(0.0f, -1.0f, 0.0f));
		rotation = rotate(rotation, radians(euler.z), vec3(0.0f, 0.0f, -1.0f));
		
		return vec3(rotation * vec4(direction, 0.0f));
	}
	
	//LIGHT METHODS
	vec4 CalculateLight(Light light, vec3 direction, vec3 normal)
	{
		//Ambient
		vec4 Ambient = vec4(light.Color, 1.0f) * light.AmbientIntensity;
		
		//Diffuse
		float diff = max(dot(normal, direction), 0.0);
		diff = ceil(diff * steps) * scaleFactor;
		
		vec4 Diffuse = vec4(light.Color, 1.0f) * light.DiffuseIntensity * diff;
		
		Diffuse.xyz = clamp(Diffuse.xyz, 0.15, 1.0);
		
		Diffuse.w = 1.0f;
		
		vec4 result = (Ambient * Diffuse);
		result.w = 1.0f;
		return result;
	}
	
	//Calculate Lights
	vec4 CalculateDirectionalLight(vec3 normal)
	{
		vec3 dir = normalize(CalculateDirection(Light_Directional.Direction));
		
		return CalculateLight(Light_Directional.Base, dir, normal);
	}
	
	vec4 CalculatePointLight(PointLight light, vec3 normal)
	{
		vec3 lightDir = normalize(light.Position - FragPos);
		float dist = length(light.Position - FragPos);
		
		vec4 color = vec4(0.1f);
		
		if (light.Distance > dist)
		{
			color = CalculateLight(light.Base, lightDir, normal);
		}
		
		float attenuation = light.Constant + (light.Linear * dist) * (light.Exp * dist) * (dist * dist);
		
		return (color / attenuation);
	}
	
	vec4 CalculateSpotLight(SpotLight light, vec3 normal)
	{
		vec3 lightDir = normalize(light.Position - FragPos);
		
		float theta = dot(lightDir, normalize(CalculateDirection(light.Direction)));
		
		if (theta > light.Cutoff)
		{
			float dist = length(lightDir);
			
			vec4 color = vec4(0.0f);
		
			if (light.Distance > dist)
			{
				color = CalculateLight(light.Base, lightDir, normal);
			}
			
			float attenuation = light.Constant + (light.Linear * dist) * (light.Exp * dist *dist);
			float spotLightIntensity = (1.0 - (1.0 - theta) / (1.0 - light.Cutoff));
			
			vec4 result = (color / attenuation);
			result.w = 1.0f;
			return result;
		}
	}
	
	uniform vec4 ColourTest;
	
	void main()
	{
		
		//Directionalasde
		vec4 result = CalculateDirectionalLight(Normal);
		
		//Point
		for (int i = 0; i < Actual_Point; i++)
		{
			result += CalculatePointLight(Light_Point[i], Normal);
		}
		
		//Spot
		for (int i = 0; i < Actual_Spot; i++)
		{
			result += CalculateSpotLight(Light_Spot[i], Normal);
		}
		
		
		vec3 texDiffCol = texture2D(albedo_texture, TextureCoords).rgb;
		if (length(texDiffCol) != 0.0)
		{
			FragColor = texture(albedo_texture, TextureCoords) * result * ColourTest;
		}
		else
		{
			FragColor = result * ColourTest;
		}
	}
#endif

