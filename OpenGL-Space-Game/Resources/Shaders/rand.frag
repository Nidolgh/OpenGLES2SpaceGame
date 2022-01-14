precision highp float;

////////////////////////////////////////////////////////////////////////////////
//  Value Noise 3D
////////////////////////////////////////////////////////////////////////////////

varying vec4 rand_col;

void main()
{
    //vec3 normal = fNormal;
    //  float f = turbulance(wPosition, kFreq);
    //  float fx = turbulance(wPosition + vec3(kEps, 0.0, 0.0), kFreq); 
    //  float fy = turbulance(wPosition + vec3(0.0, kEps, 0.0), kFreq); 
    //  float fz = turbulance(wPosition + vec3(0.0, 0.0, kEps), kFreq);
    
    //  vec3 dn = vec3(fx - f, fy - f, fz - f) * (1.0 / kEps) * 0.05;
    
    //  normal = normalize(normal - dn);  
    //  vec2 bp = blinnPhongDir(normal, lightDir, lightInt, Ka, Kd, Ks, shininess);
    
    //  vec3 color = bp.x * kDiffuse + vec3(bp.y);  
    
    gl_FragColor = rand_col;
}