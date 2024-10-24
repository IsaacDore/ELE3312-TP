import random

seed = random.randint(0,2**32-1);

print(
    "#ifndef SEED_H\n"
    "#define SEED_H\n"
    f"const uint32_t SEED = {seed};\n"
    "#endif"
    )