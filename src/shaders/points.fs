void main()
{
    if (gl_Color.a == 0) discard;
    gl_FragColor = gl_Color;
}
