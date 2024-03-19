v_types = [["mc_vec", "float"], ["mc_ivec", "int"], ["mc_uvec", "mc_uint"]]
v_element_names = [
    ["x", "y", "z", "w"],
    ["r", "g", "b", "a"],
    ["_0", "_1", "_2", "_3"],
]

print("#ifndef MC_VEC_H_INCLUDE_GUARD")
print("#define MC_VEC_H_INCLUDE_GUARD\n")

# define uint
print("typedef unsigned int mc_uint;\n")

# define vector types
for v_type in v_types:
    for i in range(2, 5):
        print("typedef union {")

        for j in range(len(v_element_names)):
            print(f"    struct {{")
            for k in range(i):
                print(f"        {v_type[1]} {v_element_names[j][k]};")
            if i == 3 and v_element_names[j][k][0] == "_":
                print(f"        {v_type[1]} _padding;")
            print(f"    }};")

        print(f"}} {v_type[0]}{i};\n")

# short vector types
print("#ifdef MC_VEC_SHORT_NAMES")
print("#define uint mc_uint")

for v_type in v_types:
    for i in range(2, 5):
        print(f"#define {v_type[0].split('_')[1]}{i} {v_type[0]}{i}")

print("#endif\n")
print("#endif\n")
