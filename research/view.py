from matplotlib import pyplot as plt
import math

N_MAX = 1000

data_no_d = []
with open('logs_no_daemon.txt', 'r') as file:
    for line in file:
        num = int(line.split()[-2])
        data_no_d.append(num)

data_no_d = data_no_d[:N_MAX]
data_no_d = sorted(data_no_d)
mean = sum(data_no_d) / len(data_no_d)
print('data_no_d mean is', mean)
print('sample len', len(data_no_d))

data_d = []
with open('logs_daemon.txt', 'r') as file:
    for line in file:
        num = int(line.split()[-2])
        data_d.append(num)

data_d = data_d[:N_MAX]
data_d = sorted(data_d)
mean = sum(data_d) / len(data_d)
print('data_d mean is', mean)
print('sample len', len(data_d))

plt.hist(data_no_d, alpha=0.5, bins=math.floor(len(data_no_d) / 50), label='демон остановлен')
# plt.plot(list(range(N_MAX)), data_no_d, label='демон остановлен')

plt.hist(data_d, alpha=0.5, bins=math.floor(len(data_d) / 50), label='демон запущен')
# plt.plot(list(range(N_MAX)), data_d, label='демон запущен')

plt.xlabel('время выполнения (нс)')
plt.ylabel('количество обработанных URB пакетов')
plt.legend()
plt.grid()
plt.show()
