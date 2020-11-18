import sys


def remove_newline(str):
    result = ""
    for i in str:
        if i != "\n":
            result += i
    return result

src_file_name = sys.argv[1]
src_file = open(src_file_name,"r")
dest_file_name = "avg_" + src_file_name.split('.')[0] + '.' + src_file_name.split('.')[1]
print(dest_file_name)
dest_file = open(dest_file_name,"w")

avg_num = 3#int(sys.argv[2])
num_val_keep = 4#int(sys.argv[3])

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
        elems_to_count = 0
        for j in range(avg_num):
            elem_to_append = remove_newline(src_lines[index + j].split(',')[-1])
            if elem_to_append != "-1":
                values_to_avg.append(float(elem_to_append))
                elems_to_count += 1
        if len(values_to_avg) != 0:
            total = 0
            for j in values_to_avg:
                total += j
            avg = total / elems_to_count
        else:
            avg = -1
        result_line = ""
        for j in values_to_keep:
            result_line += j + ','
        result_line += str(avg)
        dest_file.write(result_line + "\n")
    index += 1

src_file.close()
dest_file.close()