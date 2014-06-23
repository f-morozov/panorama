# -*- coding: utf_8 -*-

from pylab import *

res = []
lines = open('stats.txt').readlines()
for line in lines:
	sp = line.split()
	s = ""
	if(sp[0] == 'SURF'):
		s = sp[1]
	else:
		s = sp[1]+'*'
	res.append([s] + [float(x) for x in sp[2:]])

lab_blue = '#1e61b6'
left = np.array(range(len(res)))

res = sorted(res, key=lambda line: line[2])
right = np.array([x[2] for x in res])

bar(left, right, facecolor=lab_blue, edgecolor='white')
for x,y in zip(left,right):
    text(x + 0.4, y + 0.01, res[x][0], ha='center', va= 'bottom')

xlabel(u'Descriptor')
ylabel(u'Score')
title(u'Results')

tick_params(\
    axis='x',          
    which='both',      
    bottom='off',      
    top='off',       
    labelbottom='off')
	
ylim(0.6, 1)
show()
clf()

colors = ['r', 'g', 'b', 'y']
tmp = []
for i in range(4):
	tmp.append(scatter(res[i][5], 1 - res[i][2], 150, marker = '^', color = colors[i]))
for i in range(4):
	tmp.append(scatter(res[i+4][5], 1 - res[i+4][2], 150, marker = 'o', color = colors[i]))

ylim(0, 0.5)
xlim(0, 1.5)
legend(tmp, [x[0] for x in res], scatterpoints=1, loc='upper right', ncol=2, fontsize=14)
xlabel(u'Time on one set, ms')
ylabel(u'Error')
title(u'Error vs. time')

show()

clf()

res = sorted(res, key=lambda line: line[3])
right = np.array([x[3] for x in res]) * 1000

right1 = []
right2 = []
text1 = []
text2 = []
for i in range(len(right)):
	if right[i] < 20:
		right1.append(right[i])
		text1.append(res[i][0])
	else:
		right2.append(right[i])
		text2.append(res[i][0])

bar(np.array(range(len(right1))), right1, facecolor=lab_blue, edgecolor='white')

for x,y in zip(left,right1):
    text(x + 0.4, y + 0.01, text1[x], ha='center', va= 'bottom')

tick_params(\
    axis='x',          
    which='both',      
    bottom='off',      
    top='off',       
    labelbottom='off')
	
ylim(0, 10)
show()
clf()

print right
bar(np.array(range(len(right2))), right2, facecolor=lab_blue, edgecolor='white')

for x,y in zip(left,right2):
    text(x + 0.4, y + 0.01, text2[x], ha='center', va= 'bottom')

xlabel(u'Descriptor')
ylabel(u'Computation Time, µs')
title(u'Computation time')

tick_params(\
    axis='x',          
    which='both',      
    bottom='off',      
    top='off',       
    labelbottom='off')
	
ylim(0, 200)
show()