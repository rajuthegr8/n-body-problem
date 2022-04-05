#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define N 1000
#define X 100.0
#define Y 200.0
#define Z 400.0
#define R 0.5
#define dt 0.5 
#define NUM_THREADS 12
#define ep 1e-6
struct vector
{
    double x,y,z;
};
typedef struct vector vector;
struct body
{
    vector r;
    vector v;
};
double dot(vector *a,vector *b)
{
    double c = 0;
    c+= a->x * b->x;
    c+= a->y * b->y;
    c+= a->z * b->z;
    return c;
}
vector add(vector *a,vector *b)
{
    vector c;
    c.x = a->x + b->x;
    c.y = a->y + b->y;
    c.z = a->z + b->z;
    return c;
}
vector sub(vector *a,vector *b)
{
    vector c;
    c.x = a->x - b->x;
    c.y = a->y - b->y;
    c.z = a->z - b->z;
    return c;
}
vector mul(double k,vector *b)
{
    vector c;
    c.x = k*b->x;
    c.y = k*b->y;
    c.z = k*b->z;
    return c;
}

vector zero;
vector force_matrix[N][N];
vector force_body[N];
vector half_step_velocity[N];
struct body arr[N];
struct body arr_next[N];

void wall_collision();
void body_collision();
void force_calculation();
void half_step_velocity_calculation();
void position_update();
void velocity_calculation();

int main()
{
    double itime, ftime, exec_time;
    itime = omp_get_wtime();
    zero.x=zero.y=zero.z=0.0;
    #pragma omp parallel for
    for (int i = 0; i < N; i++)
    {
        arr[i].v=zero;
        force_body[i]=zero;
        half_step_velocity[i]=zero;
        arr_next[i].r=zero;
        arr_next[i].v=zero;
    }
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++)
    {    
        for (int j = 0; j < N; j++)
        force_matrix[i][j]=zero;
    }
    FILE* ptr;
    char ch[60];
    char c;
    ptr = fopen("Trajectory.txt", "r");
    int i = 8;
    while(i--)
    {
        fgets(ch, sizeof(ch), ptr);
    }
    i = 0;
    while (fgets(ch, sizeof(ch), ptr))
    {        
        
        char *a,*b;
        arr[i].r.x = strtod(ch,&a);
        arr[i].r.y = strtod(a,&b);
        arr[i].r.z = strtod(b,&a);
        i++;
    }
    fclose(ptr);
    ptr = fopen("traj.bin","wb");
    int steps = 1000;

    
    for (int i = 0; i < steps; i++)
    {
        wall_collision();
        body_collision();
        force_calculation();
        half_step_velocity_calculation();
        position_update();
        velocity_calculation();
        wall_collision();
        if(i%5==0)
        {
            for (int i = 0; i < N; i++)
            {
                fprintf(ptr,"%.2lf %.2lf %.2lf\n",arr[i].r.x,arr[i].r.y,arr[i].r.z);
            }
        }
    }
    fclose(ptr);
    ftime = omp_get_wtime();
    exec_time = ftime - itime;
    printf("\n\nTime taken is %f", exec_time);
    
    
    
}

void wall_collision()
{
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        #pragma omp for
        
            for (int i = 0; i < N; i++)
            {
                if(arr[i].r.x<R)
                {
                    arr[i].v.x=-arr[i].v.x;
                    arr[i].r.x=R;
                }
                if(arr[i].r.y<R)
                {
                    arr[i].v.y=-arr[i].v.y;
                    arr[i].r.y=R;
                }
                if(arr[i].r.z<R)
                {
                    arr[i].v.z=-arr[i].v.z;
                    arr[i].r.z=R;
                }
                if(arr[i].r.x>X-R)
                {
                    arr[i].v.x=-arr[i].v.x;
                    arr[i].r.x = X-R;
                }
                if(arr[i].r.y>Y-R)
                {
                    arr[i].v.y=-arr[i].v.y;
                    arr[i].r.y = Y-R;
                }
                if(arr[i].r.z>Z-R)
                {
                    arr[i].v.z=-arr[i].v.z;
                    arr[i].r.z = Z-R;
                }
            }
        
    }
}
void body_collision()
{
    #pragma omp parallel num_threads(NUM_THREADS)
    {
        #pragma omp for
        
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {   
                    if(i==j)continue;

                    vector r,v;
                    r = sub(&arr[i].r,&arr[j].r);
                    v = sub(&arr[i].v,&arr[j].v);
                    double c = ep+dot(&r,&r);
                    if(c>(4*R*R))continue;

                    double k = dot(&v,&r)/c;
                    v = mul(k,&r);
                    arr_next[i].v = sub(&arr[i].v,&v);
                } 
            }
        
        #pragma omp for collapse(2)
        
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {   
                    arr[i].v = arr_next[i].v;
                } 
            }
        
    }

}
void force_calculation()
{
    #pragma omp parallel num_threads(NUM_THREADS)
          
        #pragma omp for collapse(2)
        
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {   
                    if(i==j)
                    {
                        continue;
                    }
                    vector r1 = sub(&arr[i].r,&arr[j].r);
                    double c=dot(&r1,&r1)+ep;
                    r1=sub(&arr[j].r,&arr[i].r);
                    force_matrix[i][j] = mul(1/c,&r1); 
                } 
            }
        
        #pragma omp for
        
            for (int i = 0; i < N; i++)
            {
                force_body[i] = zero;
            }
        
        #pragma omp for //outer loop only
        
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {   
                    force_body[i]=add(&force_body[i],&force_matrix[i][j]); 
                } 
            }
        
    }

void half_step_velocity_calculation()
{
    vector t;
    #pragma omp parallel for private(t) num_threads(NUM_THREADS)
    
        for (int i = 0; i < N; i++)
        {
            t = mul(dt/2,&force_body[i]);
            half_step_velocity[i] = add(&arr[i].v,&t);
        }
    
}
void position_update()
{
    vector t;
    #pragma omp parallel for private(t) num_threads(NUM_THREADS)
    
        for (int i = 0; i < N; i++)
        {
            t = mul(dt,&half_step_velocity[i]);
            arr[i].r = add(&arr[i].r,&t);
        }
    
}
void velocity_calculation()
{
    force_calculation();
    vector t;
    #pragma omp parallel for private(t) num_threads(NUM_THREADS)
    
        for (int i = 0; i < N; i++)
        {
            t = mul(dt/2,&force_body[i]);
            arr[i].v = add(&arr[i].v,&t);
        }    
}
