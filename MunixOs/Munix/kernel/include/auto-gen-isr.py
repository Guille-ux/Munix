to_file = input("File Name : ")
num_handlers = int(input("Number of ISR Handlers: "))

with open(to_file, "w") as f:
    f.write("#ifndef __ISR_AUTO_GENERATED_H__\n")
    f.write("#define __ISR_AUTO_GENERATED_H__\n")
    for i in range(num_handlers):
        f.write(f"extern void isr{i}();\n")
    f.write("\n")
    f.write("#endif\n")
