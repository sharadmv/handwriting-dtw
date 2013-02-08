#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<vector>
#include<map>
#include<cassert>
#include<iostream>
#include<dirent.h>
#include<string>
#include<algorithm>
#include<sstream>
#include<iostream>
#include<fstream>
#include<algorithm>
#include<omp.h>

using namespace std;

#define INF 1e20;

double min(double x, double y) {
    return ((x)<(y)?(x):(y));
}
double max(double x, double y) {
    return ((x)>(y)?(x):(y));
}

typedef struct {
    double x;
    double y;
} point;

typedef struct {
    int index;
    double x, y;
} index;

typedef struct dist_comp {
    int location;
    double value;
    string letter;
} dist_comp;

void printdistcomp(vector<dist_comp> v) {
    for (int a = 0;a < v.size(); a++) {
        string s = ": "+v[a].letter+" = %lf, %u\n";
        printf(("%d"+s).c_str(),a, v[a].value, v[a].location);
    }
}

bool compareDistance(const dist_comp &a, const dist_comp &b) {
    return a.value < b.value;
}

int compareIndex(const void *x, const void* y) {
    index* xi = ((index *) x);
    index* yi = ((index *) y);
    double diffx = fabs(xi->x) - fabs(yi->x);
    double diffy = fabs(xi->y) - fabs(yi->y);
    return diffx+diffy;
}

string pointtostring(point point) {
    ostringstream ss;
    ss << "(" << point.x << "," << point.y <<")";
    return ss.str();
}

double comp(double x, double y) {
    return (x - y)*(x - y);
}

double dist(point x, point y) {
    return comp(x.x, y.x) +  comp(x.y, y.y);
}

void printpoints (point *arr, int len) {
    cout << "[ ";
    for (int a = 0;a < len; a++) {
        cout << pointtostring(arr[a]) << ", ";
    }
    cout << "]\n";
}

void printarray (double *arr, int len) {
    cout << "[ ";
    for (int a = 0;a < len; a++) {
        printf("%g ", arr[a]);
    }
    cout << "]\n";
}
void printarray (int *arr, int len) {
    cout << "[ ";
    for (int a = 0;a < len; a++) {
        printf("%d ", arr[a]);
    }
    cout << "]\n";
}

double kim(point *t, point *q, int j, int len, double meanx, double stdx, double meany, double stdy, double bsf, int* steps) {

    /// 1 point at front and back
    double d, lb;
    double x0 = (t[j].x - meanx)/stdx;
    double y0 = (t[len-1+j].x - meanx)/stdx;
    lb = comp(x0,q[0].x) + comp(y0,q[len-1].x);
    if (lb >= bsf)   return lb;

    /// 2 points at front
    double x1 = (t[(j+1)].x - meanx) / stdx;
    d = min(comp(x1,q[0].x), comp(x0,q[1].x));
    d = min(d, comp(x1,q[1].x));
    lb += d;
    if (lb >= bsf)   return lb;

    /// 2 points at back
    double y1 = (t[(len-2+j)].x - meanx) / stdx;
    d = min(comp(y1,q[len-1].x), comp(y0, q[len-2].x) );
    d = min(d, comp(y1,q[len-2].x));
    lb += d;
    if (lb >= bsf)   return lb;

    /// 3 points at front
    double x2 = (t[(j+2)].x - meanx) / stdy;
    d = min(comp(x0,q[2].x), comp(x1, q[2].x));
    d = min(d, comp(x2,q[2].x));
    d = min(d, comp(x2,q[1].x));
    d = min(d, comp(x2,q[0].x));
    lb += d;
    if (lb >= bsf)   return lb;

    /// 3 points at back
    double y2 = (t[(len-3+j)].x - meanx) / stdx;
    d = min(comp(y0,q[len-3].x), comp(y1, q[len-3].x));
    d = min(d, comp(y2,q[len-3].x));
    d = min(d, comp(y2,q[len-2].x));
    d = min(d, comp(y2,q[len-1].x));
    lb += d;

    /// 1 point at front and back
    x0 = (t[j].y - meany)/stdy;
    y0 = (t[(len-1+j)].y - meany)/stdy;
    lb = comp(x0,q[0].y) + comp(y0,q[len-1].y);
    if (lb >= bsf)   return lb;

    /// 2 points at front
    x1 = (t[(j+1)].y - meany) / stdy;
    d = min(comp(x1,q[0].y), comp(x0,q[1].y));
    d = min(d, comp(x1,q[1].y));
    lb += d;
    if (lb >= bsf)   return lb;

    /// 2 points at back
    y1 = (t[(len-2+j)].y - meany) / stdy;
    d = min(comp(y1,q[len-1].y), comp(y0, q[len-2].y) );
    d = min(d, comp(y1,q[len-2].y));
    lb += d;
    if (lb >= bsf)   return lb;

    /// 3 points at front
    x2 = (t[(j+2)].y - meany) / stdy;
    d = min(comp(x0,q[2].y), comp(x1, q[2].y));
    d = min(d, comp(x2,q[2].y));
    d = min(d, comp(x2,q[1].y));
    d = min(d, comp(x2,q[0].y));
    lb += d;
    if (lb >= bsf)   return lb;

    /// 3 points at back
    y2 = (t[(len-3+j)].y - meany) / stdy;
    d = min(comp(y0,q[len-3].y), comp(y1, q[len-3].y));
    d = min(d, comp(y2,q[len-3].y));
    d = min(d, comp(y2,q[len-2].y));
    d = min(d, comp(y2,q[len-1].y));
    lb += d;
    *steps = *steps + 1;

    return lb;
}

double keoghData(int *order, point *tz, point *q, point *l, point *u, double *cb, int len, double meanx, double stdx, double meany, double stdy, double bsf, int* steps) {
    double lb = 0;
    double uux,llx,uuy,lly,d;
    for (int i = 0; i < len && lb < bsf; i++) {
        uux = (u[order[i]].x-meanx)/stdx;
        llx = (l[order[i]].x-meanx)/stdx;
        uuy = (u[order[i]].y-meany)/stdy;
        lly = (l[order[i]].y-meany)/stdy;
        d = 0;
        point temp;
        if (q[i].x > uux) {
            d = comp(q[i].x, uux);
        } else if (q[i].x < llx) {
            d = comp(q[i].x, llx);
        }
        lb += d;
        temp.x = d;

        d = 0;
        if (q[i].y > uuy) {
            d = comp(q[i].y, uuy);
        } else if (q[i].y < lly) {
            d = comp(q[i].y, lly);
        }
        lb += d;
        temp.y = d;

        cb[order[i]] = temp.x + temp.y;
        *steps = *steps + 1;
    }
    return lb;
}

double keogh(int *order, point *t, point *l, point *u, double *cb, int j, int len, double meanx, double stdx, double meany, double stdy, double bsf, int* steps) {
    double lb = 0;
    double x,y,d;

    for (int i = 0; i < len && lb < bsf; i++) {
        x = (t[order[i] + j].x - meanx)/stdx;
        y = (t[order[i] + j].y - meany)/stdy;

        point c;

        d = 0;
        if (x > u[order[i]].x)
            d = comp(x,u[order[i]].x);
        else if (x < l[order[i]].x)
            d = comp(x,l[order[i]].x);
        lb += d;
        c.x = d;

        d = 0;
        if (y > u[order[i]].y)
            d = comp(y,u[order[i]].y);
        else if (y < l[order[i]].y)
            d = comp(y,l[order[i]].y);
        lb += d;
        c.y = d;

        cb[order[i]] = c.x+c.y;
        *steps = *steps + 1;
    }
    return lb;
}

double dtw(point* q, point* d, double *cb, int m, int r, double bsf, int* steps) {
    int wl = 2*r+1;
    double *prev = (double *) malloc(sizeof(double)*(wl));
    double *curr = (double *) malloc(sizeof(double)*(wl));
    double *temp;
    for (int i = 0;i < wl; i++) {
        curr[i] = INF;
        prev[i] = INF;
    }
    int i,j,k;
    double x,y,z;
    double best;
    for (i = 0; i < m; i++) {
        k = max(0,r-i);
        best = INF;
        for(j=max(0,i-r); j<=min(m-1,i+r); j++, k++) {
            if ((i==0) && (j==0)) {
                curr[k]=dist(q[0],d[0]);
                best = curr[k];
                continue;
            }
            if ((j-1<0)||(k-1<0)) {
                y = INF;
            }
            else {
                y = curr[k-1];
            }
            if ((i-1<0)||(k+1>2*r)) {
                x = INF;
            }
            else {
                x = prev[k+1];
            }
            if ((i-1<0)||(j-1<0)) {
                z = INF;
            }
            else {
                z = prev[k];
            }
            double cost = dist(q[i],d[j])+min(x,min(y,z));
            if (cost < best) {
                best = cost;
            }
            curr[k] = cost;
            *steps = *steps + 1;
        }
        if (i+r < m-1 && best /*+ cb[i+r+1]*/ >= bsf) {
            free(curr);
            free(prev);
            return best/*+cb[i+r+1]*/;
        }
        temp = curr;
        curr = prev;
        prev = temp;
    }
    double ret = prev[k-1];
    free(curr);
    free(prev);
    return ret;
}

typedef struct deque {
    int *dq;
    int size,capacity;
    int f,r;
} deque;

void init(deque *d, int capacity) {
    d->capacity = capacity;
    d->size = 0;
    d->dq = (int *) malloc(sizeof(int)*d->capacity);
    d->f = 0;
    d->r = d->capacity-1;
}

void destroy(deque *d) {
    free(d->dq);
}

void push_back(struct deque *d, int v) {
    d->dq[d->r] = v;
    d->r--;
    if (d->r < 0) {
        d->r = d->capacity-1;
    }
    d->size++;
}

void pop_front(struct deque *d) {
    d->f--;
    if (d->f < 0) {
        d->f = d->capacity-1;
    }
    d->size--;
}

void pop_back(struct deque *d) {
    d->r = (d->r+1)%d->capacity;
    d->size--;
}

int front(struct deque *d) {
    int aux = d->f - 1;

    if (aux < 0) {
        aux = d->capacity-1;
    }
    return d->dq[aux];
}

int back(struct deque *d) {
    int aux = (d->r+1)%d->capacity;
    return d->dq[aux];
}

int empty(struct deque *d) {
    return d->size == 0;
}

//Using lemire optimization
void wedge(point *t, point *t2, int len, int r, point *l, point *u, int *steps) {

    struct deque dux, dlx, duy, dly;
    init(&dux, 2*r+2);
    init(&dlx, 2*r+2);
    init(&duy, 2*r+2);
    init(&dly, 2*r+2);

    push_back(&dux, 0);
    push_back(&dlx, 0);
    push_back(&duy, 0);
    push_back(&dly, 0);

    for (int i = 1; i < len; i++) {
        if (i > r) {
            point upper;
            upper.x = max(t[front(&dux)].x,t2[front(&dux)].x);
            upper.y = max(t[front(&duy)].y,t2[front(&duy)].y);

            point lower;
            lower.x = min(t[front(&dlx)].x,t2[front(&dlx)].x);
            lower.y = max(t[front(&dly)].y,t2[front(&dly)].y);

            u[i-r-1] = upper;
            l[i-r-1] = lower;
        }

        if (max(t[i].x,t2[i].x) > max(t[i-1].x,t2[i-1].x)) {
            pop_back(&dux);
            while (!empty(&dux) && max(t[i].x,t2[i].x) > max(t[back(&dux)].x,t2[back(&dux)].x))
                pop_back(&dux);
        } else {
            pop_back(&dlx);
            while (!empty(&dlx) && min(t[i].x,t2[i].x) < min(t[back(&dlx)].x,t2[back(&dlx)].x))
                pop_back(&dlx);
        }
        push_back(&dux, i);
        push_back(&dlx, i);

        if (max(t[i].y,t2[i].y) > max(t[i-1].y,t2[i-1].y)) {
            pop_back(&duy);
            while (!empty(&duy) && max(t[i].y,t2[i].y) > max(t[back(&duy)].y,t2[back(&duy)].y))
                pop_back(&duy);
        } else {
            pop_back(&dly);
            while (!empty(&dly) && min(t[i].y,t2[i].y) < min(t[back(&dly)].y,t2[back(&dly)].y))
                pop_back(&dly);
        }
        push_back(&duy, i);
        push_back(&dly, i);
        if (i == 2 * r + 1 + front(&dux))
            pop_front(&dux);
        else if (i == 2 * r + 1 + front(&dlx))
            pop_front(&dlx);

        if (i == 2 * r + 1 + front(&duy))
            pop_front(&duy);
        else if (i == 2 * r + 1 + front(&dly))
            pop_front(&dly);
        *steps = *steps+1;
    }
    for (int i = len; i < len+r+1; i++) {
        point upper;
        upper.x = max(t[front(&dux)].x,t2[front(&dux)].x);
        upper.y = max(t[front(&duy)].y,t2[front(&duy)].y);

        point lower;
        lower.x = min(t[front(&dlx)].x,t2[front(&dlx)].x);
        lower.y = max(t[front(&dly)].y,t2[front(&dly)].y);

        u[i-r-1] = upper;
        l[i-r-1] = lower;
        if (i-front(&dux) >= 2 * r + 1)
            pop_front(&dux);
        if (i-front(&dlx) >= 2 * r + 1)
            pop_front(&dlx);
        if (i-front(&duy) >= 2 * r + 1)
            pop_front(&duy);
        if (i-front(&dly) >= 2 * r + 1)
            pop_front(&dly);
        *steps = *steps+1;
    }
    destroy(&dux);
    destroy(&dly);
}

void writeToFile(point* vec, int len, string name) {
    string out = "";
    ofstream output;
    string comp = "comp/";
    output.open((comp+name).c_str());
    for (int i = 0;i < len;i++) {
        output << vec[i].x << " " << vec[i].y << endl;
    }
}

void search(point *query, int m, double R, point *D, int length, double *distance, double *location, int debug, string name) {

    //normalize query
    point *Q, *q, *T,**C, **co;


    Q = (point *)malloc(sizeof(point )*m);
    q = (point *)malloc(sizeof(point)*m);
    T = (point *)malloc(sizeof(point)*m);
    int r = R;
    int rotationframe = 0;//floor(0.05*m);
    if (rotationframe == 0) {
        rotationframe = 0;
    }
    if (debug) {
        cout << "Number of rotation frames: " << 2*rotationframe + 1 << " (" << rotationframe << " on each side)\n";
    }
    C = (point **)malloc(sizeof(point *)*(2*rotationframe+1));
    co = (point **)malloc(sizeof(point *)*(2*rotationframe+1));
    int i = 0, j;
    double d;
    double exx = 0, ex2x = 0;
    double exy = 0, ex2y = 0;

    int steps = 0;
    while(i < m) {
        d = query[i].x;
        exx += d;
        ex2x += d*d;
        d = query[i].y;
        exy += d;
        ex2y += d*d;
        i++;
        steps++;
    }

    double qmeanx = exx/m;
    double qstdx = ex2x/m;
    double meanx;
    double stdx;
    qstdx = sqrt(qstdx-qmeanx*qmeanx);

    double qmeany = exy/m;
    double qstdy = ex2y/m;
    double meany;
    double stdy;
    qstdy = sqrt(qstdy-qmeany*qmeany);

    for(i = 0 ; i < m ; i++) {
        point temp;
        temp.x = (query[i].x - qmeanx)/qstdx;
        temp.y = (query[i].y - qmeany)/qstdy;
        q[i] = (temp);
    }

    int numkim = 0;
    int numlb1 = 0;
    int numlb2 = 0;
    int numdtw = 0;
    int **order = (int **)malloc(sizeof(int*)*(2*rotationframe+1));

    if (debug) {
        printf("Processing query...\n");
    }


    point** uo = (point**)malloc(sizeof(point)*(2*rotationframe+1));
    point** lo = (point**)malloc(sizeof(point)*(2*rotationframe+1));
    double** cb = (double**)malloc(sizeof(double)*(2*rotationframe+1));

    double** cb1 = (double **)malloc(sizeof(double)*(2*rotationframe+1));

    double** cb2 = (double **)malloc(sizeof(double)*(2*rotationframe+1));

    if (debug)
        printf("  Sorting query...\n");

    index *Q_tmp;
    Q_tmp = (index*) malloc(sizeof(index)*m);

    for(i = 0; i<m; i++) {
        Q_tmp[i].x = q[i].x;
        Q_tmp[i].y = q[i].y;
        Q_tmp[i].index = i;
    }

    qsort(Q_tmp, m, sizeof(index), compareIndex);
    for (int b = 0; b < 2*rotationframe+1; b++) {
        C[b] = (point*) malloc(sizeof(point)*m);
        co[b] = (point*) malloc(sizeof(point)*m);
        uo[b] = (point*) malloc(sizeof(point)*m);
        lo[b] = (point*) malloc(sizeof(point)*m);
        cb[b] = (double *) malloc(sizeof(double)*m);
        cb1[b] = (double *) malloc(sizeof(double)*m);
        cb2[b] = (double *) malloc(sizeof(double)*m);
        order[b] = (int *) malloc(sizeof(int)*m);
        steps++;
    }
    if (debug)
        printf("  Wedging query...\n");

    for (int w = -rotationframe, v = 0; w <= rotationframe; w++, v++) {
        for(i = 0;i < m;i++) {
            int offset = (w+i)%m;
            if (offset < 0) {
                offset += m;
            }
            C[v][offset] = q[offset];
            int o = Q_tmp[i].index;
            order[v][offset] = o;
            co[v][offset] = q[o];
            cb[v][offset] = 0;
            cb1[v][offset] = 0;
            cb2[v][offset] = 0;
            steps++;
        }
        wedge(C[v],C[v], m, r, lo[v], uo[v], &steps);
    }
    free(Q_tmp);
    free(Q);
    free(q);

    double t1 = clock();
    T = (point*)malloc(sizeof(point)*2*m);


    point *tz = (point *)malloc(sizeof(point)*m);

    double dist = 0;
    long int loc;
    i = 0;
    j = 0;
    int k = 0;
    exx = ex2x = 0;
    exy = ex2y = 0;
    double threshold = INF;
    double lbkim = 0;
    double bsf = threshold;
    int rot = -1;

    point* ubuffer = (point*)malloc(sizeof(point)*1000000);
    point* lbuffer = (point*)malloc(sizeof(point)*1000000);
    point* buffer = D;

    if (debug) {
        printf("Wedging data...\n");
        printf("Length: %d, R: %d\n",r, steps);
    }
    wedge(buffer, buffer, length, r, lbuffer, ubuffer, &steps);
    if (debug)
        printf("Searching...\n");

    for (i = 0; i < length; i++) {
        point temp = buffer[i];
        d = buffer[i].x;

        exx += d;
        ex2x += d*d;

        d = buffer[i].y;

        exy += d;
        ex2y += d*d;
        T[i%m] = temp;
        T[(i%m)+m] = temp;
        if(i >= m-1) {
            j = (i+1)%m;
            int I = i-(m-1);

            meanx = exx/m;
            stdx = ex2x/m;
            stdx = sqrt(stdx-meanx*meanx);

            meany = exy/m;
            stdy = ex2y/m;
            stdy = sqrt(stdy-meany*meany);

            for (int ro = 0; ro < 2*rotationframe+1; ro++) {
                double lbkim = kim(T, C[ro], j, m, meanx, stdx, meany, stdy,bsf,&steps);
                if (lbkim < bsf) {
                    double lbkeogh = keogh(order[ro], T, lo[ro], uo[ro],cb1[ro], j, m, meanx, stdx,meany,stdy, bsf, &steps);
                    if (lbkeogh < bsf) {
                        double lbkeogh2 = keoghData(order[ro], tz, co[ro], lbuffer+I, ubuffer+I, cb2[ro], m, meanx, stdx, meany, stdy, bsf, &steps);
                        if (lbkeogh2 < bsf) {
                            for(k=0;k<m;k++) {
                                point temp;
                                temp.x = (T[k+j].x - meanx)/stdx;
                                temp.y = (T[k+j].y - meany)/stdy;
                                tz[k] = temp;
                                steps++;
                            }

                            if (lbkeogh > lbkeogh2) {
                                cb[ro][m-1]=cb1[ro][m-1];
                                for(k=m-2; k>=0; k--)
                                    cb[ro][k] = cb[ro][k+1]+cb1[ro][k];
                            }
                            else {
                                cb[ro][m-1]=cb2[ro][m-1];
                                for(k=m-2; k>=0; k--)
                                    cb[ro][k] = cb[ro][k+1]+cb2[ro][k];
                            }

                            dist = dtw(C[ro],tz,cb[ro], m,r, bsf, &steps);

                            if (lbkeogh - 0.0000000001 > dist) {
                                cout << "BAD STUFF IS HAPPENING BRO!" << endl;
                            }
                            if(dist < bsf) {
                                rot = rotationframe;
                                bsf = dist;
                                loc = i-m+1;
                                writeToFile(C[ro], m, name+"-norm");
                                writeToFile(tz, m, name+"-match");
                            }
                            numdtw++;
                        } else {
                            //cout << "Blocked by LB2!" << endl;
                            numlb2++;
                        }
                    } else {
                        //cout << "Blocked by LB1!" << endl;
                        numlb1++;
                    }
                } else {
                    //cout << "Blocked by LBKim!" << endl;
                    numkim++;
                }
                steps++;
            }

            exx -= T[j].x;
            ex2x -= T[j].x*T[j].x;
            exy -= T[j].y;
            ex2y -= T[j].y*T[j].y;
        }
    }
    free(uo);
    free(lo);
    char end1 = '\n';
    double t2 = clock();
    if (rot != -1) {
        if (debug) {
            cout << "Rotation: " << rot << endl;
            cout << "[ ";
            /*for (int a = 0;a < m; a++) {
                printf("%g ", C[rot][a]*qstd+qmean);
            }
            cout << "]\n";
            cout << "[ ";
            for (int a = loc;a < loc+m; a++) {
                printf("%g ", buffer[a]);
            }*/
            cout << "]\n";
            cout << "Location : " << loc << endl;
            cout << "Distance : " << sqrt(bsf) << endl;
            cout << "Data Scanned : " << i << endl;
            cout << "Num Steps: " << steps << endl;
            cout << "Total Execution Time : " << (t2-t1)/CLOCKS_PER_SEC << " sec" << endl;
            cout << "--------------------------" << endl;
            cout << "Kim: " << numkim << endl;
            cout << "Keogh1: " << numlb1 << endl;
            cout << "Keogh2: " << numlb2<< endl;
            cout << "DTW: " << numdtw << endl;
        }
        *distance = sqrt(bsf);
        *location = loc;
    } else {
        if (debug)
            cout << "No match found\n";
        *distance = -1;
        *location = -1;
    }

    free(T);
    free(tz);
    free(C);
    return;
}

int main(int argc, char *argv[]) {

    point *Q;
    FILE *fp, *qp;

    int m = atol(argv[3]);
    double R = atof(argv[4]);
    int r;
    if (R<=1)
        r = floor(R*m);
    else
        r = floor(R);
    if (r == 0) {
        r = 1;
    }
    r = R;
    Q = (point*)malloc(sizeof(point)*m);

    fp = fopen(argv[2],"r");
    double d;


    float start = clock();

    //printf("Reading data...\n");
    point* buffer = (point*)malloc(sizeof(point)*1000000);
    int length = 0;
    int steps = 0;
    double x,y;
    while(fscanf(fp,"%lf %lf",&x, &y) != EOF) {
        point temp;
        temp.x = x;
        temp.y = y;
        buffer[length] = temp;
        length++;
    }
    fclose(fp);

    point* ubuffer = (point*)malloc(sizeof(point)*1000000);
    point* lbuffer = (point*)malloc(sizeof(point)*1000000);

    wedge(buffer, buffer, length, r, lbuffer, ubuffer, &steps);

    double distance, location;

    string trainDir = argv[1];

    DIR *mydir = opendir(trainDir.c_str());

    struct dirent *entry = NULL;

    vector<dist_comp> distances;
    map<char,double> sum;
    map<char,int> count;
    map<char,double> mean_loc;
    map<char,double> mean_std;
    point *norm, *match;
    vector<string> candidates;
    while((entry = readdir(mydir))) {
        string tmp = entry->d_name;
        if (tmp != "." && tmp != "..") {
            candidates.push_back(tmp);
        }
    }
    //omp_set_num_threads(1);
    #pragma omp parallel for private(qp, distance, location, buffer, length, Q) shared(r, trainDir, candidates, sum,count, mean_loc, mean_std)
    for (int l = 0; l < candidates.size(); l++) {
        //cout << omp_get_num_threads() << ",";
        //cout << omp_get_thread_num() << endl;
        int i = 0;
        string tmp = candidates[l];
        if (tmp != "." && tmp != "..") {
            qp = fopen((trainDir+tmp).c_str(),"r");
            while(fscanf(qp,"%lf %lf",&x, &y) != EOF && i < m) {
                point temp;
                temp.x = x;
                temp.y = y;
                Q[i] = temp;
                i++;
            }
            string c = tmp.substr(0,1);
            //cout << tmp << endl;

            search(Q, i, r, buffer, length, &distance, &location, 0, c);

            char character = tmp[0];
            map<char,double>::iterator it = sum.find(character);
            distance = distance / (i);
            #pragma omp critical
            {
            if(it == sum.end()) {
                sum[character] = 999999;
                count[character] = 0;
                mean_loc[character] = 0;
                mean_std[character] = 0;
            }
            if (distance < sum[character]) {
                sum[character] = distance;
                mean_loc[character] = location;
            }
            mean_std[character] += location*location;
            count[character] += 1;
            }
        }
    }
    for(map<char,double>::iterator ii=sum.begin(); ii != sum.end(); ++ii) {
        char ch = (*ii).first;
        int c = count[ch];
        double dist = (*ii).second;// / c;
        double locsum = mean_loc[ch];
        double locsum2 = mean_std[ch];

        dist_comp d;
        d.letter = (*ii).first;

        double meanx = locsum/c;
        double stdx = locsum2/c;
        stdx = sqrt(stdx-meanx*meanx);

        d.value = dist; //* sqrt(stdx);
        //cout << ch << ": " << dist << ", " << stdx << endl;

        d.location = locsum; ///c;
        distances.push_back(d);
    }

    sort(distances.begin(), distances.end(), compareDistance);
    printdistcomp(distances);
    cout << "Total time: " << (clock() - start)/CLOCKS_PER_SEC << endl;

    free(buffer);
    free(ubuffer);
    free(lbuffer);

    closedir(mydir);

    /*point* c1;
    point* c2;
    point* l;
    point* u;
    point* cb;
    int size = 20;
    c1 = (point*) malloc(sizeof(point)*size);
    c2 = (point*) malloc(sizeof(point)*size);
    l = (point*) malloc(sizeof(point)*size);
    u = (point*) malloc(sizeof(point)*size);
    cb = (point*) malloc(sizeof(point)*size);
    for (int i = 0; i < 20; i++) {
        point one,two;
        one.x = i;
        one.y = i;
        two.x = i+1;
        two.y = i+1;

        c1[i] = one;
        c2[i] = two;
    }
    printpoints(c1, size);
    printpoints(c2, size);
    int r = 5;
    int steps = 0;
    double distance = dtw(c1, c2, NULL, size, r, 99999999.0, &steps);
    cout << "DISTANCE: " << distance << endl;
    wedge(c1, c1,size, r, l, u, &steps);
    double lbkeogh = keogh(NULL, c2, l, u, cb, 0, size, 0,0,0,0, 9999999, &steps);
    cout << "LBkeogh: " << lbkeogh << endl;
    double lbkim = kim(c1, c2,0,size,0,1,0,1,999999999999,&steps);
    cout << "LBkim: " << lbkim << endl;*/

}
