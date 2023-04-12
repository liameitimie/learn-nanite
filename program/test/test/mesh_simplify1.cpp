#include "mesh_simplify1.h"
#include <stdio.h>
#include <stdlib.h>
#include <types.h>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <assert.h>
#include <random>
#include <iostream>
#include <vector>
#include <vec_math.h>

using namespace std;

u64 MurmurAdd(u64 Hash,u64 Element){
	Element*=0xcc9e2d51;
	Element=(Element<<15)|(Element>>(32-15));
	Element*=0x1b873593;

	Hash^=Element;
	Hash=(Hash<<13)|(Hash>>(32-13));
	Hash=Hash*5+0xe6546b64;
	return Hash;
}

u64 MurmurMix(u64 Hash){
	Hash^=Hash>>16;
	Hash*=0x85ebca6b;
	Hash^=Hash>>13;
	Hash*=0xc2b2ae35;
	Hash^=Hash>>16;
	return Hash;
}

Mesh simplify(Mesh& mesh,u32 target_faces){
    auto positions=mesh.positions;
    auto indices=mesh.indices;
    int v_cnt=positions.size();
    int f_cnt=indices.size()/3;

    vector<mat4> Q(v_cnt); //顶点误差矩阵
    for(int i=0;i<f_cnt;i++){
        vec3 v1=positions[indices[i*3]],v2=positions[indices[i*3+1]],v3=positions[indices[i*3+2]];

        vec3 n=normalize(cross(v2-v1,v3-v1));
        auto[a,b,c]=n;
        f32 d=-(a*v1.x+b*v1.y+c*v1.z); //计算平面表达式

        vec4 p={a,b,c,d}; //平面对应误差矩阵
        mat4 Kp;
        Kp.set_col(0,p*a),Kp.set_col(1,p*b);
        Kp.set_col(2,p*c),Kp.set_col(3,p*d);
        Q[indices[i*3]]+=Kp,Q[indices[i*3+1]]+=Kp,Q[indices[i*3+2]]+=Kp;
    }

    auto try_insert=[](vector<u32>& v,u32 x)->bool{
        auto it=lower_bound(v.begin(),v.end(),x);
        if(it==v.end()||*it!=x){v.insert(it,x);return true;}
        else return false;
    };
    auto try_erase=[](vector<u32>& v,u32 x)->bool{
        auto it=lower_bound(v.begin(),v.end(),x);
        if(it!=v.end()&&*it==x){v.erase(it);return true;}
        else return false;
    };

    vector<vector<u32>> G(v_cnt); //网格顶点图的邻接表,表内按编号排序
    for(int i=0;i<f_cnt;i++){
        u32 i1=indices[i*3],i2=indices[i*3+1],i3=indices[i*3+2];
        try_insert(G[i1],i2),try_insert(G[i1],i3);
        try_insert(G[i2],i1),try_insert(G[i2],i3);
        try_insert(G[i3],i1),try_insert(G[i3],i2);
    }

    vector<u32> fa(v_cnt); //并查集，记录节点合并到哪个节点，只能从编号小合并到编号大
    for(int i=0;i<v_cnt;i++) fa[i]=i;
    auto find=[&fa](u32 x)->u32{
        u32 y=x,z;
        while(fa[x]!=x) x=fa[x];
        while(fa[y]!=y) z=fa[y],fa[y]=x,y=z;
        return x;
    };

    struct CollapsingEdge{
        f32 error; //误差
        vec3 v;  //最佳位置
        uvec2 e; //要合并的边，x<y

        bool operator<(const CollapsingEdge& y)const{
            if(error!=y.error) return error<y.error;
            else return e<y.e;
        }
    };
    //给定需要合并的x,y两顶点，计算误差
    auto get_approx_error=[&Q,&positions](u32 x,u32 y)->CollapsingEdge{
        CollapsingEdge res{.e={x,y}};
        mat4 q=Q[x]+Q[y],invq,qt=q;
        qt.set_row(3,vec4{0,0,0,1});
        if(invert(qt,invq)){ //矩阵可逆，取逆矩阵与[0,0,0,1]的乘积为顶点位置
            vec4 v=invq.col[3]; v.w=1;
            res.v={v.x,v.y,v.z};
            res.error=dot(v,mul(q,v));
        }
        else{ //不可逆，从v1,v2,(v1+v2)/2中选error低的
            vec3 v=(positions[x]+positions[y])*0.5f,tv;
            f32 e=dot(vec4(v,1),mul(q,vec4(v,1))),te;
            tv=positions[x],te=dot(vec4(tv,1),mul(q,vec4(tv,1)));
            if(te<e) e=te,v=tv;
            tv=positions[y],te=dot(vec4(tv,1),mul(q,vec4(tv,1)));
            if(te<e) e=te,v=tv;
            res.v=v,res.error=e;
        }
        return res;
    };

    struct hash{
        size_t operator()(const uvec2& v)const{
            return MurmurAdd(v.x,v.y);
        }
    };
    set<CollapsingEdge> s;
    unordered_map<uvec2,set<CollapsingEdge>::iterator,hash> mp;

    for(u32 y=0;y<v_cnt;y++){
        for(u32 x:G[y]){
            if(x>=y) break;
            auto it=s.insert(get_approx_error(x,y)).first;
            mp[{x,y}]=it;
        }
    }

    auto print=[](vector<u32>& v){
        cout<<"[";
        int t=0;
        for(auto x:v){
            if(t) cout<<", ";
            cout<<x;
            t++;
        }
        cout<<"]";
    };

    struct Face{
        u32 i[3];
        Face(u32 i0,u32 i1,u32 i2){
            i[0]=i0,i[1]=i1,i[2]=i2;
            sort(i,i+3);
        }
        bool operator<(const Face& b)const{
            if(i[0]!=b.i[0]) return i[0]<b.i[0];
            if(i[1]!=b.i[1]) return i[1]<b.i[1];
            return i[2]<b.i[2];
        }
    };
    set<Face> deleted_faces;

    u32 faces=f_cnt;
    while(!s.empty()&&faces>target_faces){
        auto it=s.begin();
        auto[error,v,e]=*it;

        // cout<<"\n#### round: ####\n"<<"faces: "<<faces<<endl;
        // cout<<e.x<<' '<<e.y<<endl;
        // cout<<"G[e.x]->G["<<e.x<<"]:";print(G[e.x]);cout<<endl;
        // cout<<"G[e.y]->G["<<e.y<<"]:";print(G[e.y]);cout<<endl;

        assert(fa[e.x]==e.x);
        assert(fa[e.y]==e.y);
        // if(fa[e.x]!=e.x) cout<<"????"<<endl;
        // if(fa[e.y]!=e.y) cout<<"...."<<endl;

        s.erase(it),mp.erase(e);
        Q[e.x]+=Q[e.y];
        positions[e.x]=v;
        fa[e.y]=e.x;

        // cout<<"erasing "<<e.y<<" in ";print(G[e.x]);
        assert(try_erase(G[e.x],e.y));
        // cout<<"yes:";print(G[e.x]);cout<<endl;

        // cout<<"erasing "<<e.x<<" in ";print(G[e.y]);
        assert(try_erase(G[e.y],e.x));
        // cout<<"yes:";print(G[e.y]);cout<<endl;
        // cout<<endl;
        
        for(u32 x:G[e.x]){ //对x的每个邻边更新error
            uvec2 e1=(x<e.x)?uvec2{x,e.x}:uvec2{e.x,x};
            auto it=mp[e1];
            s.erase(it);
            mp[e1]=s.insert(get_approx_error(e1.x,e1.y)).first;
        }

        for(u32 y:G[e.y]){

            // cout<<"::y="<<y<<endl;
            // cout<<"G[y]->G["<<y<<"]:";print(G[y]);cout<<endl;

            uvec2 e1=(y<e.y)?uvec2{y,e.y}:uvec2{e.y,y};
            auto it=mp[e1];
            // cout<<e1.x<<' '<<e1.y<<endl;
            s.erase(it),mp.erase(e1);

            // cout<<"erasing "<<e.y<<" in ";print(G[y]);cout<<endl;
            if(!try_erase(G[y],e.y)) cout<<"!!!"<<endl;
            // else {cout<<"yes:";print(G[y]);cout<<endl;}

            // cout<<"inserting "<<y<<" in ";print(G[e.x]);cout<<endl;

            if(try_insert(G[e.x],y)){
                uvec2 e2=(y<e.x)?uvec2{y,e.x}:uvec2{e.x,y};
                mp[e2]=s.insert(get_approx_error(e2.x,e2.y)).first;

                // cout<<"yes:";print(G[e.x]);cout<<endl;

                // cout<<"inserting "<<e.x<<" in ";print(G[y]);cout<<endl;
                if(try_insert(G[y],e.x)){
                    // cout<<"yes:";print(G[y]);cout<<endl;
                }
            }
            else{
                faces--;
                deleted_faces.insert(Face(y,e.x,e.y));
            }

            // cout<<endl;
        }
        vector<u32>().swap(G[e.y]);
    }
    
    //获取简化后的顶点数组与索引数组
    //vector<u32> d; //离散化顶点编号

    // for(u32& x:indices){
    //     x=find(x);
    // }

    multiset<Face> t1,t2;

    cout<<"delete faces: "<<deleted_faces.size()<<endl;
    for(auto f:deleted_faces){
        u32 i1=find(f.i[0]),i2=find(f.i[1]),i3=find(f.i[2]);
        if(i1!=i2&&i1!=i3&&i2!=i3){
            cout<<"!!!: "<<f.i[0]<<' '<<f.i[1]<<' '<<f.i[2]<<endl;
            cout<<i1<<' '<<i2<<' '<<i3<<endl;
        }
        t1.insert(Face(i1,i2,i3));
    }

    u32 tfaces=f_cnt;
    u32 j=0;
    for(u32 i=0;i<f_cnt;i++){
        u32 i1=find(indices[i*3]),i2=find(indices[i*3+1]),i3=find(indices[i*3+2]);
        if(i1!=i2&&i1!=i3&&i2!=i3){
            //indices[j*3]=i1,indices[j*3+1]=i2,indices[j*3+2]=i3;
            //d.push_back(i1),d.push_back(i2),d.push_back(i3);
            j++;
        }
        else{
            tfaces--;
            t2.insert(Face(i1,i2,i3));
        }
    }

    cout<<t1.size()<<' '<<t2.size()<<endl;

    for(auto f:t1){
        if(t2.find(f)==t2.end()){
            cout<<"!!! 1:"<<endl;
            cout<<"t1.val: "<<f.i[0]<<' '<<f.i[1]<<' '<<f.i[2]<<endl;
        }
    }
    for(auto f:t2){
        if(t1.find(f)==t1.end()){
            cout<<"!!! 2:"<<endl;
            cout<<"t2.val: "<<f.i[0]<<' '<<f.i[1]<<' '<<f.i[2]<<endl;
        }
    }

    //assert(faces==tfaces);

    cout<<j<<' '<<faces<<' '<<tfaces<<endl;

    // indices.resize(faces*3);
    // sort(d.begin(),d.end());
    // d.erase(unique(d.begin(),d.end()),d.end());
    
    // for(u32& i: indices){
    //     i=lower_bound(d.begin(),d.end(),i)-d.begin();
    // }

    // for(u32 i=0,j=0;i<v_cnt;i++,j++){
    //     while(i<v_cnt&&find(i)!=i) i++;
    //     while(j<i&&find(j)==j) j++;
    //     if(i<v_cnt&&j<i) positions[j]=positions[i];
    // }
    // positions.resize(d.size());

    return Mesh{positions,indices};
}

// Mesh simplify(Mesh& mesh,u32 target_faces){
//     auto positions=mesh.positions;
//     auto indices=mesh.indices;
//     int v_cnt=positions.size();
//     int f_cnt=indices.size()/3;

//     // vector<mat4> ttt;
//     // vector<vec4> ttt1;

//     vector<mat4> Q(v_cnt); //顶点误差矩阵
//     for(int i=0;i<f_cnt;i++){
//         vec3 v1=positions[indices[i*3]];
//         vec3 v2=positions[indices[i*3+1]];
//         vec3 v3=positions[indices[i*3+2]];

//         vec3 n=normalize(cross(v2-v1,v3-v1));
//         auto[a,b,c]=n;
//         f32 d=-(a*v1.x+b*v1.y+c*v1.z); //计算平面表达式

//         vec4 p={a,b,c,d}; //平面对应误差矩阵
//         mat4 Kp;
//         Kp.set_col(0,p*a);
//         Kp.set_col(1,p*b);
//         Kp.set_col(2,p*c);
//         Kp.set_col(3,p*d);
//         Q[indices[i*3]]+=Kp;
//         Q[indices[i*3+1]]+=Kp;
//         Q[indices[i*3+2]]+=Kp;

//         // if(indices[i*3]==0||indices[i*3+1]==0||indices[i*3+2]==0){
//         //     ttt.push_back(Kp);
//         //     ttt1.push_back(p);
//         // }
//     }

//     // for(int i=0;i<16;i++){
//     //     vec3 p0=positions[0];
//     //     printf("p0:[%f,%f,%f]\n",p0.x,p0.y,p0.z);
//     //     f32 dx=rand()/(f32)RAND_MAX;
//     //     f32 dy=rand()/(f32)RAND_MAX;
//     //     f32 dz=rand()/(f32)RAND_MAX;
//     //     dx*=0.1,dy*=0.1,dz*=0.1;
//     //     vec3 pt=vec3{p0.x+dx,p0.y+dy,p0.z+dz};
//     //     printf("pt:[%f,%f,%f]\n",pt.x,pt.y,pt.z);

//     //     f32 se=0;
//     //     for(int i=0;i<ttt.size();i++){
//     //         vec4 p=ttt1[i];
//     //         mat4 K=ttt[i];
//     //         vec3 n=vec3{p.x,p.y,p.z};

//     //         f32 e=dot(pt-p0,n);
//     //         //cout<<e<<' '<<e*e<<endl;

//     //         e=dot(p,vec4(pt,1));
//     //         //cout<<e<<' '<<e*e<<endl;

//     //         e=dot(vec4(pt,1),mul(K,vec4(pt,1)));
//     //         //cout<<e<<endl;
//     //         se+=e;
//     //     }
//     //     cout<<se<<endl;
//     //     se=dot(vec4(pt,1),mul(Q[0],vec4(pt,1)));
//     //     cout<<se<<endl<<endl;
//     // }

//     vector<vector<u32>> G(v_cnt); //网格顶点图的邻接表,表内按编号排序
//     for(int i=0;i<f_cnt;i++){
//         u32 i1=indices[i*3];
//         u32 i2=indices[i*3+1];
//         u32 i3=indices[i*3+2];
//         auto try_insert=[&G](u32 x,u32 y){
//             auto it=lower_bound(G[x].begin(),G[x].end(),y);
//             if(it==G[x].end()||*it!=y) G[x].insert(it,y);
//         };
//         try_insert(i1,i2),try_insert(i1,i3);
//         try_insert(i2,i1),try_insert(i2,i3);
//         try_insert(i3,i1),try_insert(i3,i2);
//     }

//     vector<u32> fa(v_cnt); //并查集，记录节点合并到哪个节点，只能从编号小合并到编号大
//     for(int i=0;i<v_cnt;i++) fa[i]=i;
//     auto find=[&fa](u32 x)->u32{
//         u32 y=x,z;
//         while(fa[x]!=x) x=fa[x];
//         while(fa[y]!=y) z=fa[y],fa[y]=x,y=z;
//         return x;
//     };
    
//     struct CollapsingEdge{
//         f32 error; //误差
//         vec3 v;  //最佳位置
//         uvec2 e; //要合并的边，x<y

//         bool operator<(const CollapsingEdge& y)const{
//             return error<y.error;
//         }
//     };
//     //给定需要合并的x,y两顶点，计算误差
//     auto get_approx_error=[&Q,&positions](u32 x,u32 y)->CollapsingEdge{
//         CollapsingEdge res{.e={x,y}};
//         mat4 q=Q[x]+Q[y],invq,qt=q;
//         qt.set_row(3,vec4{0,0,0,1});
//         if(invert(qt,invq)){ //矩阵可逆，取逆矩阵与[0,0,0,1]的乘积为顶点位置
//             vec4 v=invq.col[3]; v.w=1;
//             res.v={v.x,v.y,v.z};
//             res.error=dot(v,mul(q,v));

//             //auto ttt=mul(qt,invq);
//             //assert(res.error>0);
//         }
//         else{ //不可逆，从v1,v2,(v1+v2)/2中选error低的
//             vec3 v=(positions[x]+positions[y])*0.5,tv;
//             f32 e=dot(vec4(v,1),mul(q,vec4(v,1))),te;
//             tv=positions[x],te=dot(vec4(tv,1),mul(q,vec4(tv,1)));
//             if(te<e) e=te,v=tv;
//             tv=positions[y],te=dot(vec4(tv,1),mul(q,vec4(tv,1)));
//             if(te<e) e=te,v=tv;
//             res.v=v,res.error=e;

//             // assert(res.error>0);
//         }
//         return res;
//     };

//     struct hash{
//         size_t operator()(const uvec2& v)const{
//             return MurmurAdd(v.x,v.y);
//         }
//     };
//     multiset<CollapsingEdge> s;
//     unordered_map<uvec2,multiset<CollapsingEdge>::iterator,hash> mp;

//     for(u32 y=0;y<v_cnt;y++){
//         for(u32 x:G[y]){
//             if(x>=y) break;
//             auto it=s.insert(get_approx_error(x,y));
//             mp[{x,y}]=it;
//         }
//     }
    
//     vector<u32> book(v_cnt);
//     u32 faces=f_cnt;

//     int tcnt=0;

//     while(!s.empty()&&faces>target_faces){

//         tcnt++;
//         cout<<"\n\n   ##### round: "<<tcnt<<" #####"<<endl;

//         auto it=s.begin();
//         CollapsingEdge ce=*it;
        
//         assert(find(ce.e.x)==ce.e.x);
//         assert(find(ce.e.y)==ce.e.y);


//         printf("s.erase(%d,%d), mp.erase(%d,%d)\n",it->e.x,it->e.y,ce.e.x,ce.e.y);

//         s.erase(it),mp.erase({ce.e.x,ce.e.y});

//         printf("x:%d->",ce.e.x);
//         for(u32 x: G[ce.e.x]){
//             printf("(%d,%d),",x,find(x));
//         }
//         cout<<endl;
//         printf("y:%d->",ce.e.y);
//         for(u32 y: G[ce.e.y]){
//             printf("(%d,%d),",y,find(y));
//         }
//         cout<<endl<<endl;

//         fa[ce.e.y]=ce.e.x; //合并
//         Q[ce.e.x]+=Q[ce.e.y];
//         positions[ce.e.x]=ce.v;

//         for(u32 x: G[ce.e.x]){ //对x的每个邻边更新error
//             u32 fx=find(x);
//             if(book[fx]==1||fx==ce.e.x) continue;


//             printf("x:%d, fx:%d",x,fx);  cout<<endl;


//             book[fx]=1; //标记此节点
//             uvec2 e=(ce.e.x<fx)?uvec2{ce.e.x,fx}:uvec2{fx,ce.e.x};

//             printf("mp.query(%d,%d)",e.x,e.y); fflush(stdout);

//             auto it=mp[e];

//             printf("->it(%d,%d)",it->e.x,it->e.y); cout<<endl;
//             printf("s.erase(%d,%d)",it->e.x,it->e.y); cout<<endl;


//             s.erase(it);
//             mp[e]=s.insert(get_approx_error(e.x,e.y));


//             printf("s.insert(%d,%d), mp.change\n",e.x,e.y);

//         }

//         cout<<endl;

//         for(u32 y: G[ce.e.y]){
//             u32 fy=find(y);
//             if(book[fy]==2||fy==ce.e.x) continue;


//             printf("y:%d, fy:%d\n",y,fy);


//             uvec2 e=(ce.e.y<fy)?uvec2{ce.e.y,fy}:uvec2{fy,ce.e.y};
//             auto it=mp[e];

//             printf("mp.query(%d,%d)->it(%d,%d)\n",e.x,e.y,it->e.x,it->e.y);
//             printf("s.erase(%d,%d), mp.erase(%d,%d)\n",it->e.x,it->e.y,e.x,e.y);

//             s.erase(it),mp.erase(e);
//             if(book[fy]){ //节点被标记，代表三角形被简化，此边直接删去，不重新进堆
//                 faces--;
//             } else{ //节点未标记，将fy连接到ce.e.x
//                 uvec2 e1=(ce.e.x<fy)?uvec2{ce.e.x,fy}:uvec2{fy,ce.e.x};

//                 if(mp.find(e1)!=mp.end())
//                     printf(" !!!! mp[e1]\n");

//                 mp[e1]=s.insert(get_approx_error(e1.x,e1.y));

//                 printf(" # s.insert(%d,%d), mp.insert(%d,%d)\n",e1.x,e1.y,e1.x,e1.y);

//             }
//             book[fy]=2;
//         }
//         for(u32 x: G[ce.e.x]) book[find(x)]=0; //清空标记数组
//         for(u32 y: G[ce.e.y]) book[find(y)]=0;
//     }
    
//     //获取简化后的顶点数组与索引数组

//     vector<u32> d; //离散化顶点编号

//     u32 tfaces=f_cnt;
//     for(u32 i=0,j=0;i<f_cnt;i++){
//         u32 i1=find(indices[i*3]),i2=find(indices[i*3+1]),i3=find(indices[i*3+2]);
//         if(i1!=i2&&i1!=i3&&i2!=i3){
//             indices[j*3]=i1,indices[j*3+1]=i2,indices[j*3+2]=i3;
//             d.push_back(i1),d.push_back(i2),d.push_back(i3);
//             j++;
//         }
//         else tfaces--;
//     }

//     assert(faces==tfaces);

//     indices.resize(faces*3);
//     sort(d.begin(),d.end());
//     d.erase(unique(d.begin(),d.end()),d.end());
    
//     for(u32& i: indices){
//         i=lower_bound(d.begin(),d.end(),i)-d.begin();
//     }

//     for(u32 i=0,j=0;i<v_cnt;i++,j++){
//         while(i<v_cnt&&find(i)!=i) i++;
//         while(j<i&&find(j)==j) j++;
//         if(i<v_cnt&&j<i) positions[j]=positions[i];
//     }
//     positions.resize(d.size());

//     return Mesh{positions,indices};
// }