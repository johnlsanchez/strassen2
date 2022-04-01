import random

file1 = open("2x2.txt", "w")
for i in range(2 * 2 * 2):
    file1.write(str(random.randint(-1, 1)) + "\n")