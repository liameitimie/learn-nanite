#pragma once
#include <vec_math.h>
#include <math.h>


struct Camera{
    vec3 position;
    f32 pitch;
    f32 yaw;

    f32 move_speed=0.05;
    f32 rotate_speed=0.07;

    static f32 radians(f32 angle){
        return angle*pi/180;
    }
    vec3 direction(){
        f32 rp=radians(pitch);
        f32 ry=radians(yaw);
        
        return vec3{
            .x=f32(cos(rp)*cos(ry)),
            .y=f32(sin(rp)),
            .z=f32(cos(rp)*sin(ry))
        };
    }

    // vec3 direction(f32 yaw,f32 pitch){
    //     f32 rp=radians(pitch);
    //     f32 ry=radians(yaw);
        
    //     return vec3{
    //         .x=f32(cos(rp)*cos(ry)),
    //         .y=f32(sin(rp)),
    //         .z=f32(cos(rp)*sin(ry))
    //     };
    // }

    void move_front(f32 tick_time){
        position+=direction()*tick_time*move_speed;
    }
    void move_right(f32 tick_time){
        vec3 up={0,1,0};
        vec3 front=direction();
        vec3 right=normalize(cross(front,up));
        position+=right*tick_time*move_speed;
        //position+=direction(0,yaw+90)*tick_time*move_speed;
    }
    void rotate_view(f32 yaw_offset,f32 pitch_offset){
        yaw+=yaw_offset*rotate_speed;
        pitch+=-pitch_offset*rotate_speed;
        if(pitch>89) pitch=89;
        if(pitch<-89) pitch=-89;
    }

    mat4 view_mat(){
        vec3 up={0,1,0};
        vec3 front=direction();
        vec3 right=normalize(cross(front,up));
        up=cross(right,front);
        
        mat4 rotate;
        rotate.set_row(0,vec4(right,0));
        rotate.set_row(1,vec4(up,0));
        rotate.set_row(2,vec4(-front,0));
        rotate.set_row(3,vec4(0,0,0,1));

        mat4 move;
        move.set_col(0,vec4(1,0,0,0));
        move.set_col(1,vec4(0,1,0,0));
        move.set_col(2,vec4(0,0,1,0));
        move.set_col(3,vec4(-position,1));

        return mul(rotate,move);
    }
    mat4 projection_mat(f32 fov,f32 aspect){
        // assuming n=-0.1, f=-inf
        f32 t=1/tan(fov*0.5);
        mat4 mat;
        mat.set_col(0,vec4(-t/aspect,0,0,0));
        mat.set_col(1,vec4(0,t,0,0));
        mat.set_col(2,vec4(0,0,0,1));
        mat.set_col(3,vec4(0,0,-0.1,0));
        return mat;
    }
};