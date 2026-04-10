uniform vec2 resolution;
uniform float offset;
uniform vec2  origin;
uniform vec3  waveColour;


void main(){
    vec2 pixelLocal = gl_FragCoord.xy - origin;
    vec2 position = pixelLocal.xy / resolution;
    // Gradient from accent gold at bottom to white at top
    vec3 colour = mix(waveColour, vec3(1.0, 1.0, 1.0), position.y);
    float intensity = (offset - position.y)<0?0:1;
    gl_FragColor = vec4(colour, intensity);
}
