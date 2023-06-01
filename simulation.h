#include <iostream>
#define N 130
#define K 20
#define MAX_VELOCITY 10000000
#define MIN_VELOCITY -10000000

class Fluid {
public:
    float density[N + 2][N + 2];
    float density_prev[N + 2][N + 2];
    float velocityX[N + 2][N + 2];
    float velocityX_prev[N + 2][N + 2];
    float velocityY[N + 2][N + 2];
    float velocityY_prev[N + 2][N + 2];
    float dt;
    float forceStrength;
    bool square_bnd;
    int sqr_bnd_size;

    Fluid(){
        dt = 0.0001f;
        forceStrength = 255.f;
        for (int i = 0; i < N + 2; i++) {
            for (int j = 0; j < N + 2; j++) {
                density[i][j] = 0;
                density_prev[i][j] = 0;
                velocityX[i][j] = 0;
                velocityX_prev[i][j] = 0;
                velocityY[i][j] = 0;
                velocityY_prev[i][j] = 0;
            }
        }
        square_bnd = false;
        sqr_bnd_size = 10;
    }
    
    void clear() {
        for (int i = 0; i < N + 2; i++) {
            for (int j = 0; j < N + 2; j++) {
                density[i][j] = 0;
                density_prev[i][j] = 0;
                velocityX[i][j] = 0;
                velocityX_prev[i][j] = 0;
                velocityY[i][j] = 0;
                velocityY_prev[i][j] = 0;
            }
        }
    }

    void set_bnd(int b, float x[N + 2][N + 2]) {
        for (int i = 1; i <= N; i++) {
            x[0][i]     = b == 1 ? -1 * x[1][i]: x[1][i];
            x[N + 1][i] = b == 1 ? -1 * x[N][i]: x[N][i];
            x[i][0]     = b == 2 ? -1 * x[i][1]: x[i][1];
            x[i][N + 1] = b == 2 ? -1 * x[i][N]: x[i][N];
        }
        x[0][0]         = 0.5 * (x[1][0] + x[0][1]);
        x[0][N + 1]     = 0.5 * (x[1][N + 1] + x[0][N]);
        x[N + 1][0]     = 0.5 * (x[N][0] + x[N + 1][1]);
        x[N + 1][N + 1] = 0.5 * (x[N][N + 1] + x[N + 1][N]);

        if(square_bnd){
            int sqr_flr = int(N / 2) - sqr_bnd_size;
            int sqr_cel = int(N / 2) + sqr_bnd_size;
            for(int i = sqr_flr; i <= sqr_cel; i++){
                x[sqr_flr][i]     = b == 1 ? -1 * x[sqr_flr - 1][i]: x[sqr_flr - 1][i];
                x[sqr_cel][i]     = b == 1 ? -1 * x[sqr_cel + 1][i]: x[sqr_cel + 1][i];
                x[i][sqr_flr]     = b == 2 ? -1 * x[i][sqr_flr - 1]: x[i][sqr_flr - 1];
                x[i][sqr_cel + 0] = b == 2 ? -1 * x[i][sqr_cel + 1]: x[i][sqr_cel + 1];
            }
            for(int i = sqr_flr + 1; i < sqr_cel; i++){
                for(int j = sqr_flr + 1; j < sqr_cel; j++){
                    x[i][j] = 0;
                }
            }
        }

    }

    void diffuse(int b, float x[N + 2][N + 2], float x0[N + 2][N + 2]) {
        float a = dt * N * N;
        for (int k = 0; k < K; k++) {
            for (int i = 1; i <= N; i++) {
                for (int j = 1; j <= N; j++) {
                    if(square_bnd){
                        int tmpbnd = sqr_bnd_size - 1;
                        if((j < N/2 + tmpbnd) && (j > N/2 - tmpbnd) && (i < N/2 + tmpbnd) && (i > N/2 - tmpbnd)){
                            continue;
                        }
                    }

                    x[i][j] = (x0[i][j] + a * (x[i - 1][j] + x[i + 1][j] + x[i][j - 1] + x[i][j + 1])) / (1 + 4 * a);
                }
            }
            set_bnd(b, x);
        }
    }

    void advect(int b, float d[N + 2][N + 2], float d0[N + 2][N + 2], float u[N + 2][N + 2], float v[N + 2][N + 2]) {
        float dt0 = dt * N;
        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                float x = i - dt0 * u[i][j], y = j - dt0 * v[i][j];
                if (x < 0.5) x = 0.5;
                if (x > N + 0.5) x = N + 0.5;
                int i0 = (int)x, i1 = i0 + 1;

                if (y < 0.5) y = 0.5;
                if (y > N + 0.5) y = N + 0.5;
                int j0 = (int)y, j1 = j0 + 1;
                
                float s1 = x - i0, s0 = 1 - s1, t1 = y - j0, t0 = 1 - t1;
                d[i][j] = s0 * (t0 * d0[i0][j0] + t1 * d0[i0][j1]) + s1 * (t0 * d0[i1][j0] + t1 * d0[i1][j1]);
            }
        }
        set_bnd(b, d);
    }

    void densityUpdate() {
        diffuse(0, density_prev, density);
        advect(0, density, density_prev, velocityX, velocityY);
    }

    void project(float u[N + 2][N + 2], float v[N + 2][N + 2], float p[N + 2][N + 2], float div[N + 2][N + 2]) {
        float h = 1.0f / N;
        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                div[i][j] = -0.5 * h * (u[i + 1][j] - u[i - 1][j] + v[i][j + 1] - v[i][j - 1]);
                p[i][j] = 0;
            }
        }
        set_bnd(0, div);
        set_bnd(0, p);

        for (int k = 0; k < K; k++) {
            for (int i = 1; i <= N; i++) {
                for (int j = 1; j <= N; j++) {
                    p[i][j] = (div[i][j] + p[i - 1][j] + p[i + 1][j] + p[i][j - 1] + p[i][j + 1]) / 100;
                }
            }
            set_bnd(0, p);
        }

        for (int i = 1; i <= N; i++) {
            for (int j = 1; j <= N; j++) {
                u[i][j] -= 0.5 * (p[i + 1][j] - p[i - 1][j]) / h;
                v[i][j] -= 0.5 * (p[i][j + 1] - p[i][j - 1]) / h;
            }
        }
        set_bnd(1, u);
        set_bnd(2, v);
    }

    void velocityUpdate() {
        diffuse(1, velocityX_prev, velocityX);
        diffuse(2, velocityY_prev, velocityY);
        project(velocityX_prev, velocityY_prev, velocityX, velocityY);
        advect(1, velocityX, velocityX_prev, velocityX_prev, velocityY_prev);
        advect(2, velocityY, velocityY_prev, velocityX_prev, velocityY_prev);
        project(velocityX, velocityY, velocityX_prev, velocityY_prev);
    }

    void Update() {
        // check();
        velocityUpdate();
        densityUpdate();
    }

    void check() {
        for (int i = 0; i < N + 2; i++) {
            for (int j = 0; j < N + 2; j++) {
                if (density[i][j] < 0) density[i][j] = 0;
                if (density_prev[i][j] < 0) density_prev[i][j] = 0;
                if (velocityX[i][j] > MAX_VELOCITY) velocityX[i][j] = MAX_VELOCITY;
                if (velocityX[i][j] < MIN_VELOCITY) velocityX[i][j] = MIN_VELOCITY;
                if (velocityX_prev[i][j] > MAX_VELOCITY) velocityX_prev[i][j] = MAX_VELOCITY;
                if (velocityX_prev[i][j] < MIN_VELOCITY) velocityX_prev[i][j] = MIN_VELOCITY;

                if (velocityY[i][j] > MAX_VELOCITY) velocityY[i][j] = MAX_VELOCITY;
                if (velocityY[i][j] < MIN_VELOCITY) velocityY[i][j] = MIN_VELOCITY;
                if (velocityY_prev[i][j] > MAX_VELOCITY) velocityY_prev[i][j] = MAX_VELOCITY;
                if (velocityY_prev[i][j] < MIN_VELOCITY) velocityY_prev[i][j] = MIN_VELOCITY;
            }
        }
    }

    void addDensity(int r, int c, float amount) {
        density[r][c] += amount;
    }

    void addVelocityX(int r, int c, float amount) {
        velocityX[r][c] += amount;
    }

    void addVelocityY(int r, int c, float amount) {
        velocityY[r][c] += amount;
    }

    void addSqrBnd(){
        square_bnd == true ? square_bnd = false: square_bnd = true;
    }
};
