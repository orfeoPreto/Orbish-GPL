

uniform float totalScope;
uniform float windowForLog;
uniform vec2 resolution;
uniform float offset;
uniform bool reverse;

void main()
{
    float ratio = totalScope / resolution.x;
    float position;
    float flip = (reverse)?1:-1;
    if (totalScope == 0) {
        position = 0;
    }else{
        float tmp = offset;
        if (!reverse) {
            tmp = totalScope - offset;
        }
        position = tmp / ratio / resolution.x;
    }
    float newX = gl_FragCoord.x / resolution.x;

    float diff = position - newX;
    float intensity = 0;
    if (diff * flip < 0){
        intensity = 1;
    }else{
        int window = int(ceil(windowForLog))+1;
        float broadScope = abs(diff) * pow(10,window);
        float logResult = log(broadScope)/ log(10);
        intensity = (logResult + window) / (window * 2);
    }
     gl_FragColor = vec4 (0.9921875, 0.8398438, 0.0585938, 1-intensity);
}
