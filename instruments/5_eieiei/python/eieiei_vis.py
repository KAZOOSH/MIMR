#
# EIEIEI Visualization using
#
# Raspberry Pi 3d demo using OpenGLES 2.0 via Python
#

import socket



import ctypes
import time
import math
# Pick up our constants extracted from the header files with prepare_constants.py
from egl import *
from gl2 import *
from gl2ext import *

print "Starting Eieiei Visualization"


# Define verbose=True to get debug messages
verbose = True

# Define some extra constants that the automatic extraction misses
EGL_DEFAULT_DISPLAY = 0
EGL_NO_CONTEXT = 0
EGL_NO_DISPLAY = 0
EGL_NO_SURFACE = 0
DISPMANX_PROTECTION_NONE = 0

# Open the libraries
bcm = ctypes.CDLL('libbcm_host.so')
opengles = ctypes.CDLL('libGLESv2.so')
openegl = ctypes.CDLL('libEGL.so')

eglint = ctypes.c_int

eglshort = ctypes.c_short


def mapValue(value, leftMin, leftMax, rightMin, rightMax):
    # Figure out how 'wide' each range is
    value = min(value, leftMax)

    leftSpan = leftMax - leftMin
    rightSpan = rightMax - rightMin

    # Convert the left range into a 0-1 range (float)
    valueScaled = float(value - leftMin) / float(leftSpan)

    # Convert the 0-1 range into a value in the right range.
    return rightMin + (valueScaled * rightSpan)


def eglints(L):
    """Converts a tuple to an array of eglints (would a pointer return be better?)"""
    return (eglint*len(L))(*L)

eglfloat = ctypes.c_float

def eglfloats(L):
    return (eglfloat*len(L))(*L)
                
def check(e):
    """Checks that error is zero"""
    if e==0: return
    if verbose:
        print 'Error code',hex(e&0xffffffff)
    raise ValueError

class EGL(object): 

    def __init__(self,depthbuffer=False):
        """Opens up the OpenGL library and prepares a window for display"""
        b = bcm.bcm_host_init()
        assert b==0
        self.display = openegl.eglGetDisplay(EGL_DEFAULT_DISPLAY)
        assert self.display
        r = openegl.eglInitialize(self.display,0,0)
        assert r
        if depthbuffer:
            attribute_list = eglints(     (EGL_RED_SIZE, 8,
                                      EGL_GREEN_SIZE, 8,
                                      EGL_BLUE_SIZE, 8,
                                      EGL_ALPHA_SIZE, 8,
                                      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                                      EGL_DEPTH_SIZE, 16,
                                      EGL_NONE) )
        else:
            attribute_list = eglints(     (EGL_RED_SIZE, 8,
                                      EGL_GREEN_SIZE, 8,
                                      EGL_BLUE_SIZE, 8,
                                      EGL_ALPHA_SIZE, 8,
                                      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                                      EGL_NONE) )
        # EGL_SAMPLE_BUFFERS,  1,
        # EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                                                                    
        numconfig = eglint()
        config = ctypes.c_void_p()
        r = openegl.eglChooseConfig(self.display,
                                     ctypes.byref(attribute_list),
                                     ctypes.byref(config), 1,
                                     ctypes.byref(numconfig));
        assert r
        r = openegl.eglBindAPI(EGL_OPENGL_ES_API)
        assert r
        if verbose:
            print 'numconfig=',numconfig
        context_attribs = eglints( (EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE) )
        self.context = openegl.eglCreateContext(self.display, config,
                                        EGL_NO_CONTEXT,
                                        ctypes.byref(context_attribs))
        assert self.context != EGL_NO_CONTEXT
        width = eglint()
        height = eglint()
        s = bcm.graphics_get_display_size(0,ctypes.byref(width),ctypes.byref(height))
        self.width = width
        self.height = height
        assert s>=0
        dispman_display = bcm.vc_dispmanx_display_open(0)
        dispman_update = bcm.vc_dispmanx_update_start( 0 )
        dst_rect = eglints( (0,0,600,600) )
        #dst_rect = eglints( (0,0,width.value,height.value) )
        #src_rect = eglints( (0,0,width.value<<16, height.value<<16) )
        src_rect = eglints( (0,0,480<<16, 480<<16) )
        assert dispman_update
        assert dispman_display
        dispman_element = bcm.vc_dispmanx_element_add ( dispman_update, dispman_display,
                                  0, ctypes.byref(dst_rect), 0,
                                  ctypes.byref(src_rect),
                                  DISPMANX_PROTECTION_NONE,
                                  0 , 0, 0)
        bcm.vc_dispmanx_update_submit_sync( dispman_update )

        #test res
        nativewindow = eglints((dispman_element,width,height));
        nw_p = ctypes.pointer(nativewindow)
        self.nw_p = nw_p
        self.surface = openegl.eglCreateWindowSurface( self.display, config, nw_p, 0)
        assert self.surface != EGL_NO_SURFACE
        r = openegl.eglMakeCurrent(self.display, self.surface, self.surface, self.context)
        assert r

class demo():

    def showlog(self,shader):
        """Prints the compile log for a shader"""
        N=1024
        log=(ctypes.c_char*N)()
        loglen=ctypes.c_int()
        opengles.glGetShaderInfoLog(shader,N,ctypes.byref(loglen),ctypes.byref(log))
        print log.value

    def showprogramlog(self,shader):
        """Prints the compile log for a program"""
        N=1024
        log=(ctypes.c_char*N)()
        loglen=ctypes.c_int()
        opengles.glGetProgramInfoLog(shader,N,ctypes.byref(loglen),ctypes.byref(log))
        print log.value
            
    def __init__(self):
        self.vertex_data = eglfloats((-1.0,-1.0,1.0,1.0,
                         1.0,-1.0,1.0,1.0,
                         1.0,1.0,1.0,1.0,
                         -1.0,1.0,1.0,1.0))
        self.vshader_source = ctypes.c_char_p(
              "attribute vec4 vertex;"
              "varying vec2 tcoord;"
              "void main(void) {"
              "  vec4 pos = vertex;"
              "  pos.xy*=0.9;"
              "  gl_Position = pos;"
              "  tcoord = vertex.xy*0.5+0.5;"
              "}")
      
        self.fshader_source = ctypes.c_char_p(
              "uniform vec4 color;"
              "void main(void) {"
              "   gl_FragColor = color;"
              "}")

        # Julia
        julia_fshader_source = ctypes.c_char_p("""
            #define AA 4.

#define PURPLE vec3(0.64,0,0.69)
#define PINK vec3(1,0,0.59)
#define TURQUOISE vec3(0,0.73,0.83) 
#define BLUE vec3(0,0.38,0.68) 

	uniform vec4 color;
	uniform vec2 scale;
	uniform vec2 offset;
	varying vec2 tcoord;

uniform float iTime;
uniform float depth;
uniform float isIdle;

#define DSP_STR 1.5
#define PI_HALF 1.57079632679


float getsat(vec3 c)
{
    float mi = min(min(c.x, c.y), c.z);
    float ma = max(max(c.x, c.y), c.z);
    return (ma - mi)/(ma+ 1e-7);
}

//Improved rgb lerp
vec3 iLerp(in vec3 a, in vec3 b, in float x)
{
    //Interpolated base color (with singularity fix)
    vec3 ic = mix(a, b, x) + vec3(1e-6,0.,0.);
    
    //Saturation difference from ideal scenario
    float sd = abs(getsat(ic) - mix(getsat(a), getsat(b), x));
    
    //Displacement direction
    vec3 dir = normalize(vec3(2.*ic.x - ic.y - ic.z, 2.*ic.y - ic.x - ic.z, 2.*ic.z - ic.y - ic.x));
    //Simple Lighntess
    float lgt = dot(vec3(1.0), ic);
    
    //Extra scaling factor for the displacement
    float ff = dot(dir, normalize(ic));
    
    //Displace the color
    ic += DSP_STR*dir*sd*ff*lgt;
    return clamp(ic,0.,1.);
}
vec3 vLerp(float x){
    x = abs(x);

    if (x < 0.25){
        return iLerp(PURPLE, PINK, x*4.0);
    }
    else if (x < 0.5){
        return iLerp(PINK, TURQUOISE, (x-0.25)*4.0);
    }
    else if (x < 0.75){
        return iLerp(TURQUOISE, BLUE, (x-0.5)*4.0);
    }
    else{
        return iLerp(BLUE, PURPLE, (x-0.75)*4.0);
    }
}


float metaball(vec2 p, float r)
{
    return r / dot(p, p);
}

vec3 samplef(in vec2 uv)
{
    float t0x = -0.2 + cos(iTime) * .3;
    float t0y = -0.2 + cos(iTime * 2.4) * .3;
    float t1x = -0.7 + sin(iTime * .5) * 0.25;
    float t1y = -0.7 + sin(iTime * .3) * 0.35;
    //float t2x = -0.1 + cos(iTime * 0.1) * 0.9; //grosse kurve
    //float t2y = - 0.1 +sin(iTime * 0.05) * 0.88;
    float t3x = 0.8 + cos(iTime * 1.5) * 0.33; 
    float t3y = 0.2 +sin(iTime * 2.0) * 0.3;
    float t4x = 0.5 + cos(iTime * 0.8) * 0.2; 
    float t4y = 0.7 +sin(iTime * 0.6) * 0.2;
    //float t6y = -0.6 +cos(iTime * 0.2) * 0.5;
    //float t5x = 1.0 + cos(iTime * 0.7) * 0.3; //ausserhalb
    //float t5y = -1.0 +sin(iTime * 0.3) * 0.3;

    //float d = 1.0-(depth/127.0) +1.0;
    float d = depth/127.0 +1.0;

    float r = metaball(uv + vec2(t0x, t0y), .02 *d) * //unten rechts
              metaball(uv + vec2(t1x, t1y), .07*d)+

              metaball(uv + vec2(t3x, t3y), .1*d) * // oben links
              metaball(uv + vec2(t4x, t4y), .05*d)+

              metaball(uv + vec2(t0x, t4x), .07*d) * // oben rechts
              metaball(uv + vec2(t1y, t4y), .12*d) +

              metaball(uv + vec2(t3y, t0x), .04*d) * //unten links
              metaball(uv + vec2(t3x, t1y), .09*d) +
              metaball(uv + vec2(t3x, t4y), .03*d);// +

              //metaball(uv + vec2(t5y, t5y), .06*d) +

              //metaball(uv + vec2(t5x, t5x), .05*d) +

              //metaball(uv + vec2(t2x, t2y), .04*d);

    
    

    //vec3 bg = vLerp( cos(iTime/10.0));
    //vec3 bg = vLerp( cos(depth*PI_HALF));
    vec3 bg = vLerp( depth*0.9/127.0);

    vec3 col = vec3(0.0,1.0,0.0);//vLerp(sin(iTime/9.0));


    vec3 c = bg;

    

    if (r>.3) c = 0.4 * col;
    if (r>.4) c = bg;
    if (r>.43) c = 0.6 * col;
    if (r>.53) c = bg;
    if (r>.56) c = 0.8 * col;
    if (r>.66) c = bg;
    if (r>.69) c = col;
    if (r>.9) c = bg;

    return c;
}

	uniform sampler2D tex;
	void main(void) {
    vec2 fragCoord = vec2(gl_FragCoord.x,gl_FragCoord.y);
    vec2 iResolution = vec2(480.0,480.0);

    if(fragCoord.x > iResolution.x || fragCoord.y > iResolution.y ){
        gl_FragColor = vec4(0.0,0.0,0.0, 1.0);
    }else{

    vec2 uv = (fragCoord.xy / iResolution.xy * 2. - 1.)
            * vec2(iResolution.x / iResolution.y, 1) * 1.25;

    vec3 col = vec3(uv.x,uv.y,0.0);


    col = samplef(uv);  
    
    if(isIdle == 1.0) col*=0.3;
    else col -= isIdle*0.2;

    col = clamp(col,0.0,1.0);
    
    gl_FragColor = vec4(col, 1.0);}
	}""")

        vshader = opengles.glCreateShader(GL_VERTEX_SHADER);
        opengles.glShaderSource(vshader, 1, ctypes.byref(self.vshader_source), 0)
        opengles.glCompileShader(vshader);

        if verbose:
            self.showlog(vshader)
            
        fshader = opengles.glCreateShader(GL_FRAGMENT_SHADER);
        opengles.glShaderSource(fshader, 1, ctypes.byref(julia_fshader_source), 0);
        opengles.glCompileShader(fshader);

        if verbose:
            self.showlog(fshader)

        program = opengles.glCreateProgram();
        opengles.glAttachShader(program, vshader);
        opengles.glAttachShader(program, fshader);
        opengles.glLinkProgram(program);

        if verbose:
            self.showprogramlog(program)
            
        self.program = program
        self.unif_color = opengles.glGetUniformLocation(program, "color");
        self.attr_vertex = opengles.glGetAttribLocation(program, "vertex");
        self.unif_time = opengles.glGetUniformLocation(program, "iTime");
        self.unif_depth = opengles.glGetUniformLocation(program, "depth");
        self.unif_idle = opengles.glGetUniformLocation(program, "isIdle");
        self.unif_tex = opengles.glGetUniformLocation(program, "tex");
        

        opengles.glClearColor ( eglfloat(0.0), eglfloat(1.0), eglfloat(1.0), eglfloat(1.0) );
        
        self.buf=eglint()
        opengles.glGenBuffers(1,ctypes.byref(self.buf))

        self.check()

        # Prepare a texture image
        self.tex=eglint()
        self.check()
        opengles.glGenTextures(1,ctypes.byref(self.tex))
        self.check()
        opengles.glBindTexture(GL_TEXTURE_2D,self.tex)
        self.check()
        # opengles.glActiveTexture(0)
        #test_tex=(eglshort*(1920*1080))(*([3567]*20000))
        #test_tex_p = ctypes.pointer(test_tex)
        #self.store=[test_tex,test_tex_p]
        #opengles.glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,1920,1080,0,GL_RGB,GL_UNSIGNED_SHORT_5_6_5,0)
        #opengles.glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,800,600,0,GL_RGB,GL_UNSIGNED_SHORT_5_6_5,0)
        #opengles.glTexImage2D(GL_TEXTURE_2D,0,1920,1080,0,GL_RGB,GL_UNSIGNED_BYTE,0)
        self.check()
        opengles.glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, eglfloat(GL_NEAREST))
        opengles.glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, eglfloat(GL_NEAREST))
        self.check()
        # Prepare a framebuffer for rendering
        self.tex_fb=eglint()
        opengles.glGenFramebuffers(1,ctypes.byref(self.tex_fb))
        self.check()
        opengles.glBindFramebuffer(GL_FRAMEBUFFER,self.tex_fb)
        self.check()
        opengles.glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,self.tex,0)
        self.check()
        opengles.glBindFramebuffer(GL_FRAMEBUFFER,0)
        self.check()
        # Prepare viewport
        opengles.glViewport ( 0, 0, egl.width, egl.height );
        self.check()
        
        # Upload vertex data to a buffer
        opengles.glBindBuffer(GL_ARRAY_BUFFER, self.buf);
        opengles.glBufferData(GL_ARRAY_BUFFER, ctypes.sizeof(self.vertex_data),
                             ctypes.byref(self.vertex_data), GL_STATIC_DRAW);
        opengles.glVertexAttribPointer(self.attr_vertex, 4, GL_FLOAT, 0, 16, 0);
        opengles.glEnableVertexAttribArray(self.attr_vertex);
        self.check()
        
    def draw_triangles(self,dist,isIdle):

        inIdle = isIdle
        if time.time() - lastActive < 0.25:
            p = (time.time() - lastActive)*4*2*math.pi
            inIdle = -math.cos(p);
        #print inIdle

        # Now render to the main frame buffer
        opengles.glBindFramebuffer(GL_FRAMEBUFFER,0)
        # Clear the background (not really necessary I suppose)
        opengles.glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        self.check()
        opengles.glBindBuffer(GL_ARRAY_BUFFER, self.buf);
        self.check()
        opengles.glUseProgram ( self.program );
        self.check()
        opengles.glBindTexture(GL_TEXTURE_2D,self.tex)
        self.check()
        opengles.glUniform4f(self.unif_color, eglfloat(0.5), eglfloat(0.5), eglfloat(0.8), eglfloat(1.0));
        self.check()
        opengles.glUniform1f(self.unif_time, eglfloat(time.time()-1506193010.0));
        self.check()
        opengles.glUniform1f(self.unif_depth, eglfloat(dist));
        self.check()
        opengles.glUniform1f(self.unif_idle, eglfloat(inIdle));
        self.check()
        opengles.glUniform1i(self.unif_tex, 0); # I don't really understand this part, perhaps it relates to active texture?
        self.check()
        
        opengles.glDrawArrays ( GL_TRIANGLE_FAN, 0, 4 );
        self.check()

        opengles.glBindBuffer(GL_ARRAY_BUFFER, 0);

        opengles.glFlush()
        opengles.glFinish()
        self.check()
        
        openegl.eglSwapBuffers(egl.display, egl.surface);
        self.check()      
        
    def check(self):
        e=opengles.glGetError()
        if e:
            print hex(e)
            raise ValueError
        
def showerror():
    e=opengles.glGetError()
    print hex(e)
    
if __name__ == "__main__":

    # open UDP socket to listen other script
    udpIn = socket.socket( socket.AF_INET, socket.SOCK_DGRAM)
    udpIn.bind( ('', 5013 ) )
    udpIn.settimeout(0.1)

    egl = EGL()
    d = demo()

    #tl = 0
    distance = 0.0
    isIdle = 1
    lastActive = 0
    while 1:
        #t = time.time()-tl;
        #tl = time.time();
        #print(1/t);
        try:
            #receive values
            data, addr = udpIn.recvfrom(1024)
            #print data
            value = data.split(":")
            #print value   


            distance = float(value[0]) #mapValue(float(data),0,30,0,128);
            
            if isIdle == 1 and int(value[1]) == 0:
                lastActive = time.time();

            isIdle = int(value[1])
            #print distance

        except Exception:
            pass

        #draw vis
        d.draw_triangles(distance,isIdle)
    showerror()


        
    
