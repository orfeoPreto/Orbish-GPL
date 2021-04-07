            attribute vec3 position;
            attribute vec2 UV;
            varying vec2 outUV;
            
            void main()
            {
                gl_Position = vec4(position, 1.0);
                outUV=UV;
            }
