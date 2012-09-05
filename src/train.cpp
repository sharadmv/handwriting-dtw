#include<stdio.h>
#include <stdlib.h>
#include<dirent.h>
#include<math.h>
#include<vector>
#include<deque>
#include<iostream>
#include<fstream>

using namespace std;

const string TRAIN_DIR = "train/";
const string GEN_DIR = "gen/";

void error(int code) {
  if (code == 0) {
    printf("No training directory found...\n");
  } else if (code == 1) {
    printf("Training file not found...\n");
  }
  exit(1);
}

vector<string> read_train() {
  DIR* dir;
  string dirname = TRAIN_DIR;
  dir = opendir(dirname.c_str());
  vector<string> files;
  if (dir == NULL) {
    error(0);
  }
  struct dirent *d;
  while ((d = readdir(dir)) != NULL) {
    string file = string(d->d_name); 
    if (file != "." && file != "..") {
      files.push_back(file);
    }
  }
  return files;
}

int main(int argc, char *argv[]) {
  vector<string> files = read_train();
  FILE* file;
  ofstream output;
  for (int i = 0; i < files.size(); i++) {
    cout << "Reading...: " << TRAIN_DIR + files[i] << "\n";
    file = fopen((TRAIN_DIR+files[i]).c_str(), "r");
    deque<vector<double> > points;
    double x,y;
    output.open((GEN_DIR+files[i]).c_str());
    while(fscanf(file,"%lf,%lf ",&x,&y) != EOF) {
      vector<double> point;
      point.push_back(x);
      point.push_back(y);
      points.push_back(point);
      if (points.size() > 3) {
        points.pop_front();
      }
      if (points.size() == 3) {
        vector<double> v1, v2;
        v1.push_back(points[0][0] - points[1][0]);
        v1.push_back(points[0][1] - points[1][1]);
        v2.push_back(points[2][0] - points[1][0]);
        v2.push_back(points[2][1] - points[1][1]);
        double dot = v1[0]*v2[0]+v1[1]*v2[1];
        double mag1 = sqrt(v1[0]*v1[0]+v1[1]*v1[1]);
        double mag2 = sqrt(v2[0]*v2[0]+v2[1]*v2[1]);
        double angle = acos(dot/(mag1*mag2));
        output << angle << endl;
      }
    }
    cout << "Outputting: " << GEN_DIR + files[i] << endl;
    output.close();
  }
  return 0;
}
