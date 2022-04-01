import random

file1 = open("1024x1024.txt", "w")
for i in range(1024 * 1024 * 2):
    file1.write(str(random.randint(0, 1)) + "\n")