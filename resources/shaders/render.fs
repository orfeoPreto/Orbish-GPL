            uniform sampler2D  tex;
            varying vec2 outUV;
            void main()
            {
                gl_FragColor = texture2D(tex, outUV);
            }