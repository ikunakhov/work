import matplotlib.pyplot as plt
import os
import numpy as np

def norm_ar(in_ar1, in_ar2):
    n1 = np.argmax(in_ar1)
    tmp = in_ar1[n1]
    if n1 == 0:
        return
    for iterator in range(len(in_ar1)):
        in_ar1[iterator] /= tmp
        in_ar1[iterator] *= in_ar2[n1]

    mas_sub(in_ar1, in_ar2)
    return n1

def mas_sub(in_ar1, in_ar2):
    for i in range(min(len(in_ar1), len(in_ar2))):
        in_ar2[i] -= in_ar1[i]
    return



path = '~/Документы/fft_gcc/'
cons = 'a.out'
x = []
y = []
x_tmp2 = []
high_level = []
input_smpl ='1 0 0 0 0 0 1 1' #1 0 0 0 0 0 1 1
input_ph = []
j = 0
tmp = 0
n = 0
while j < 30:
    x_tmp = [float(s) for s in input_smpl.split(" ")]
    while n < len(x_tmp):
        cnst = 0.0
        if j == 0:
            cnst = 0.0
        else:
            cnst = 0.5
        tmp_tmp = j
        tmp_tmp+=cnst
        input_ph.append(str(tmp_tmp)+' ')
        n += 1
    data = os.popen(path + cons + ' ' + str(len(x_tmp)) + ' ' + input_smpl + ' ' + str(input_ph)).read()
    x_tmp2 = [float(s) for s in data.split(',')]
    input_ph.clear()

    if  j == 0 :
        x = x_tmp2
        for iter_ in range(len(x)):
            high_level.append(x[iter_])
    else:
        norm_ar(x_tmp2, x)

    n = 0
    j += 0.5
cnt_tmp = 0
y_hlevel = []
for cnt in range(len(x)):
    y.append(cnt_tmp)
    y_hlevel.append(cnt_tmp+0.05)
    cnt_tmp+=1

plt.xlabel('time',size =14)
plt.ylabel('samples', size = 14)
plt.bar(y,x,0.02, color = 'b')
plt.bar(y_hlevel,high_level, 0.02, color = 'r')
#plt.plot(y, x, label = 'samples')
plt.legend(loc='upper left')
plt.show()
