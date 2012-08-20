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

double kim(double *t, double *q, int j, int len, double mean, double std, double bsf) {

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

  return lb;
}

double keogh(double *t, double *u, double *l, int j, int len, double mean, double std, double bsf) {
  double lb = 0;
  double x, d;

  for (int i = 0; i < len && lb < bsf; i++)
  {
    x = (t[i+j] - mean) / std;
    d = 0;
    if (x > u[i])
      d = dist(x,u[i]);
    else if(x < l[i])
      d = dist(x,l[i]);
    lb += d;
    //cb[i] = d;
  }
  return lb;
}

double keoghData(double *tz, double *q,  double *l, double *u, int len, double mean, double std, double bsf)
{
  double lb = 0;
  double uu,ll,d;
  for (int i = 0; i < len && lb < bsf; i++) {
    uu = (u[i]-mean)/std;
    ll = (l[i]-mean)/std;
    d = 0;
    if (q[i] > uu) {
      d = dist(q[i], uu);
    } else {   
      if (q[i] < ll) {
        d = dist(q[i], ll);
      }
    }
    lb += d;
    //cb[i] = d;
  }
  return lb;
}

double dtw(double* q, double* d, int m, int r, double bsf, int* steps) {
  int wl = 2*r+1;//(2*r)+1;
  double *prev = (double *)malloc(sizeof(double)*(wl));
  double *curr = (double *)malloc(sizeof(double)*(wl));
  double *temp;
  for (int i = 0;i < wl; i++) {
    curr[i] = INF;
    prev[i] = INF;
  }
  int i,j,k; 
  double x,y,z;
  for (i = 0; i < m; i++) {
    k = max(0,r-i);
    double best = INF;
    for(j=max(0,i-r); j<=min(m-1,i+r); j++, k++) {
      if ((i==0)&&(j==0))
      {
        curr[k]=dist(q[0],d[0]);
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
    if (i+r>m-1 && best >= bsf) {
      free(curr);
      free(prev);
      return best;
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

  double *Q, *T,*u,*l, **C;
  FILE *fp;
  FILE *qp;
  int m = atol(argv[3]);

  int i = 0;
  int j;

  qp = fopen(argv[2],"r");
  Q = (double *)malloc(sizeof(double)*m);
  T = (double *)malloc(sizeof(double)*m);
  int r = floor(0.05*m);
  if (r == 0) {
    r = 1;
  }
  int rotationframe = floor(0.05*m);
  if (rotationframe == 0) {
    rotationframe = 1;
  }
  cout << "Number of rotation frames: " << 2*rotationframe + 1 << " (" << rotationframe << " on each side)\n";
  C = (double **)malloc(sizeof(double*)*(2*rotationframe+1));
  double d;
  double ex = 0, ex2 = 0;
  int steps = 0;

  int numkim = 0;
  int numlb1 = 0;
  int numlb2 = 0;
  int numdtw = 0;

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
  for (int b = 0; b < 2*rotationframe+1; b++) {
    C[b] = (double *) malloc(sizeof(double)*m);
  }
  for(i = 0;i < m;i++) {
    double znorm = (Q[i] - qmean)/qstd;
    for (int w = -rotationframe, v = 0; w <= rotationframe; w++, v++) {
      int offset = (w+i)%m;
      if (offset < 0) {
        offset += m;
      }
      C[v][offset] = znorm;
    }
    steps++;
  }
  free(Q);

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
  double* uo = (double *)malloc(sizeof(double)*m);
  double* lo = (double *)malloc(sizeof(double)*m);
  double* ubuffer = (double *)malloc(sizeof(double)*1000000);
  double* lbuffer = (double *)malloc(sizeof(double)*1000000);
  int length=0;
  double* buffer = (double *)malloc(sizeof(double)*1000000);
  while(fscanf(fp,"%lf",&d) != EOF && length <= 5000)
  {
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
        double lbkim = kim(T, C[ro],j, m, mean, std, bsf);
        if (lbkim < bsf) {
          wedge(C[ro],C[ro], m, r, lo, uo);
          double lbkeogh= keogh(T, uo, lo, j, m, mean, std, bsf);
          if (lbkeogh < bsf) {
            for(k=0;k<m;k++) {   
              tz[k] = (T[k+j] - mean)/std;
              steps++;
            }
            double lbkeogh2 = keoghData(tz, C[ro], lbuffer+I, ubuffer+I, m, mean, std, bsf);
            if (lbkeogh2 < bsf) {
              dist = dtw(C[ro],tz,m,r, bsf, &steps);
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

