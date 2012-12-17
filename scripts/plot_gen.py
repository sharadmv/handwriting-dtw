#!/usr/bin/python

import sys, getopt
import matplotlib
from matplotlib import pyplot as plt
matplotlib.rcParams['legend.fancybox'] = True
from pylab import np

def main(argv):
    letters = ["gen/"+x+"-0" for x in process_args(argv)]
    to_plot = []

    plot = plt.subplot(3,1,1)
    for letter in letters:
        data = np.loadtxt(letter,delimiter=" ");
        print data
        plot.plot(data, label=letter[4:-2])

    plot.legend(shadow=True,fancybox=True)
    plt.draw()
    plt.show()

def process_args(argv):
    try:
        opts, args = getopt.getopt(argv,"hi:o:",[])
    except getopt.GetoptError:
        print 'plot.py <letter1> <letter2> ...'
        sys.exit(2)
        for opt, arg in opts:
            if opt == '-h':
                print 'plot.py <letter1> <letter2> ...'
                sys.exit()
    return args

if __name__ == "__main__":
    main(sys.argv[1:])


def process_args(argv):
    try:
        opts, args = getopt.getopt(argv,"hi:o:",[])
    except getopt.GetoptError:
        print 'plot.py <letter1> <letter2> ...'
        sys.exit(2)
        for opt, arg in opts:
            if opt == '-h':
                print 'plot.py <letter1> <letter2> ...'
                sys.exit()
    return args

if __name__ == "__main__":
    main(sys.argv[1:])
