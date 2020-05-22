import os
import subprocess
import time

import matplotlib.pyplot as plt
import numpy as np

srv = [1]
cl = [1, 5, 10, 25, 50, 100]

results = {}
labels = []

def draw():
	plt.rcdefaults()

	x = np.arange(len(labels))
	width = 0.3

	fig, ax = plt.subplots()

	rects = []
	for i in range(len(cl)):
		bar_offset = ((i - len(cl) // 2) * width) / len(cl)
		rects.append(ax.bar(x + bar_offset, results[cl[i]], width / len(cl), label='clients = {}'.format(cl[i])))

	ax.set_ylabel('Messages per Second/MPS')
	ax.set_title('SystemV MQ Bench')
	ax.set_xticks(x)
	ax.set_xticklabels(labels)
	ax.legend()

	def autolabel(rects):
	    for rect in rects:
	        height = rect.get_height()
	        ax.annotate('{}'.format(height),
	                    xy=(rect.get_x() + rect.get_width() / 2, height),
	                    xytext=(0, 3),  # 3 points vertical offset
	                    textcoords="offset points",
	                    ha='center', va='bottom')

	for i in rects:
		autolabel(i)

	fig.tight_layout()
	plt.show()


def bench(affinity={}):
	labels.append(str(affinity))

	for c in cl:
		subprocess.run(['taskset', '-a', '-cp', affinity['srv'], str(os.getpid())], stdout=subprocess.DEVNULL)
		servers = [subprocess.Popen('/home/artur/queue/srv', stdout=subprocess.PIPE) for i in srv]

		subprocess.run(['taskset', '-a', '-cp', affinity['cl'], str(os.getpid())], stdout=subprocess.DEVNULL)
		clients = [subprocess.Popen('/home/artur/queue/bcl', stdout=subprocess.DEVNULL) for i in range(c)]

		subprocess.run(['taskset', '-a', '-cp', '0-3', str(os.getpid())], stdout=subprocess.DEVNULL)
		time.sleep(3)

		for _c in clients:
			_c.kill()

		for _s in servers:
			_s.kill()

		mps = []
		for s in servers:
			output = s.stdout.readlines()

			_mps = [int(score) for score in output]
			mps.append(sum(_mps) / len(_mps))

		if c in results:
			results[c].append(mps[0])
		else:
			results[c] = [mps[0]]

		time.sleep(1)

	print(str(affinity) + ' passed...')
# --------------------
subprocess.run(['killall', '-9', 'bcl'], stdout=subprocess.DEVNULL)
subprocess.run(['killall', '-9', 'srv'], stdout=subprocess.DEVNULL)


bench(affinity={'srv': '0', 'cl': '0'})
bench(affinity={'srv': '0', 'cl': '1'})
bench(affinity={'srv': '0', 'cl': '0-1'})
bench(affinity={'srv': '0-1', 'cl': '0-1'})
bench(affinity={'srv': '0', 'cl': '1-2'})
bench(affinity={'srv': '0', 'cl': '0-2'})
bench(affinity={'srv': '0-1', 'cl': '0-2'})
bench(affinity={'srv': '0-2', 'cl': '0-2'})

# transform_data()

draw()
