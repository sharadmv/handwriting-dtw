#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>

#define INF 1e20;
#define min(x,y) ((x)<(y)?(x):(y))
#define max(x,y) ((x)>(y)?(x):(y))
#define dist(x,y) ((x-y)*(x-y))

using namespace std;

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

/// Data structure for sorting the query
typedef struct Index
{   double value;
  int    index;
} Index;

/// Sorting function for the query, sort by abs(z_norm(q[i])) from high to low
int comp(const void *a, const void* b)
{   Index* x = (Index*)a;
  Index* y = (Index*)b;
  return abs(y->value) - abs(x->value);   // high to low
}

struct deque
{   int *dq;
  int size,capacity;
  int f,r;
};

void init(deque *d, int capacity)
{
  d->capacity = capacity;
  d->size = 0;
  d->dq = (int *) malloc(sizeof(int)*d->capacity);
  d->f = 0;
  d->r = d->capacity-1;
}

void destroy(deque *d)
{
  free(d->dq);
}

void push_back(struct deque *d, int v)
{
  d->dq[d->r] = v;
  d->r--;
  if (d->r < 0)
    d->r = d->capacity-1;
  d->size++;
}

void pop_front(struct deque *d)
{
  d->f--;
  if (d->f < 0)
    d->f = d->capacity-1;
  d->size--;
}

void pop_back(struct deque *d)
{
  d->r = (d->r+1)%d->capacity;
  d->size--;
}

int front(struct deque *d)
{
  int aux = d->f - 1;

  if (aux < 0)
    aux = d->capacity-1;
  return d->dq[aux];
}

int back(struct deque *d)
{
  int aux = (d->r+1)%d->capacity;
  return d->dq[aux];
}

int empty(struct deque *d)
{
  return d->size == 0;
}

//Using lemire optimization
void wedge(double *t, double *t2, int len, int r, double *l, double *u) {

  struct deque du, dl;


  init(&du, 2*r+2);
  init(&dl, 2*r+2);

  push_back(&du, 0);
  push_back(&dl, 0);

  for (int i = 1; i < len; i++)
  {
    if (i > r)
    {
      u[i-r-1] = max(t[front(&du)],t2[front(&du)]);
      l[i-r-1] = min(t[front(&dl)],t2[front(&dl)]);
    }
    if (max(t[i],t2[i]) > max(t[i-1],t2[i-1]))
    {
      pop_back(&du);
      while (!empty(&du) && max(t[i],t2[i]) > max(t[back(&du)],t2[back(&du)]))
        pop_back(&du);
    }
    else
    {
      pop_back(&dl);
      while (!empty(&dl) && min(t[i],t2[i]) < min(t[back(&dl)],t2[back(&dl)]))
        pop_back(&dl);
    }
    push_back(&du, i);
    push_back(&dl, i);
    if (i == 2 * r + 1 + front(&du))
      pop_front(&du);
    else if (i == 2 * r + 1 + front(&dl))
      pop_front(&dl);
  }
  for (int i = len; i < len+r+1; i++)
  {
    u[i-r-1] = max(t[front(&du)],t2[front(&du)]);
    l[i-r-1] = min(t[front(&dl)],t2[front(&dl)]);
    if (i-front(&du) >= 2 * r + 1)
      pop_front(&du);
    if (i-front(&dl) >= 2 * r + 1)
      pop_front(&dl);
  }
  destroy(&du);
  destroy(&dl);
}

double kim(double *t, double *q, int j, int len, double mean, double std, double bsf, int* steps) {

  /// 1 point at front and back
  double d, lb;
  double x0 = (t[j] - mean) / std;
  double y0 = (t[(len-1+j)] - mean) / std;
  lb = dist(x0,q[0]) + dist(y0,q[len-1]);
  if (lb >= bsf)   return lb;

  /// 2 points at front
  double x1 = (t[(j+1)] - mean) / std;
  d = min(dist(x1,q[0]), dist(x0,q[1]));
  d = min(d, dist(x1,q[1]));
  lb += d;
  if (lb >= bsf)   return lb;

  /// 2 points at back
  double y1 = (t[(len-2+j)] - mean) / std;
  d = min(dist(y1,q[len-1]), dist(y0, q[len-2]) );
  d = min(d, dist(y1,q[len-2]));
  lb += d;
  if (lb >= bsf)   return lb;

  /// 3 points at front
  double x2 = (t[(j+2)] - mean) / std;
  d = min(dist(x0,q[2]), dist(x1, q[2]));
  d = min(d, dist(x2,q[2]));
  d = min(d, dist(x2,q[1]));
  d = min(d, dist(x2,q[0]));
  lb += d;
  if (lb >= bsf)   return lb;

  /// 3 points at back
  double y2 = (t[(len-3+j)] - mean) / std;
  d = min(dist(y0,q[len-3]), dist(y1, q[len-3]));
  d = min(d, dist(y2,q[len-3]));
  d = min(d, dist(y2,q[len-2]));
  d = min(d, dist(y2,q[len-1]));
  lb += d;
  *steps = *steps + 1;

  return lb;
}

double keogh(int *order, double *t, double *u, double *l,double *cb, int j, int len, double mean, double std, double bsf, int* steps) {
  double lb = 0;
  double x, d;

  for (int i = 0; i < len && lb < bsf; i++)
  {
    x = (t[order[i]+j] - mean) / std;
    d = 0;
    if (x > u[order[i]])
      d = dist(x,u[order[i]]);
    else if(x < l[order[i]])
      d = dist(x,l[order[i]]);
    lb += d;
    cb[order[i]] = d;
    *steps = *steps + 1;
  }
  return lb;
}

double keoghData(int *order, double *tz, double *q,  double *l, double *u, double *cb, int len, double mean, double std, double bsf, int* steps)
{
  double lb = 0;
  double uu,ll,d;
  for (int i = 0; i < len && lb < bsf; i++) {
    uu = (u[order[i]]-mean)/std;
    ll = (l[order[i]]-mean)/std;
    d = 0;
    if (q[i] > uu) {
      d = dist(q[i], uu);
    } else {   
      if (q[i] < ll) {
        d = dist(q[i], ll);
      }
    }
    lb += d;
    cb[order[i]] = d;
    *steps = *steps + 1;
  }
  return lb;
}

double dtw(double* q, double* d, double *cb, int m, int r, double bsf, int* steps) {
  int wl = 2*r+1;
  double *prev = (double *)malloc(sizeof(double)*(wl));

  double *curr = (double *)malloc(sizeof(double)*(wl));
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
      if ((i==0)&&(j==0))
      {
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
      if ((i-1<0)||(k+1>2*r)) {  x = INF; }
      else                    {  x = prev[k+1]; }
      if ((i-1<0)||(j-1<0))   {  z = INF; }
      else                    {  z = prev[k]; }
      double cost = dist(q[i],d[j])+min(x,min(y,z));
      if (cost < best) {
        best = cost;
      }
      curr[k] = cost;
      *steps = *steps + 1;
    }
    if (i+r < m-1 && best /*+cb[i+r+1]*/ >= bsf) {
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

int main(int argc , char *argv[])
{

  double *Q, *q, *T,**C, **co;
  FILE *fp;
  FILE *qp;
  int m = atol(argv[3]);

  int i = 0;
  int j;

  qp = fopen(argv[2],"r");
  Q = (double *)malloc(sizeof(double)*m);
  q = (double *)malloc(sizeof(double)*m);
  T = (double *)malloc(sizeof(double)*m);
  int r;
  double R = atof(argv[4]);
  if (R<=1)
    r = floor(R*m);
  else
    r = floor(R);
  int rotationframe = 0;// floor(0.05*m);
  if (rotationframe == 0) {
    rotationframe = 0;
  }
  cout << "Number of rotation frames: " << 2*rotationframe + 1 << " (" << rotationframe << " on each side)\n";
  C = (double **)malloc(sizeof(double*)*(2*rotationframe+1));
  co = (double **)malloc(sizeof(double*)*(2*rotationframe+1));
  double d;
  double ex = 0, ex2 = 0;
  int steps = 0;

  int numkim = 0;
  int numlb1 = 0;
  int numlb2 = 0;
  int numdtw = 0;
  int **order = (int **)malloc(sizeof(int)*(2*rotationframe+1));

  while(fscanf(qp,"%lf",&d) != EOF && i < m)
  {
    ex += d;
    ex2 += d*d;
    Q[i] = d;
    i++;
    steps++;
  }

  fclose(qp);

  double qmean = ex/m;
  double qstd = ex2/m;
  double mean;
  double std;
  qstd = sqrt(qstd-qmean*qmean);

  double** uo = (double **)malloc(sizeof(double)*(2*rotationframe+1));
  double** lo = (double **)malloc(sizeof(double)*(2*rotationframe+1));
  double** cb = (double **)malloc(sizeof(double)*(2*rotationframe+1));

  double** cb1 = (double **)malloc(sizeof(double)*(2*rotationframe+1));

  double** cb2 = (double **)malloc(sizeof(double)*(2*rotationframe+1));

  for(i = 0 ; i < m ; i++) {
    q[i] = (Q[i] - qmean)/qstd;
  }

  double* u = (double *)malloc(sizeof(double)*m);
  double* l = (double *)malloc(sizeof(double)*m);
  wedge(q, q, m, r, l, u);
  Index *Q_tmp;
  Q_tmp = (Index *)malloc(sizeof(Index)*m);
  /// Sort the query one time by abs(z-norm(q[i]))
  for(i = 0; i<m; i++)
  {
    Q_tmp[i].value = q[i];
    Q_tmp[i].index = i;
  }
  
  qsort(Q_tmp, m, sizeof(Index),comp);
  for (int b = 0; b < 2*rotationframe+1; b++) {
    C[b] = (double *) malloc(sizeof(double)*m);
    co[b] = (double *) malloc(sizeof(double)*m);
    uo[b] = (double *) malloc(sizeof(double)*m);
    lo[b] = (double *) malloc(sizeof(double)*m);
    cb[b] = (double *) malloc(sizeof(double)*m);
    cb1[b] = (double *) malloc(sizeof(double)*m);
    cb2[b] = (double *) malloc(sizeof(double)*m);
    order[b] = (int *) malloc(sizeof(double)*m);
    steps++;
  }
  for (int w = -rotationframe, v = 0; w <= rotationframe; w++, v++) {
    for(i = 0;i < m;i++) {
      double znorm = (Q[i] - qmean)/qstd;
      int offset = (w+i)%m;
      if (offset < 0) {
        offset += m;
      }
      C[v][offset] = znorm;
      int o = Q_tmp[i].index;
      order[v][offset] = o;
      co[v][offset] = q[o];
      uo[v][offset] = u[o];
      lo[v][offset] = l[o];
      cb[v][offset] = 0;
      cb1[v][offset] = 0;
      cb2[v][offset] = 0;
      steps++;
    }
    wedge(C[v],C[v], m, r, lo[v], uo[v]);
  }
  free(Q);
  free(q);

  double t1 = clock();
  T = (double *)malloc(sizeof(double)*2*m);


  double *tz = (double *)malloc(sizeof(double)*m);

  double dist = 0;
  long int loc;
  i = 0;
  j = 0;
  int k = 0;
  ex = ex2 = 0;
  double threshold = INF;
  double lbkim = 0;
  double bsf = threshold;
  int rot = -1;

  fp = fopen(argv[1],"r");
  double* ubuffer = (double *)malloc(sizeof(double)*1000000);
  double* lbuffer = (double *)malloc(sizeof(double)*1000000);
  int length=0;
  double* buffer = (double *)malloc(sizeof(double)*1000000);
  while(fscanf(fp,"%lf",&d) != EOF) {
    buffer[length] = d;
    steps++;
    length++;
  }
  wedge(buffer,buffer, length, r, lbuffer, ubuffer);
  for (i=0;i<length;i++) {
    d = buffer[i];

    ex += d;
    ex2 += d*d;
    T[i%m] = d;
    T[(i%m)+m] = d;
    if(i >= m-1) {
      j = (i+1)%m;
      int I = i-(m-1);

      mean = ex/m;
      std = ex2/m;
      std = sqrt(std-mean*mean);
      for (int ro = 0; ro < 2*rotationframe+1; ro++) {
        double lbkim = kim(T, C[ro],j, m, mean, std, bsf, &steps);
        if (lbkim < bsf) {

          double lbkeogh = keogh(order[ro], T, uo[ro], lo[ro],cb1[ro], j, m, mean, std, bsf, &steps);
          if (lbkeogh < bsf) {
            for(k=0;k<m;k++) {   
              tz[k] = (T[k+j] - mean)/std;
              steps++;
            }
            double lbkeogh2 = keoghData(order[ro], tz, co[ro], lbuffer+I, ubuffer+I,cb2[ro], m, mean, std, bsf, &steps);
            if (lbkeogh2 < bsf) {
              if (lbkeogh > lbkeogh2)
              {
                cb[ro][m-1]=cb1[ro][m-1];
                for(k=m-2; k>=0; k--)
                  cb[ro][k] = cb[ro][k+1]+cb1[ro][k];
              }
              else
              {
                cb[ro][m-1]=cb2[ro][m-1];
                for(k=m-2; k>=0; k--)
                  cb[ro][k] = cb[ro][k+1]+cb2[ro][k];
              }
              dist = dtw(C[ro],tz,cb[ro], m,r, bsf, &steps);
              if(dist < bsf) {
                rot = rotationframe;
                bsf = dist;
                loc = i-m+1;
              }
              numdtw++;
            } else {
              numlb2++;
            }
          } else {
            numlb1++;
          }
        } else {
          numkim++;
        }

        steps++;
      }

      ex -= T[j];
      ex2 -= T[j]*T[j];
    }
  }
  free(uo);
  free(lo);
  fclose(fp);
  char end1 = '\n';
  double t2 = clock();
  if (rot != -1) {
    cout << "Rotation: " << rot << endl;
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
  } else {
    cout << "No match found\n";
  }

  free(T);
  free(tz);
  free(C);
  free(buffer);
  return 0;
}

