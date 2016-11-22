import ctypes as ct
import matplotlib.pyplot as plt
import math as m
import numpy  as np
from scipy.fftpack import fftshift


def my_cast(ptr, length,  mass):
    for i in range(length):
        mass.append(ptr[i])


def cur_distr(q, rg):
    M = rg -1
    y = m.cos(((q - (M - 1)/2)*m.pi)/(M - 1))*m.cos(((q - (M - 1)/2)*m.pi)/(M - 1))+ 0.2
    return y

def han_window(q, rg):
    y = 0.5*(1- m.cos(2*m.pi*q/rg))
    return y

def find_extr(in_arr, extr_arr):
    for i in range(len(in_arr) - 2):
        if (in_arr[i+1] > in_arr[i]) and (in_arr[i+1] > in_arr[i+2]):
            extr_arr.append(1)
        elif (in_arr[i+1] < in_arr[i]) and (in_arr[i+1] < in_arr[i+2]):
            extr_arr.append(-1)
        else:
            extr_arr.append(0)

def aprox_window_lr(y, x, main_y, ind):
    a = (x[len(x) - 1] -  x[0])
    cpy = ind
    for i in range(len(x)):
        main_y[ind] = (-(x[i] - x[0])/(2*a) + 1)*y[i]
        ind+=1
    m = np.min(main_y)
    while (cpy != ind):
        main_y[cpy] += m/2
        cpy+=1

def aprox_window_rl(y, x, main_y, ind):
    a = (x[len(x) - 1] -  x[0])
    cpy = ind
    for i in range(len(x)):
        main_y[ind] =((x[i] - x[0])/(2*a) + 1/2)*y[i]
        ind+=1
    m = np.min(main_y)
    while (cpy != ind):
        main_y[cpy] += m / 2
        cpy += 1

x = []
y = []
fi = []
rg = 64
fft_points = 512
tmp = 0

for i in range(rg):
    x.append(i)
    y.append(cur_distr(i, rg))
    fi.append(tmp)
    tmp+=0


n = len(y)
invert = 0;
plt.xlabel('x',size =14)
plt.ylabel('y', size = 14)
plt.plot(x,y)
plt.show()
y_tmp = y
y = y + fi

lib = ct.CDLL( './fft_libr.so' )
pfun = lib.exec
pfun.argtypes =[ct.c_int,ct.c_int, ct.c_int, ct.POINTER(ct.c_double), ct.POINTER(ct.c_double)]

doubleP = (ct.c_double * len(y))(*y)
d = (ct.c_double * fft_points)()
ct.cast(d, ct.POINTER(ct.c_double))

pfun(n, fft_points, invert, doubleP, d)
mem = []
my_cast(d, fft_points, mem)
freq = np.linspace(-rg, rg, fft_points)

extr_arr = []

response = 20*np.log10((mem/ np.max(mem)))
find_extr(response,extr_arr)

tmp_arr_y = []
tmp_arr_x = []
load_m = 0
tmp_point = 0
i = 0
j = 0
plt.xlabel('frequency',size = 14)
plt.ylabel('dB', size = 14)
erl = fftshift(response)
plt.plot(freq, erl)


while i < (len(extr_arr)):
    if (extr_arr[i] == -1):
        load_m = i
        while ((extr_arr[i] != 1) and (i < (len(extr_arr) - 1))):
            tmp_arr_y.append(extr_arr[i])
            tmp_arr_x.append(freq[i])
            i+=1
            j+=1
        if (i != (len(extr_arr) - 1)):
            aprox_window_lr(tmp_arr_y, tmp_arr_x, response, load_m)
        j= 0
        tmp_arr_x = []
        tmp_arr_y = []
    elif (extr_arr[i] == 1):
        load_m = i
        while ((extr_arr[i] != -1)  and (i < (len(extr_arr) - 1))):
            tmp_arr_y.append(extr_arr[i])
            tmp_arr_x.append(freq[i])
            i+=1
            j+=1
        if (i != (len(extr_arr) - 1)):
            aprox_window_rl(tmp_arr_y, tmp_arr_x, response, load_m)
        j= 0
        tmp_arr_x = []
        tmp_arr_y = []
    else:
        i+=1
response = fftshift(response)
plt.plot(freq,response)
plt.show()


fft_points = 512
n = len(response)
invert = 1
for i in range(len(response)):
    np.append(response, 0)
doubleP = (ct.c_double * len(response))(*response)
d = (ct.c_double * fft_points)()
ct.cast(d, ct.POINTER(ct.c_double))
pfun(n, fft_points, invert, doubleP, d)
mem = []
my_cast(d, fft_points, mem)
freq = np.linspace(-rg, rg, fft_points)
plt.xlabel('x',size =14)
plt.ylabel('y', size = 14)
plt.plot(freq,mem)
plt.show()