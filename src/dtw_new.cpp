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

void printvector(vector<double> vector) {
  cout << "[ ";
  string delimiter = "";
  for (int a = 0;a < vector.size(); a++) {
    cout << delimiter << vector[a];
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

double dtw(vector<double> *q, vector<double> *d, vector<double> *cb, int m, int r, double bsf, int* steps) {

  int wl = 2*r+1;

  vector<double> prev, curr, temp;

  for (int i = 0;i < wl; i++) {
    curr.push_back(1e20);
    prev.push_back(1e20);
  }

  int i,j,k; 
  double x,y,z;
  double best;

  for (i = 0; i < m; i++) {
    k = max(0,r-i);
    best = INF;
    for(j = max(0,i-r); j <= min(m-1,i+r); j++, k++) {
      if ((i == 0) && (j == 0)) {
        curr[k] = dist((*q)[0], (*d)[0]);
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
      double cost = dist((*q)[i],(*d)[j])+min(x,min(y,z));
      if (cost < best) {
        best = cost;
      }
      curr[k] = cost;
      *steps = *steps + 1;
    }
    if (i + r < m-1 && best ) { //+ (*cb)[i+r+1] >= bsf) {
      return best;//+(*cb)[i+r+1];
    }
    temp = curr;
    curr = prev;
    prev = temp;
  }
  double ret = prev[k-1];
  return ret;
}

double lb_keogh_data(vector<int> *order, vector<double> *tz, vector<double> *q,  vector<double> *l, vector<double> *u, vector<double> *cb, int len, double mean, double std, double bsf, int* steps) {
  double lb = 0;
  double uu,ll,d;
  for (int i = 0; i < len && lb < bsf; i++) {
    uu = ((*u)[(*order)[i]]-mean)/std;
    ll = ((*l)[(*order)[i]]-mean)/std;
    d = 0;
    if ((*q)[i] > uu) {
      d = dist((*q)[i], uu);
    } else {   
      if ((*q)[i] < ll) {
        d = dist((*q)[i], ll);
      }
    }
    lb += d;
    (*cb)[(*order)[i]] = d;
    *steps = *steps + 1;
  }
  return lb;
}

double lb_keogh(vector<int> *order, vector<double> *t, vector<double> *u, vector<double> *l, vector<double> *cb, int j, int len, double mean, double std, double bsf, int* steps) {

  double lb = 0;
  double x, d;

  for (int i = 0; i < len && lb < bsf; i++) {
    x = ((*t)[(*order)[i]+j] - mean) / std;
    d = 0;
    if (x > (*u)[(*order)[i]]) {
      d = dist(x,(*u)[(*order)[i]]);
    } else if(x < (*l)[(*order)[i]]) {
      d = dist(x,(*l)[(*order)[i]]);
    }
    lb += d;
    (*cb)[(*order)[i]] = d;
    *steps = *steps + 1;
  }
  return lb;
}

void wedge(vector<double> *t, vector<double> *t2, int len, int r, vector<double> *l, vector<double> *u, int *steps) {
  deque<double> du, dl;
  du.push_back(0);
  dl.push_back(0);
  for (int i=1; i < len; i++) {
    if (i > r) {
      (*u)[i-r-1] = max((*t)[du[0]], (*t2)[du[0]]);
      (*l)[i-r-1] = max((*t)[dl[0]], (*t2)[dl[0]]);
    }
    if (max((*t)[i],(*t2)[i]) > max((*t)[i-1], (*t2)[i-1])) {
      printvector(*t);
      printvector(*t2);
      assert(du.size() > 0);
      du.pop_back();
      int size = du.size();
      while (size != 0 && max((*t)[i], (*t2)[i]) > max((*t)[size-1],(*t2)[size-1])) {
        printdeque(du);
        assert(du.size() > 0);
        du.pop_back();
      }
    } else {
      assert(dl.size() > 0);
      dl.pop_back();
      int size = du.size();
      while (size != 0 && max((*t)[i], (*t2)[i]) > max((*t)[size-1],(*t2)[size-1])) {
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
    (*u)[i-r-1] = max((*t)[du[0]],(*t2)[du[0]]);
    (*l)[i-r-1] = min((*t)[dl[0]],(*t2)[dl[0]]);
    if (i-du[0] >= 2*r+1) {
      du.pop_front();
    }
    if (i-dl[0] >= 2*r+1) {
      dl.pop_front();
    }
    *steps = *steps + 1;
  }
}

double lb_kim(vector<double> *t, vector<double> *q, int j, int len, double mean, double std, double bsf, int* steps) {

  double d, lb;
  double x0 = ((*t)[j] - mean) / std;
  double y0 = ((*t)[(len-1+j)] - mean) / std;
  lb = dist(x0,(*q)[0]) + dist(y0,(*q)[len-1]);
  if (lb >= bsf) {  
    return lb;
  }

  double x1 = ((*t)[(j+1)] - mean) / std;
  d = min(dist(x1,(*q)[0]), dist(x0,(*q)[1]));
  d = min(d, dist(x1,(*q)[1]));
  lb += d;
  if (lb >= bsf) {  
    return lb;
  }

  double y1 = ((*t)[(len-2+j)] - mean) / std;
  d = min(dist(y1,(*q)[len-1]), dist(y0, (*q)[len-2]) );
  d = min(d, dist(y1,(*q)[len-2]));
  lb += d;
  if (lb >= bsf) {  
    return lb;
  }

  double x2 = ((*t)[(j+2)] - mean) / std;
  d = min(dist(x0,(*q)[2]), dist(x1, (*q)[2]));
  d = min(d, dist(x2,(*q)[2]));
  d = min(d, dist(x2,(*q)[1]));
  d = min(d, dist(x2,(*q)[0]));
  lb += d;
  if (lb >= bsf) {   
    return lb;
  }

  double y2 = ((*t)[(len-3+j)] - mean) / std;
  d = min(dist(y0,(*q)[len-3]), dist(y1, (*q)[len-3]));
  d = min(d, dist(y2,(*q)[len-3]));
  d = min(d, dist(y2,(*q)[len-2]));
  d = min(d, dist(y2,(*q)[len-1]));
  lb += d;

  *steps = *steps + 1;

  return lb;
}

int main(int argc, char *argv[]) {
  int steps = 0;

  FILE *dp, *qp;
  qp = fopen(argv[2], "r");
  dp = fopen(argv[1], "r");

  vector<double> query, querysort;
  double d;
  fscanf(qp,"%lf",&d);

  double qsum = d, qsq = 0;
  int counter = 1;

  while(fscanf(qp,"%lf",&d) != EOF) {
    query.push_back(d);
    double prev = qsum;
    qsum = prev + (d-qsum)/counter;
    qsq = qsq + (d-qsum)*(d-prev); 
    counter++;
  }

  int qsize = query.size();

  double qmean = qsum;
  double qstd = sqrt(qsq/(qsize-1));
  for (int i = 0;i<qsize;i++) {
    if (qstd != 0) {
      query[i] = (query[i]-qmean)/qstd;
    } else {
      query[i] = 0;
    }
  }

  double r = 0.05 * qsize;
  if (argc > 1) {
    r = atol(argv[3]) * qsize;
  }
  if (r == 0) {
    r = 1;
  }

  vector<double> ql, qu;
  for (int i = 0; i < qsize; i++) {
    ql.push_back(0);
    qu.push_back(0);
  }


  int numkim = 0;
  int numlb1 = 0;
  int numlb2 = 0;
  int numdtw = 0;


  printf("  Sorting query...\n");

  vector<index> sorted;
  vector<int> order;

  vector<double> cb, cb1, cb2;

  for(int i = 0; i < qsize; i++) {
    index ind;
    ind.value = query[i];
    ind.index = i;
    sorted.push_back(ind);
    cb.push_back(0);
    cb1.push_back(0);
    cb2.push_back(0);
    querysort.push_back(0);
  }
  
  qsort(&sorted, qsize, sizeof(index), compare);

  for (int i = 0; i < qsize; i++) {
    int o = sorted[i].index;
    order.push_back(o);
    querysort[i] = query[o];
  }

  wedge(&query, &query, qsize, r, &ql, &qu, &steps);

  double start = clock();

  vector<double> data, zdata;


  for (int i = 0; i < 2*qsize; i++) {
    data.push_back(0);
    if (i < qsize) {
      zdata.push_back(0);
    }
  }

  vector<double> buffer, bl, bu;


  while(fscanf(dp,"%lf",&d) != EOF) {
    buffer.push_back(d);
    bl.push_back(0);
    bu.push_back(0);
    steps++;
  }

  int length = buffer.size();
  
  wedge(&buffer, &buffer, length, r, &bl, &bu, &steps);

  d = buffer[0];
  double dsum = d, dsq = 0;

  int offset, actual;


  double threshold = INF;
  double bsf = threshold;

  double distance, location;

  for (int i = 1; i < length; i++) {
    d = buffer[i]; 
    double prev = dsum;
    dsum = prev + (d-dsum)/(i+1);
    dsq = dsq + (d-dsum)*(d-prev); 

    data[i%qsize] = d;
    data[(i%qsize) + qsize] = d;

    if (i >= qsize - 1) {
      offset = (i + 1) % qsize;
      actual = i - (qsize - 1);

      double dmean = dsum;
      double dstd = sqrt(dsq/(qsize-1));

      double kim = lb_kim(&data, &query, offset, qsize, dmean, dstd, bsf, &steps);
      if (kim < bsf) {
        double keogh = lb_keogh(&order, &data, &ql, &qu,&cb1, offset, qsize, dmean, dstd, bsf, &steps);
        if (keogh < bsf) {
          for (int j = 0; j < qsize; j++) {
            zdata[j] = (data[j+offset] - dmean)/dstd;
            steps++;
          }
          double keogh2 = lb_keogh_data(&order, &zdata,&querysort, &bl, &bu, &cb2, qsize, dmean, dstd, bsf, &steps); 
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
            distance = dtw(&query, &zdata, &cb, qsize, r, bsf, &steps);
            if (distance < bsf) {
              bsf = distance;
              location = i - qsize + 1;
            }
          }
        }
      }

    }
  }
  double end = clock();

  cout << "Location : " << location << endl;
  cout << "Distance : " << sqrt(bsf) << endl;
  cout << "Data Scanned : " << length << endl;
  cout << "Num Steps: " << steps << endl;
  cout << "Total Execution Time : " << (end - start)/CLOCKS_PER_SEC << " sec" << endl;
  cout << "--------------------------" << endl;
  return 0;
}
