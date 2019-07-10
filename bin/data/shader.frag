#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D tex0;
uniform vec2 res;

void main()
{   
    vec2 uv = gl_FragCoord.xy/res.xy;
    vec4 color = texture2D(tex0,uv);
    gl_FragColor = vec4(vec3(1., 200./255., 6./255.), 1.-step(color.w, 0.2));
}