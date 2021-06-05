uniform vec2 resolution;
uniform float offset;
uniform vec2  origin;


void main(){
    vec2 pixelLocal = gl_FragCoord.xy - origin;
    vec2 position = pixelLocal.xy / resolution;
    vec3 colour = vec3(pow(position.y,0.5), 1-min(1,pow(position.y,4)), 0.0585938);
    float intensity = (offset - position.y)<0?0:1;
    gl_FragColor = vec4(colour, intensity);
}
