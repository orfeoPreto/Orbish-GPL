

uniform float totalScope;
uniform float windowForLog;
uniform vec2 resolution;
uniform float offset;
uniform bool reverse;
uniform vec2  origin;


void main()
{
    vec2 pixelLocal = gl_FragCoord.xy - origin;

    float ratio = totalScope / resolution.x;
    float position;
    float flip = (reverse)?1:-1;
    float intensity = 1;

    if (totalScope != 0) {
        float tmp = (!reverse)?totalScope - offset:offset;
        position = tmp / ratio / resolution.x;
        float newX = pixelLocal.x / resolution.x;

        float diff = position - newX;
        if (diff * flip < 0){
            intensity = 1;
        }else{
            int window = int(ceil(windowForLog))+1;
            float broadScope = abs(diff) * pow(10,window);
            float logResult = log(broadScope)/ log(10);
            intensity = (logResult + window) / (window * 2);
        }
    }
     gl_FragColor = vec4 (0.9921875, 0.8398438, 0.0585938, 1-intensity);
}
