#!/usr/bin/python

import sys, getopt
import matplotlib
from matplotlib import pyplot as plt
from matplotlib.pyplot import *
matplotlib.rcParams['legend.fancybox'] = True
matplotlib.rcParams['figure.figsize'] = 5, 10
from pylab import np
from matplotlib.font_manager import FontProperties

fontP = FontProperties()
fontP.set_size('small')


def main(argv):
    args = process_args(argv)
    #letters = ["train/sharad-new/"+x for x in args]
    letters = [""+x for x in args]
    #letters = ["data_train/"+x for x in args]
    #letters = ["train/lei-1354774953143/"+x for x in args]
    to_plot = []

    plot = plt.subplot(1,1,1)
    for i in range(len(letters)):
        data = np.loadtxt(letters[i],delimiter=" ");
        x = [point[0] for point in data]
        y = [point[1] for point in data]
        plot.plot(x,y, label=args[i],linewidth=8)

    #plot.legend(shadow=True,fancybox=True,loc='upper center',bbox_to_anchor=(0.5,-0.1),ncol=4)
    plot.set_aspect('1.0')
    #subplots_adjust(bottom=0.25)   # <--
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
