# Face Remove

Given the sizes, positions and orientations of cubes in 3D space, finds cube faces which are hidden from view and removes them. Disabling rendering for hidden faces nets a significant performance boost with no change in appearance.

<figure>
  <img
  src="face-remove_1.png"
  width="300"
  alt="Cubes in various positions and orientations, some with their faces hidden">
  <figcaption>Cubes in various positions and orientations, some with their faces hidden</figcaption>
</figure>

<figure>
  <img
  src="face-remove_2.png"
  width="300"
  alt="Multiple cubes together cover up the orange cubes face">
  <figcaption>Multiple cubes together cover up the orange cubes face</figcaption>
</figure>


## Usage

Run it in your browser [here](https://htmlpreview.github.io/?https://github.com/Infinitifall/face-remove/blob/main/index.html). Note that it expects the position of a cube to be the center of its bottom face (instead of its geometric center) and the input/outputs look like this

```json
{
    "objects":[
        {
            "s":[1,2,3.5],
            "p":[0.2,10.5,-1],
            "r":[0,1.57,-0.5],
            "f":"1C"
        },
        {
            "s":[7,6,5],
            "p":[0,100,0],
        },
    ]
}
```


## How it works

To determine if a face is hidden we check if all points on an $n \times n$ grid are touching or covered by another cube. We can get an arbitrarily accurate (albeit computationally heavy) solution as $n \rightarrow \infty$ (for most scenarios though, $n \approx 10$, checking 100 points per face, works just fine).

For an in depth breakdown, see the [blog post](https://infinitifall.net/scripting/face-remove.html).


