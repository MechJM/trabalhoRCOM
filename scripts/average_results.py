file = open("file.txt","r+")
index = 0
lines = file.readlines()
result_file = open("result.txt","w")
for line in lines:
    if (index) % 3 == 0:
        line1 = line
        line2 = lines[index + 1]
        line3 = lines[index + 2]
        split_line1 = line1.split(',')
        split_line2 = line2.split(',')
        split_line3 = line3.split(',')
        val1 = split_line1[0]
        val2 = split_line1[1]
        val3s = []
        val3s.extend([split_line1[-1],split_line2[-1],split_line3[-1]])
        avg = (float(val3s[0]) + float(val3s[1]) + float(val3s[2]))/3
        line1 = line1.replace(split_line1[-1],str(avg))
        result_file.write(line1 + "\n")
    index += 1


file.close()
result_file.close()