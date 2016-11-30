#!/usr/bin/python2
import matplotlib.pyplot as plt

if __name__ == "__main__":
    infoFile = "/home/ivan/git_work/work/Projects_C_C++/cholesky/tests/test.txt"
    f = open(infoFile, "r")
    i = 0
    x_cord = []
    y_cord = []
    lst_str = []
    label= []
    for line in f:
        tmp = line.split(" ")
        length = len(tmp)
        tmp[length-1] = tmp[length-1][0:3]
        if i is 0:
            for point in tmp:
                x_cord.append(point)
        else:
            for point in tmp:
                lst_str.append(point)
            y_cord.append(lst_str)
            lst_str = []
            label.append("cores "+str(i))
        i+=1

    k = 0
    plt.ylabel('TIME', size=14)
    plt.xlabel('MATRIX SIZE', size=14)
    while k < (i-1):
        plt.plot(x_cord, y_cord[k], label=label[k])
        k+=1
    plt.legend(loc='upper left')
    plt.show()

