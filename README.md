### Installing sdl2 and sdl2_ttf:
1. `$ sudo apt-get install libsdl2-dev`
2. `$ sudo apt-get install libsdl2-ttf-dev`

### Preparing data from the `raw_data` directory and storing the preprocessed data to a newly-created `data` directory:
`$ ./data_cooker.sh`

### To visualize for any file, modify the line in `main.c`
`load_animation_for_data("../data/r101");`
