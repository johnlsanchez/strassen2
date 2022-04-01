import random

file1 = open("15x15.txt", "w")
for i in range(15 * 15 * 2):
    file1.write(str(random.randint(0, 1)) + "\n")