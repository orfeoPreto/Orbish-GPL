uniform vec2 resolution;
uniform float offset;
uniform vec2  origin;
uniform float downbeat;
uniform vec3  waveColour;

float circleShape(vec2 position, float radius){
    return step(radius, length(position - vec2(0.5)));
}

void main(){
    vec2 pixelLocal = gl_FragCoord.xy - origin;

    vec2 position = pixelLocal.xy / resolution;
    vec3 colour = waveColour;

    float rest = fract(offset);
    rest = pow(rest,3);
    float scale = 1.0 + downbeat;
    float circle =1.0;
    if(rest > 0.01){
        circle = circleShape(position, rest*.1*scale);
    }
    gl_FragColor = vec4(colour, 1-circle);
}
