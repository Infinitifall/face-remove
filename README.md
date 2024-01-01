# Face Remove

Given the sizes, positions and orientations of cubes in 3D space, finds cube faces which are hidden from view and removes them. Disabling rendering for hidden faces nets a significant performance boost with no change in appearance.

A significant improvement over the Krunker Editor face optimizer, which can only handle one-on-one face overlaps.


## Setup

`python3` and `gcc` must be installed

```bash
# clone repo
git clone https://github.com/Infinitifall/face-remove
cd face-remove

# build
make
```

## Run

Paste your map export in `input.json`

```bash
# run
python3 run.py
```


## How it works

To determine if a face is hidden we check if all points on an $n \times n$ grid are touching or covered by another cube. We can get an arbitrarily accurate (albeit computationally heavy) solution as $n \rightarrow \infty$. For most scenarios though, $n \approx 20$, checking 400 points per face, works just fine, which is what we do here.


In the image below, the yellow cube and the blue cube will have one face each disabled, as they are hidden inside the purple and the green cubes respectively.

![Cubes in various positions and orientations, some with their faces hidden](./face-remove_1.png)


In the image below, the orange cube will have one face disabled, as it is hidden by the other three cubes put together.

![Multiple cubes together cover up the orange cubes face](./face-remove_2.png)

