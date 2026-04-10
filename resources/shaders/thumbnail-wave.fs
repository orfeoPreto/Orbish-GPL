

uniform float  totalScope;
uniform float windowForLog;
uniform vec2  resolution;
uniform vec2  origin;
uniform vec3  waveColour;

uniform float audioSampleData[1000];

void getAmplitudeForXPos (in float xPos,  out float audioAmplitude)
{
   int k=0;
   int c=1;
   float perfectSamplePosition = ((999.0 * xPos / resolution.x) * c)+k;
   int leftSampleIndex = int (floor (perfectSamplePosition));
   int rightSampleIndex = int (ceil (perfectSamplePosition));
   audioAmplitude = mix (audioSampleData[leftSampleIndex], audioSampleData[rightSampleIndex], fract (perfectSamplePosition));
}

void main()
{
    vec2 pixelLocal = gl_FragCoord.xy - origin;
    float y = pixelLocal.y / resolution.y;
    float amplitude = 0.0;
    getAmplitudeForXPos (pixelLocal.x, amplitude);

    amplitude = 0.5 - amplitude / 2.0;
    float r = abs (0.01 / (amplitude-y));

    float normalizedY = y - 0.5;
    float normalizedAmplitude = amplitude - 0.5;
    float intensity = 0;
    float coeff  =0;
    if(abs(normalizedY) <= abs(normalizedAmplitude+0.001)){
        if (windowForLog < 0) {
            intensity = 1;
            coeff = 0.5;
        }else{
            float tmp = abs(normalizedY/normalizedAmplitude+0.001);
            coeff = windowForLog *.1;
            intensity = pow(tmp,.3/(coeff + 0.0001));
            /*
    //        float tmp2 = windowForLog;
    //        float broadScope = tmp * pow(10,tmp2);
    //        float logResult = log(broadScope)/ log(10);
    //        intensity = (logResult + tmp2) / tmp2 * 2;
             */
        }
    }else{
        intensity = 0;
    }
    // Horizontal gradient fade: soft edges at left/right extremities
    float nx = pixelLocal.x / resolution.x; // 0..1 across width
    float fadeIn  = smoothstep(0.0, 0.15, nx);       // 0→1 over first 15%
    float fadeOut = smoothstep(0.0, 0.15, 1.0 - nx); // 1→0 over last 15%
    float hFade = fadeIn * fadeOut * 0.85 + 0.15;     // range: 0.15 .. 1.0

    // Rounded corner mask (10px radius)
    float radius = 10.0;
    vec2 p = pixelLocal;
    vec2 sz = resolution;
    vec2 q = abs(p - sz * 0.5) - (sz * 0.5 - vec2(radius));
    float dist = length(max(q, 0.0)) - radius;
    float cornerAlpha = 1.0 - smoothstep(-1.0, 1.0, dist);

    gl_FragColor = vec4(waveColour.r, waveColour.g - (.45-coeff *.8), waveColour.b, intensity * hFade * cornerAlpha);
}
