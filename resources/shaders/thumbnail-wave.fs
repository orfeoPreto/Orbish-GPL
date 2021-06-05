

uniform float  totalScope;
uniform float windowForLog;
uniform vec2  resolution;
uniform vec2  origin;

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
    
    float coeff = 0;
    if(abs(normalizedY) <= abs(normalizedAmplitude+0.001)){
        float tmp = abs(normalizedY/normalizedAmplitude+0.001);
        coeff = windowForLog *.1;
        intensity = pow(tmp,.3/(coeff + 0.0001));
        /*
//        float tmp2 = windowForLog;
//        float broadScope = tmp * pow(10,tmp2);
//        float logResult = log(broadScope)/ log(10);
//        intensity = (logResult + tmp2) / tmp2 * 2;
         */
    }else{
        intensity = 0;
    }
     gl_FragColor = vec4 (0.9921875, 0.8398438 - (.45-coeff *.8) , 0.0585938  , intensity);
}
