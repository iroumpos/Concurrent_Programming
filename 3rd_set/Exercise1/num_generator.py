import random

with open('numbers.txt', 'w') as f:
    for i in range(0, 100):
        number = random.randint(1, 100)
        f.write('%d' % number)
        f.write(' ')