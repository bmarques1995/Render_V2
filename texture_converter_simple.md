# DDS

`./texconv -f R8G8B8A8_UNORM_SRGB -m 0 <texture_file> -o <output_dir>`

## Another Args: 

- `-m 0`: generates all mips
- `-f`: tells the output format

# KTX

`./toktx --genmipmap --encode uastc <output_file> <texture_file>`

## Another Args: 

- `--genmipmap`: generates all mips
- `--encode uastc`: tells the output format