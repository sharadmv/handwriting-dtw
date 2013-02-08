#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<vector>
#include<cassert>
#include<iostream>
#include<dirent.h>
#include<string>
#include<algorithm>
#include<sstream>
#include<iostream>
#include<fstream>
#include<algorithm>

using namespace std;

typedef struct {
    double x;
    double y;
} point;

int main(int argc, char *argv[]) {
    string trainDir = argv[1];
    string normalDir = "normal/";
    DIR *mydir = opendir(trainDir.c_str());
    struct dirent *entry = NULL;
    FILE *qp;
    double x,y;
    int i;
    point *q = (point *) malloc(sizeof(point)*2000);
    ofstream output;
    while((entry = readdir(mydir))) {
        string tmp = entry->d_name;
        if (tmp != "." && tmp != "..") {
            double exx = 0, ex2x = 0, exy = 0 , ex2y = 0;
            qp = fopen((trainDir+tmp).c_str(),"r");
            i = 0;
            while(fscanf(qp,"%lf %lf",&x, &y) != EOF) {
                point temp;
                temp.x = x;
                temp.y = y;
                q[i] = temp;
                exx += x;
                ex2x += x*x;
                exy += y;
                ex2y += y*y;
                i++;
            }
            double meanx = exx/i;
            double stdx = ex2x/i;
            stdx = sqrt(stdx-meanx*meanx);

            double meany = exy/i;
            double stdy = ex2y/i;
            stdy = sqrt(stdy-meany*meany);



            output.open((normalDir+tmp).c_str());

            double normx, normy;
            for (int j = 0; j < i; j++) {
                normx = (q[j].x - meanx)/stdx;
                normy = (q[j].y - meany)/stdy;
                output << normx << " " << normy << endl;
            }
            output.close();
        }
    }
    return 0;
}
