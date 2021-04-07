uniform vec2 resolution;
uniform float offset;

float circleShape(vec2 position, float radius){
    return step(radius, length(position - vec2(0.5)));
}

void main(){
    vec2 position = gl_FragCoord.xy / resolution;
    vec3 colour = vec3(0.9921875, 0.8398438, 0.0585938);
    float rest = fract(offset);
    rest = pow(rest,3);
    float circle =1.0;
    if(rest > 0.01){
        circle = circleShape(position, rest*.1);
    }
    gl_FragColor = vec4(colour, 1-circle);
}
