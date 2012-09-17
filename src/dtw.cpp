#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include<vector>
#include<deque>
#include<cassert>
#include <iostream>

#define INF 1e20;

using namespace std;

double min(double x, double y) { 
  return x < y ? x : y;
}
double max(double x, double y) { 
  return x > y ? x : y;
}
double dist(double x, double y) {
  return (x-y)*(x-y);
}

void printarray(int *arr, int l) {
  cout << "[ ";
  string delimiter = "";
  for (int a = 0;a < l; a++) {
    cout << delimiter << arr[a];
    delimiter = " ";
  }
  cout << " ]\n";
}

void printarray(double* arr, int l) {
  cout << "[ ";
  string delimiter = "";
  for (int a = 0;a < l; a++) {
    cout << delimiter << arr[a];
    delimiter = " ";
  }
  cout << " ]\n";
}

void printdeque(deque<double> deque) {
  cout << "[ ";
  string delimiter = "";
  for (int a = 0;a < deque.size(); a++) {
    cout << delimiter << deque[a];
    delimiter = " ";
  }
  cout << " ]\n";
}

struct Index {   
  double value;
  int index;
};
typedef Index index;

int compare(const void *a, const void *b) {
  index* x = (index*) a;
  index* y = (index*) b;
  return abs(x->value)-abs(y->value);
}

double dtw(double *q, double *d, double *cb, int m, int r, double bsf, int* steps) {

  int wl = 2*r+1;

  double *prev, *curr, *temp;
  prev = (double*)malloc(sizeof(double)*(2*r+1));
  curr = (double*)malloc(sizeof(double)*(2*r+1));

  for (int i = 0;i < wl; i++) {
    curr[i] = 0;
    prev[i] = 0;
  }

  int i,j,k; 
  double x,y,z;
  double best;

  for (i = 0; i < m; i++) {
    k = max(0,r-i);
    best = INF;
    for(j = max(0,i-r); j <= min(m-1,i+r); j++, k++) {
      if ((i == 0) && (j == 0)) {
        curr[k] = dist(q[0], d[0]);
        best = curr[k];
        continue;
      }
      if ((j - 1 < 0) || (k - 1 < 0)) {
        y = INF;
      } else {
        y = curr[k-1]; 
      }
      if ((i - 1 < 0)||(k + 1 > 2*r)) {  
        x = INF; 
      } else {  
        x = prev[k+1]; 
      }
      if ((i - 1 < 0)||(j - 1 < 0)) {  
        z = INF; 
      } else {  
        z = prev[k]; 
      }
      double cost = dist(q[i],d[j])+min(x,min(y,z));
      if (cost < best) {
        best = cost;
      }
      curr[k] = cost;
      *steps = *steps + 1;
    }
    if (i + r < m-1  && cb[i+r+1] >= bsf) {
      return best+cb[i+r+1];
    }
    temp = curr;
    curr = prev;
    prev = temp;
  }
  free(curr);
  free(prev);
  double ret = prev[k-1];
  return ret;
}

double lb_keogh_data(int *order, double *tz, double *q,  double *l, double *u, double *cb, int len, double mean, double std, double bsf, int* steps) {
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

double lb_keogh(int *order, double *t, double *u, double *l, double *cb, int j, int len, double mean, double std, double bsf, int* steps) {

  double lb = 0;
  double x, d;


  for (int i = 0; i < len && lb < bsf; i++) {
    x = (t[order[i]+j] - mean) / std;
    d = 0;
    if (x > u[order[i]]) {
      d = dist(x,u[order[i]]);
    } else if(x < l[order[i]]) {
      d = dist(x,l[order[i]]);
    }
    lb += d;
    cb[order[i]] = d;
    *steps = *steps + 1;
  }
  return lb;
}

void wedge(double *t, double *t2, int len, int r, double *l, double *u, int *steps) {
  deque<int> du, dl;
  du.push_back(0);
  dl.push_back(0);
  for (int i = 1; i < len; i++) {
    if (i > r) {
      u[i-r-1] = max(t[du[0]], t2[du[0]]);
      l[i-r-1] = max(t[dl[0]], t2[dl[0]]);
    }
    if (max(t[i],t2[i]) > max(t[i-1], t2[i-1])) {
      assert(du.size() > 0);
      du.pop_back();
      while (du.size() != 0 && max(t[i], t2[i]) > max(t[du.size()-1],t2[du.size()-1])) {
        assert(du.size() > 0);
        du.pop_back();
      }
    } else {
      assert(dl.size() > 0);
      dl.pop_back();
      while (dl.size() != 0 && max(t[i], t2[i]) > max(t[dl.size()-1],t2[dl.size()-1])) {
        assert(dl.size() > 0);
        dl.pop_back();
      }
    }
    du.push_back(i);
    dl.push_back(i);
    if (i == 2*r+1 + du[0]) {
      du.pop_front();
    } else if (i == 2*r+1 + dl[0]) {
      dl.pop_front();
    }
    *steps = *steps + 1;
  }
  for (int i=len; i<len+r+1; i++) {
    u[i-r-1] = max(t[du[0]],t2[du[0]]);
    l[i-r-1] = min(t[dl[0]],t2[dl[0]]);
    if (i-du[0] >= 2*r+1) {
      du.pop_front();
    }
    if (i-dl[0] >= 2*r+1) {
      dl.pop_front();
    }
    *steps = *steps + 1;
  }
}

double lb_kim(double *t, double *q, int j, int len, double mean, double std, double bsf, int* steps) {

  double d, lb;
  double x0 = (t[j] - mean) / std;
  double y0 = (t[(len-1+j)] - mean) / std;
  lb = dist(x0,q[0]) + dist(y0,q[len-1]);
  if (lb >= bsf) {  
    return lb;
  }

  double x1 = (t[(j+1)] - mean) / std;
  d = min(dist(x1,q[0]), dist(x0,q[1]));
  d = min(d, dist(x1,q[1]));
  lb += d;
  if (lb >= bsf) {  
    return lb;
  }

  double y1 = (t[(len-2+j)] - mean) / std;
  d = min(dist(y1,q[len-1]), dist(y0, q[len-2]) );
  d = min(d, dist(y1,q[len-2]));
  lb += d;
  if (lb >= bsf) {  
    return lb;
  }

  double x2 = (t[(j+2)] - mean) / std;
  d = min(dist(x0,q[2]), dist(x1, q[2]));
  d = min(d, dist(x2,q[2]));
  d = min(d, dist(x2,q[1]));
  d = min(d, dist(x2,q[0]));
  lb += d;
  if (lb >= bsf) {   
    return lb;
  }

  double y2 = (t[(len-3+j)] - mean) / std;
  d = min(dist(y0,q[len-3]), dist(y1, q[len-3]));
  d = min(d, dist(y2,q[len-3]));
  d = min(d, dist(y2,q[len-2]));
  d = min(d, dist(y2,q[len-1]));
  lb += d;

  *steps = *steps + 1;

  return lb;
}

int main(int argc, char *argv[]) {
  int steps = 0;

  FILE *dp, *qp;
  qp = fopen(argv[2], "r");
  dp = fopen(argv[1], "r");

  double window = 1;
  if (argc > 1) {
    window = atol(argv[3]);
  }

  double *query, *querysort;
  query = (double*) malloc(sizeof(double)*window);
  querysort = (double*) malloc(sizeof(double)*window);

  double d;
  double qsum = 0, qsq = 0;
  int counter = 0;

  while(fscanf(qp,"%lf",&d) != EOF && counter < window) {
    query[counter] = d;
    double prev = qsum;
    qsum += d;
    qsq += d*d;
    counter++;
  }

  int qsize = counter;

  double qmean = qsum/qsize;
  double qstd = qsq/qsize;
  qstd = sqrt(qstd-qmean*qmean);
  for (int i = 0;i<qsize;i++) {
    if (qstd != 0) {
      query[i] = (query[i]-qmean)/qstd;
    } else {
      query[i] = 0;
    }
  }


  double r;
  double R = atof(argv[4]);
  if (R<=1)
    r = floor(R*qsize);
  else
    r = floor(R);

  double *ql, *qu;
  ql = (double*)malloc(sizeof(double)*qsize);
  qu = (double*)malloc(sizeof(double)*qsize);

  index *sorted;
  int *order;

  order = (int*)malloc(sizeof(int)*qsize);
  sorted = (index*)malloc(sizeof(index)*qsize);

  double *cb, *cb1, *cb2;

  cb = (double*)malloc(sizeof(double)*qsize);
  cb1 = (double*)malloc(sizeof(double)*qsize);
  cb2 = (double*)malloc(sizeof(double)*qsize);

  for (int i = 0; i < qsize; i++) {
    ql[i] = 0;
    qu[i] = 0;
    index ind;
    ind.value = query[i];
    ind.index = i;
    sorted[i] = ind;
    cb[i] = 0;
    cb1[i] = 0;
    cb2[i] = 0;
    querysort[i] = 0;
  }


  int numkim = 0;
  int numlb1 = 0;
  int numlb2 = 0;
  int numdtw = 0;



  qsort(sorted, qsize, sizeof(index), compare);

  for (int i = 0; i < qsize; i++) {
    int o = sorted[i].index;
    order[i] = o;
    querysort[i] = query[o];
  }


  wedge(query, query, qsize, r, ql, qu, &steps);


  double *data, *zdata;

  zdata = (double*)malloc(sizeof(double)*(2*qsize));
  data = (double*)malloc(sizeof(double)*(2*qsize));


  for (int i = 0; i < 2*qsize; i++) {
    data[i] = 0;
    if (i < qsize) {
      zdata[i] = 0;
    }
  }

  double *buffer, *bl, *bu;
  long EPOCH_SIZE = 1000000;

  buffer = (double*)malloc(sizeof(double)*EPOCH_SIZE);
  bl = (double*)malloc(sizeof(double)*EPOCH_SIZE);
  bu = (double*)malloc(sizeof(double)*EPOCH_SIZE);

  double start = clock();
  int dcounter = 0;
  while(fscanf(dp,"%lf",&d) != EOF) {
    buffer[dcounter] = d;
    bl[dcounter] = 0;
    bu[dcounter] = 0;
    dcounter++;
    steps++;
  }

  int length = dcounter;
  
  wedge(buffer, buffer, length, r, bl, bu, &steps);

  double dsum = 0, dsq = 0;
  double dmean = 0;
  double dstd = 0;

  int offset, actual;


  double threshold = INF;
  double bsf = threshold;

  double distance, location;

  for (int i = 0; i < length; i++) {
    d = buffer[i]; 
    double prev = dsum;
    dsum = dsum + d;
    dsq = dsq + d*d;

    data[i%qsize] = d;
    data[(i%qsize) + qsize] = d;

    if (i >= qsize - 1) {
      offset = (i + 1) % qsize;
      actual = i - (qsize - 1);


      dmean = dsum/qsize;
      dstd = dsq/qsize;
      dstd = sqrt(dstd-dmean*dmean);
      double kim = lb_kim(data, query, offset, qsize, dmean, dstd, bsf, &steps);
      if (kim < bsf) {
        double keogh = lb_keogh(order, data, qu, ql,cb1, offset, qsize, dmean, dstd, bsf, &steps);
        if (keogh < bsf) {
          for (int j = 0; j < qsize; j++) {
            zdata[j] = (data[j+offset] - dmean)/dstd;
            steps++;
          }
          double keogh2 = lb_keogh_data(order, zdata, querysort, bu+actual, bl+actual, cb2, qsize, dmean, dstd, bsf, &steps); 
          if (keogh2 < bsf) {
            if (keogh > keogh2) {
              cb[qsize-1]=cb1[qsize-1];
              for(int k = qsize - 2; k >= 0; k--) {
                cb[k] = cb[k+1]+cb1[k];
              }
            } else {
              cb[qsize-1]=cb2[qsize-1];
              for(int k = qsize - 2; k >= 0; k--) {
                cb[k] = cb[k+1]+cb2[k];
              }
            }
            distance = dtw(query, zdata, cb, qsize, r, bsf, &steps);
            if (distance < bsf) {
              bsf = distance;
              location = i - qsize + 1;
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
      dsum -= data[offset];
      dsq -= data[offset]*data[offset];
    }
  }
  double end = clock();


  cout << "Query: ";
  cout << "[ ";
  for (int a = 0;a < qsize; a++) {
    printf("%g ", query[a]*qstd+qmean);
  }
  cout << "]\n";
  cout << "Match: ";
  cout << "[ ";
  for (int a = location;a < location+qsize; a++) {
    printf("%g ", buffer[a]);
  }
  cout << "]\n";
  cout << "Location : " << location << endl;
  cout << "Distance : " << sqrt(bsf) << endl;
  cout << "Data Scanned : " << length << endl;
  cout << "Num Steps: " << steps << endl;
  cout << "Total Execution Time : " << (end - start)/CLOCKS_PER_SEC << " sec" << endl;
  cout << "--------------------------" << endl;
  cout << "Kim: " << numkim << endl;
  cout << "Keogh1: " << numlb1 << endl;
  cout << "Keogh2: " << numlb2<< endl;
  cout << "DTW: " << numdtw << endl;

  free(query);
  free(querysort);
  free(ql);
  free(qu);
  free(order);
  free(sorted);
  free(cb);
  free(cb1);
  free(cb2);
  free(zdata);
  free(data);
  free(buffer);
  free(bl);
  free(bu);
  return 0;
}
