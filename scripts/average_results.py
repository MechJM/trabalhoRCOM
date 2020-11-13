import sys

src_file_name = sys.argv[1]
src_file = open(src_file_name,"r")
dest_file_name = "avg_" + src_file_name.split('.')[0] + '.' + src_file_name.split('.')[1]
print(dest_file_name)
dest_file = open(dest_file_name,"w")

avg_num = int(sys.argv[2])
num_val_keep = int(sys.argv[3])

index = 0
src_lines = src_file.readlines()
for i in src_lines:
    current_line = src_lines[index]
    if index % avg_num == 0:
        values_to_keep = []
        values_to_avg = []
        line_values = current_line.split(',')
        for j in range(num_val_keep):
            values_to_keep.append(line_values[j])
        for j in range(avg_num):
            values_to_avg.append(float(src_lines[index + j].split(',')[-1]))
        total = 0
        for j in values_to_avg:
            total += j
        avg = total / avg_num
        result_line = ""
        for j in values_to_keep:
            result_line += j + ','
        result_line += str(avg)
        dest_file.write(result_line + "\n")
    index += 1

src_file.close()
dest_file.close()