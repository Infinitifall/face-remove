# Face Remove

Given the sizes, positions and orientations of cubes in 3D space, finds cube faces which are hidden from view and removes them. Disabling rendering for hidden faces nets a significant performance boost with no change in appearance.


## Setup

- [Python 3](https://www.python.org/downloads/) and [gcc](https://gcc.gnu.org/) (or any other C compiler) must be installed

- Clone this repository and make

  ```
  git clone https://github.com/Infinitifall/face-remove
  cd face-remove
  make
  ```


## Usage

1. Paste your map export in `input.json`
2. Run `run.py`
3. The optimized map export will be in `output.json` 


## How it works

To determine if a face is hidden we check if all points on an $n \times n$ grid are touching or covered by another cube. We can get an arbitrarily accurate (albeit computationally heavy) solution as $n \rightarrow \infty$ (for most scenarios though, $n \approx 10$, checking 100 points per face, works just fine).

<figure>
  <img
  src="face-remove_1.png"
  width="300"
  alt="Cubes in various positions and orientations, some with their faces hidden">
</figure>

<figure>
  <img
  src="face-remove_2.png"
  width="300"
  alt="Multiple cubes together cover up the orange cubes face">
</figure>
