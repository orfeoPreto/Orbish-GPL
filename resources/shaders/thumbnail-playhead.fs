

uniform float totalScope;
uniform float windowForLog;
uniform vec2 resolution;
uniform float offset;
uniform bool reverse;
uniform vec2  origin;
uniform vec3  waveColour;
uniform vec3  bgColour;


void main()
{
    vec2 pixelLocal = gl_FragCoord.xy - origin;

    // Rounded corner SDF (10px radius)
    float radius = 10.0;
    vec2 p = pixelLocal;
    vec2 sz = resolution;
    vec2 q = abs(p - sz * 0.5) - (sz * 0.5 - vec2(radius));
    float dist = length(max(q, 0.0)) - radius;
    float cornerAlpha = 1.0 - smoothstep(-1.0, 1.0, dist);

    // Outside rounded rect: fully transparent (shows parent bg from clear)
    if (cornerAlpha < 0.01) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    // Playhead intensity
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

    // Composite: inner bg + playhead overlay, masked by rounded corners
    // The bg fill covers the cleared outer bg inside the rounded rect
    vec3 base = bgColour;
    // Blend playhead tint on top of bg
    float playheadAlpha = 1.0 - intensity;
    vec3 color = mix(base, waveColour, playheadAlpha);
    gl_FragColor = vec4(color, cornerAlpha);
}
